
void setupServer();
void creatSocket();	
void bindSocket();
void sockListening();
void srvrlogging(int i, char *msg);
void acceptConnection();
void startServer();
void reaper(int sig);
void invokeCGI(int socket, char *args[]);
void getServerConfig();
void sendError(int socket, char *errMsg);
#define BUFSIZE 2*1024


