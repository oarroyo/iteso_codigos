/**
  @file server.c

  @author Alvaro Parres
  @date Feb/2013

*/

#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>


#include "netlib/tcp.h"
#include "utils/debug.h"

//Prototipos Locales
void start_protocol(const int clientSocket,const char *remoteFilename,const char *localFilename);
int sendLine(const int clientSocket, const char* writeBuffer);
void doGet(int socket, const char *localFilename, unsigned long size);
u_long currentTimeMillis();
 
//Funciones
int start_client(const u_short port, const char ip[], const char remoteFilename[], const char localFilename[]) {

	int socket;
	
	socket = newTCPClientSocket4(ip, port);
	
	if(socket == -1) {
		return 1;
	}
	
	start_protocol(socket,remoteFilename,localFilename);
	closeTCPSocket(socket);
	debug(2,"Close connection (%s:%u)",ip,port);
	return 0;
			
}

void start_protocol(const int socket,const char *remoteFilename,const char *localFilename) {

	char writeBuffer[1024];
	char readBuffer[10240];
	int readBytes;
	char *cmdString;
	char *freeCmdString;
	char *ptr;
	char *token;

	sprintf(writeBuffer,"GET %s\r\n\r\n",remoteFilename);
	sendLine(socket,writeBuffer);
	
	cmdString = (char *) malloc(254);
	while((readBytes = read(socket,readBuffer,10240))>0) {
		cmdString = (char *) realloc(cmdString,strlen(cmdString)+readBytes+1);
		strncat(cmdString,readBuffer,readBytes);
		ptr = cmdString+(strlen(cmdString)-4);
		if(strcmp(ptr,"\r\n\r\n")==0) {
			*ptr = '\0';
			break;
		}
	}
	
	freeCmdString = cmdString;
	
	debug(4,"Recibimos: %s\n",cmdString);
	token = strsep(&cmdString,"\r");
	cmdString++; // Eliminamos el \n de igual manera.
	
	if(strcmp(token,"NOT_FOUND")==0) {
		debug(1,"ERROR: Archivo no encontrado(%s)",token);
		return;
	} else if(strcmp(token,"OK")==0) {
		token = strstr(cmdString,":");
		doGet(socket,localFilename,strtol(token+2,NULL,10));
	} else {
		debug(1,"ERROR UNKOWN ANSWER (%s)",token);
	}
	
	sendLine(socket,"QUIT\r\n\r\n");
	free(freeCmdString);
}

void doGet(int socket, const char *localFilename, unsigned long size) {

	u_int writeBytes = 0;
	char *readBuffer;
	u_int readBytes = 0;
	u_long totalReadBytes = 0;
	int fd;
	int localError;
	u_long start;
	u_long end;
		
	debug(4,"Doing Get for: %s (%li)",localFilename,size);
	fd = open(localFilename,O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
	if(fd == -1) {
		//Error al crear el archivo local.
		localError = errno;
		debug(1,"Can't create Local File (%s)",strerror(localError));
		return;
	}
	
	readBuffer = (char *) malloc(102400);
	start = currentTimeMillis();	
	while(totalReadBytes < size) {
		readBytes = read(socket,readBuffer,102400);
		if(readBytes > 0) {
			writeBytes = 0;
			while(writeBytes < readBytes) {
				writeBytes += write(fd,readBuffer+writeBytes,readBytes-writeBytes);
			}
		}
		totalReadBytes += readBytes;
		end = currentTimeMillis();
		if(end>start)
			debug(1,"Get %lu/%lu ( %0.0f%% ) speed %lukbps",totalReadBytes,size,(((float)totalReadBytes/(float)size)*100),(totalReadBytes/(end-start)*1000/1024));		
	}	
	
	free(readBuffer);
	close(fd);

}

int sendLine(const int clientSocket, const char* writeBuffer) {
	u_int writeBytes = 0;
	
	do { 
		writeBytes += write(clientSocket,writeBuffer+writeBytes,strlen(writeBuffer)-writeBytes);
	} while(writeBytes < strlen(writeBuffer));
	debug(4,"-> %s",writeBuffer);	

	return writeBytes;
}

u_long currentTimeMillis() {
        u_long t;
        struct timeval tv;
        gettimeofday(&tv, 0);
        t = (tv.tv_sec*1000)+tv.tv_usec;
        return t;
}

