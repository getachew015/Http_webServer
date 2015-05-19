all: 	webServer cgi move
	
move: 
	mv webServer ../cgi-bin/
	mv cgi ../cgi-bin/

cgi: 	
	gcc -std=gnu99 cgi.c -o cgi 

webServer: webServer.o serverhelper.o
	gcc -o webServer webServer.o serverhelper.o

serverhelper.o: serverhelper.c webServer.h
	gcc -c serverhelper.c

webServer.o: webServer.c webServer.h
	gcc -c webServer.c
 
clean:
	rm ../cgi-bin/webServer webServer.o serverhelper.o ../cgi-bin/cgi
