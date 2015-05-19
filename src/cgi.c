#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include  <fcntl.h>
#define BUFSIZE 1024

char *requestFile; char *mimeType; char *webroot; 
char *tokens[5]; 
char *url;
FILE *fp;

void getServerConfig(){ // Get root directory of webpage files
	char *config_file;
	char	*line = malloc(100);
	webroot = malloc(100);
	config_file = malloc(50);
	config_file = "../config_files/httpd.conf";
	fp = fopen(config_file, "r");
	fscanf(fp, "%s %s", line, webroot);
	fclose(fp);
	free(line);
}

void srvrlogging(int i, char *msg){ //A function for logging server activities
	char *access_log; char *error_log;
	FILE *fpacc, *fperr;
	access_log = "../log/accesslogs.log";  error_log = "../log/errorlogs.log";
	time_t t; 	
	time(&t);
	if (i==0){
	fpacc=fopen(access_log, "a+");
	fprintf(fpacc,"\nServer got Accessed on %s Request was %s %s",ctime(&t), tokens[0], msg);
	fclose(fpacc);
	}
	else if(i==-1){
	fperr = fopen(error_log, "a+");
	fprintf(fperr,"%s occured on %s ",msg, ctime(&t));
	fclose(fperr);
	} 
}

int getRequestMethod(char *request){ //identify incoming request
	int i=0; char *substr;
	if(strlen(request)>0){
		for (char *ptr=strtok(request," "); ptr!= NULL; ptr=strtok(NULL, " "))//tokenize url
			{ char *pass=strdup(ptr); tokens[i]=pass; i++; 	}
	}
	requestFile=malloc(BUFSIZE);
	strcpy(requestFile,tokens[1]);
	if(strcmp("GET", tokens[0])==0) 
		return 1;
	else if (strcmp("POST", tokens[0])==0)
		return 2;
	else if (strcmp("HEAD", tokens[0])==0)
		return 3;
	else 
		return -1;
}

int checkUrl(char *request){ //validate request
	char *mime;
	if(strstr(request,"/cgi-bin/")==NULL) return -1;
	if(strstr(request,".")==NULL) return -1;
		mime=strtok(request, "."); 
		mime=strtok(NULL, ".");
	if(mime==NULL) return -1;
	else {mimeType=mime; return 0;}	
}

char * getContent(char *type){//mime's supported by my server
	if (strcmp(type,"html")==0 || strcmp(type,"htm")==0) return "text/html";
	else if (strcmp(type,"gif")==0 ) return "image/gif";
	else if (strcmp(type,"jpeg")==0 || strcmp(type,"jpg")==0 || strcmp(type,"jpe")==0) return "image/jpeg";
	else if (strcmp(type,"psd")==0) return "image/vnd.adobe.photoshop";
	else return "MIME is OK! 400 But Not Supported on My server :)";
}


char * checkMime(char *request){
	char *mime_file; 
	char *retValue; char *line; size_t len=0, read; 
	retValue="Error 400 Bad Request";
	if(checkUrl(request)<0) return retValue;
	else{
		mime_file= "../config_files/mime.types";
		fp = fopen(mime_file, "r");
	while((read = getline(&line, &len, fp)) != -1){
	if((strstr(line,"#")==NULL)){ 
		if (strstr(line,mimeType)!=NULL) {  retValue = getContent(mimeType); fclose(fp); return retValue; } 
		} //if
	}//while
	}//else 
	return retValue;
}

char * getFile(char *request){
	FILE *fp; int i, j=0; char *path; char *fileName;
	size_t len; 
	getServerConfig();
	fileName=malloc(BUFSIZE);
	strcpy(fileName,request);
	for(i=strlen("/cgi-bin/");i<strlen(request);++i){
		if(j<i) { fileName[j]=fileName[i]; j++; }			
	}
	fileName=strndup(fileName, strlen(fileName)-strlen("/cgi-bin/"));
	path = malloc(BUFSIZE);
	strcpy(path,webroot); strcat(path,fileName);
	path=strndup(path, strlen(webroot)+strlen(fileName));
	return path;
	free(path);
	free(fileName);		
}

void writeHttpHead(char *type, int len){// write Http Header
	time_t rawtime;
	printf("\nHTTP/1.0 200 OK");
	printf("\nContent-Type: %s", type);
	printf("\nServer: INET-Server v/0.1");
	printf("\nContent-Length: %i", len);
//	printf("\nDate: %s", ctime(&rawtime));
	printf("\n\n");
}

char * writeFile(char *file){//write files on socket/STD OUT
	FILE *webFp; char ch; 
	webFp = fopen(file,"r");
	do { 
		ch = fgetc(webFp); 
	if( feof(webFp) ) break ;
		printf("%c", ch);
	}while(1);
	fclose(webFp);
}

int getFileSize(char *file){
	int size=0;
	struct stat st;
	stat(file, &st);
	size = st.st_size;
	return size;
}

char * getStatus(){
	char *notFound = "Error 404 Not Found"; char *ok = "200 OK"; 
	struct stat st;
	int result = stat(getFile(requestFile), &st);	
	if(result==0 ) return ok;
	else  return notFound;
}

void httpGET(){
	char *mimeContent; char *fileStatus="Error 404 Not Found";
	mimeContent=(checkMime(tokens[1]));
	if(strstr(mimeContent,"400")!=NULL) { 
		printf("HTTP/1.0 %s \nContent Type: text/plain\n\n%s",mimeContent,mimeContent); 
		srvrlogging(-1,mimeContent);  
		}
	else if (strcmp(getStatus(), fileStatus)==0) { 
		printf("HTTP/1.0 %s \nContent Type: text/plain\n\n%s",getStatus(),getStatus()); 
		srvrlogging(-1,fileStatus); 
		}
		else 
		{ 	
			srvrlogging(0,getFile(requestFile));
			writeHttpHead(mimeContent, getFileSize(getFile(requestFile)) );
			writeFile( getFile(requestFile) );
		}
}

void htmlPage(char *name, int salary){
	printf("\nHTTP/1.0 200 OK");
	printf("\nContent-Type: text/html");
	printf("\nServer: INET-Server v/0.1");
	printf("\n\n");
	printf("<html> <head> <title>Posting Your Result </title> </head> ");
	printf("<body> <h4> Your TAX Information is as follows </h4>");
	printf("<p> Dear <b> %s</b> Thank you for using our online secure Tax service </p>", name);
	printf("<p> Your Yearly Gross Income is <b> %i </b> and you owe a tax amount of <b> %.2f</b> </p>", (salary*12), (salary*0.15));
	printf("<p> If You Have Any Question Please give us a call <b> 800-582-9758 </b> </p> </body> </html>");
	
}

void httpPOST(char *buffer){
	char *values; char *line; size_t len=0, read;
	char *name; char *key; int salary;
	values=malloc(100); 
	FILE *temp;
	srvrlogging(0,buffer); 
	temp=fopen("temp.txt", "a+");
	fputs(buffer,temp);
	fclose(temp);
	temp=fopen("temp.txt", "r");
	while((read = getline(&line, &len, temp)) != -1 ){
	if (strstr(line,"fname")!=NULL) strcpy(values,line);
	}
	//************************ Extract Values ***************************//
	key=malloc(50);
	strcpy(key,values);
	name=strtok(values,"&");
	key=strtok(key,"&");
	key=strtok(NULL," "); key=strtok(key,"="); key=strtok(NULL," "); salary=atoi(key);
	name=strtok(name,"="); name=strtok(NULL," ");
	//********************** Pass Values Back To Browser *******************//
	htmlPage(name,salary);
	remove("temp.txt");
}


void main (int argc, char *argv[] ){

//*********** Function Proto Type ******************
void getServerConfig();
void srvrlogging(int i, char *msg);
int getRequestMethod(char *request);
int checkUrl(char *request);
char * getContent(char *type);
char * checkMime(char *request);
char * getFile(char *request);
void writeHttpHead(char *type, int len);
char * writeFile(char *file);
int getFileSize(char *file);
char * getStatus();
void httpGET();
void htmlPage(char *name, int salary);
void httpPOST(char *buffer);
//***************** Main Body *********************

	int method;
	url=malloc(BUFSIZE);
	strcpy(url,argv[0]);
	method=getRequestMethod(url);
	if(method==1) 	httpGET();
	else if(method==2) httpPOST(argv[0]);
	else printf("%s","Error 501 Not Implemented");
	free(url);
	exit(0);
}

