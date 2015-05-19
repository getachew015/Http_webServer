#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include "webServer.h"

int mSock,sSock; int port; int maxConn;//will get from httpd.conf


void setupServer(){//Get Server configuration information
	FILE *fp;
	char *webroot; char *conf_file;  char *line = malloc(100);
	webroot = malloc(100);
	// Setup default values
	conf_file = "../config_files/httpd.conf";
	fp = fopen(conf_file, "r");
	if (fp == NULL)
	{ srvrlogging(-1,strerror(errno));  exit(1); }
	fscanf(fp, "%s %s", line, webroot);
	fscanf(fp, "%s %i", line, &port);
	fscanf(fp, "%i", &maxConn);
	fclose(fp);
	free(line);
}

void creatSocket ()
{
	mSock = socket(AF_INET, SOCK_STREAM, 0);//TCP socket
	if ( mSock == -1 )
		{  srvrlogging(-1,strerror(errno));  exit(-1);	}
}

void bindSocket()//Bind the socket to the port specified in config file
{
	int bound;
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	bound=bind(mSock, (struct sockaddr *)&address, sizeof(address));
	if ( bound < 0 )
		{ srvrlogging(-1,strerror(errno)); exit(-1); }
}

void sockListening()// Make socket to be passive/in listening mode for maxConn
{
	int listner;
	listner=listen(mSock, maxConn);
	if (listner < 0) 
		{ srvrlogging(-1,strerror(errno)); exit(-1); }
}

void invokeCGI(int socket, char *args[]){
	close(1);
	dup2(socket,1);
	int rc = execvp("./cgi", args);	
	if(rc < 0)//send 500 error message if exec failes
		sendError(socket,"500 Internal Server Error");
}

int recieve(int socket){
	char *url;
	char buffer[BUFSIZE];
	memset (buffer,'\0', BUFSIZE);
	ssize_t sSize;
	if (socket<0) 
		{  srvrlogging(-1,strerror(errno)); return -1;}
	sSize = recv(socket, buffer, BUFSIZE, 0);
	if (sSize < 0) 
		{ srvrlogging(-1,strerror(errno)); return -1;}
	else {
		url = malloc(BUFSIZE);
		strcpy(url,buffer);
		char *request[]={url,NULL};    
		invokeCGI(socket, request); 
		return 0;
	}
}

void reaper(int sig){//catch child's termination Signal
	int status;
	while(wait3(&status, WNOHANG,(struct rusage *)0)>=0);
}

void acceptConnection(){
	struct sockaddr_storage connector;
	int addrlen;
	addrlen = sizeof(connector);
	sSock = accept(mSock, (struct sockaddr *)&connector, &addrlen);
	if ( sSock < 0 ) srvrlogging(-1,strerror(errno)); 
	switch (fork())   {
	case 0: //child
		close(mSock);//close child's listening socket inherited from parent
		recieve(sSock);
		exit(0);
	default: //parent
		close(sSock);
		break; 
	case -1: //fork failed
		srvrlogging(-1,strerror(errno));
		exit(-1);   
	}
}

void startServer(){
	/* Server Algorithm disigned as stated in text book "Internetworking with TCP/IP Volume 3 by ​Douglas E. Comer" Algorithm 8.1
	   Socket() --> Bind() --> Listen() --> Accept( [ Recieve() --> Send() --> Close() ] )--> stay in accepting mode */	
	creatSocket();	
	bindSocket();
	sockListening();
	(void) signal(SIGCHLD, reaper);//Eliminate a zombie process 
	while (1){//accept connections in an infinite loop
		acceptConnection();
	}
}

void main()
{	
	setupServer();
	printf("Web Server is Running on Localhost Port# %d\n", port);	
	startServer();
}


