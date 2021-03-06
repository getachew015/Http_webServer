Network Programming Project1
HTTP 1.0 Server With CGI and Concurrency

READ ME

This document is included as a Read Me file for the HTTP 1.0 server written in C
Note: the web files included in the webroot folder are downloaded from http://www.free-templates.me/

I. Files and Web-Server Directory Structure
The web server has a directory structure to hold its important files; the folder webserver is the main
folder which holds the other sub-folders which are cgi-bin, src, www, config_files and log. In the cgi-bin
folder there are two important executable files one is cgi which performs a CGI functionality as specified in the
RFC 3875 and webServer which is the web server program. The src folder contains all source code files; 
if in case any editing is performed on the source codes the user has to run make clean and make command from the src directory in the terminal. The config_files folder contains a configuration file called httpd.conf and mime.types file which 
holds mime types and corresponding type/subtype header. 
The httpd.conf file holds important setup information for the server and CGI. The logs folder holds two files called accesslogs.log
and errorlogs.log file which are used to hold the files accessed and the errors occurred during server client
communication. The last folder is the www folder which is a root directory for all the html/web page files that
can be accessed by sending a request to the server. This folder contains an index.html file which enables the user
to browse through all the other web files.

II. Server Algorithm
The server is designed to be an HTTP web server and it is written in c language. The algorithm is made
to adhere to RFC/1945 HTTP 1.0 standard. Communication with the server is enabled through the socket API
which uses a TCP/IP protocol to communicate with peers. The algorithm is also designed to be a concurrent
connection-oriented server. When the web server program starts execution it performs configuring the default values using the
function void setupServer() which mainly opens the server configuration file from the httpd.conf file and assign
the default values for the port to open a socket on, the root directory for its files and maximum number of
connections allowed. And there is a function to log server activities like who accessed what file at what time and
all errors that occurred while communicating with clients or internal server error. The other functions are to
perform the main socket activities createSocket() --> bindSocket() --> socketListening() --> acceptSocket( ) -->
[ Recieve() --> Send() --> Close() ]. After creating a passive socket then it will be made to stay in accepting
mode using an infinite loop. To support multiple users it forks the main process and handles client's request in
the child process and after handling the request it closes the connection and the process will be killed.

III. Server, CGI and User Agent Communication
The server uses a CGI program written in C to handle incoming HTTP requests interactively. After the
server creates a new process to handle a request it accepts the request and then invokes the CGI program and
passes the request to the CGI. The CGI program then extracts the HTTP request method (GET or POST),
requested file and Protocol which is either HTTP/1.0 or HTTP/1.1. Requests coming from the user agent/client
will send the request in a format of localhost:8000/cgi-bin/file.extension for example to access the index.htm file
user should type as follows http://localhost:8000/cgi-bin/index.htm and then the server will pass the incoming
request to the CGI.
The CGI program will first perform validation of the request i.e. weather it is sent with the right format
written like the above example all file requests should have a format of /cgi-bin/filename.extension and then it
will extract the required information according to the method weather it is GET or POST and if the request
doesn't have the right format an “Error 400 Bad Request” Error will be send. After validating the request the CGI
program will check if the requested mime type is supported or not by searching the extension in the mime.types
file found in the config-files folder. If the mime type is not found it will send bad request but if the mime type is
found and not implemented on the server it will send not supported on the server message to the user/client.
HTTP GET; after confirming weather requested mime is supported or not the next step will be to
respond according to the incoming request method for example if it is a GET request CGI will search for the file
and if it is found it will pass an HTTP header as specified in the RFC/1945 and the requested file.
HTTP POST; There is a sample post.htm file which is used as a sample to show POST method
implementation. So if the request is post it will extract the values from the key1=value1&key2=value2 message
format and perform some operation on the values and post it back to the user using the browser. This is mainly
done to show interactive dynamic operations of the CGI program at the back end.

IV. Usage
Extract the webserver folder in wherever suitable place on the local drive. The folder comes with an executable file already in it so a user can run it fromterminal by changing the directory to the cgi-bin location of the server and run “./webServer”. In case if the user
wants to recompile the executables it has to be done from the src folder of the webserver and run make clean
command from terminal to remove previous executables and run make to create a new executable and this will
create a new executable file in the cgi-bin folder.
The default port address where the server runs on is 8000 on the localhost so a user can send request
with the format localhost:8000/cgi-bin/filename.extension from a browser and then if the file is available it will
be displayed. Localhost:8000/cgi-bin/index.htm could be a good place to start navigating through all the web
pages available in the root directory of the server.
To check the “POST” functionality please open the post.htm page and fill in the form with the right
format like single name in the name field and a number in the salary field and click submit this will post the
values and the CGI will extract the values from the message and display processed values back to the user.

V. Testing
The bullet points below explain how to test the server program for an RFC adherence and server functionality.
•
Server Side Errors: all 5XX error messages for example “500 Internal Server Error” can be checked
by first running the webserver and while the server is running do a make clean from terminal and this
will remove the cgi executable then when the server is requested for a file it will try to do exec on the cgi
and this will cause exec to fail and this will cause a “500 Internal Server Error” error message.
•
Client Side Errors: all 4XX error messages can be checked by sending a request with a wrong format
for example “localhost:8000/cgi-bin/” missing file or “localhost:/cgi-bin/index.” missing extension
would cause “Error 400 Bad Request” message to be sent to user. And localhost:8000/cgi-
bin/wrongfile.html for example will cause a “Error 404 Not Found” message to be sent.
•
HTTP GET: to check if the GET request method is working properly open the “localhost:8000/cgi-
bin/index.htm” file and this allows to navigate through the available web pages.
•
HTTP POST: to check if the “POST” method is working open the post.htm file and then give it a name
and some number in the salary text field then click submit.
•
Graphics: to check graphics navigate to the webroot folder of the server and get a name for any .jpg
or .gif file from the images or webroot folder and open that from the browser.
•
Logging: all the server activities will be recorded either on the accesslogs.log or errorlogs.log file and
this can be checked by navigating to the log folder and open the log files.
