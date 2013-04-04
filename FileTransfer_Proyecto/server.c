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

#include "netlib/tcp.h"
#include "utils/debug.h"

//Prototipos Locales
void start_protocol(const int clientSocket,const char baseDir[]);
void doGet(const int clientSocket, const char  baseDir[], const char *fileName);
int sendLine(const int clientSocket, const char* writeBuffer);

//Funciones
int start_server(const u_short port, const char baseDir[]) {

	int serverSocket;
	int clientSocket;
	
	char clientIP[18];
	u_int clientPort;
	
	int forkID;
	
	int localError;
	
	serverSocket = newTCPServerSocket4("0.0.0.0",port,5);
	
	if(serverSocket == -1) {
		return 1;
	}
	
	while(true) {
		bzero(clientIP,sizeof(clientIP));
		clientPort = 0;
		
		debug(1,"%s","Waiting for a Client...");
		clientSocket = waitConnection4(serverSocket,clientIP,&clientPort);
		if(clientSocket == -1) {
			debug(1,"%s","ERROR: Invalid Client Socket");
			continue;
		}
		debug(2,"Connected Client %s:%u",clientIP,clientPort);
	
		forkID = fork();
		if(forkID == 0) {
			//Hilo para para trabajo con el Cliente
			start_protocol(clientSocket,baseDir);
			closeTCPSocket(clientSocket);
			debug(2,"Close connection (%s:%u)",clientIP,clientPort);
			
		} else if(forkID > 0) {
			//Hilo para el trabajo del Servidor
			closeTCPSocket(clientSocket);
			
		} else {
			//Error en el fork.
			localError = errno;
			debug(0,"ERROR, Cant Fork for Client %s",strerror(localError));
			return 1;
		}
		
	}

	closeTCPSocket(serverSocket);	
	return 0;
	
}

void start_protocol(const int clientSocket,const char baseDir[]) {

	char readBuffer[1024];
	int readBytes;
	char *cmdString;
	char *ptr;
	char *token;

	while(true) {
		cmdString = (char *) calloc(255,1);
		while((readBytes = read(clientSocket,readBuffer,1))>0) {
			cmdString = (char *) realloc(cmdString,strlen(cmdString)+readBytes+1);
			strncat(cmdString,readBuffer,readBytes);
			ptr = cmdString+(strlen(cmdString)-4);
			if(strcmp(ptr,"\r\n\r\n")==0) {
				*ptr = '\0';
				break;
			}
		}

		debug(4,"<- %s",cmdString);

		token = strtok(cmdString," \t\b");
		if(strcmp(token,"GET")==0) {
			token = strtok(NULL," \t\b");
			if(token == NULL) {
				sendLine(clientSocket, "ERROR ( Missing FileName )\r\n\r\n");
			} else {
				doGet(clientSocket,baseDir, token);
			}
		} else if(strcmp(token,"LIST")==0) {
			//DoList();
		} else if(strcmp(token,"QUIT")==0) {
					break;
		} else {
			sendLine(clientSocket, "UNKOWN METHOD\r\n\r\n");
		}

		if(cmdString != NULL) free(cmdString);
	}

}


void doGet(const int clientSocket, const char  baseDir[], const char *fileName) {
	
	char *writeBuffer = (char *) malloc(256);
	u_int writeBytes = 0;
	char *readBuffer;
	u_int readBytes = 0;
	char *fullPath;
	int fd;
	int localError;
	struct stat fs;
	
	fullPath = (char *) malloc(strlen(baseDir)+strlen(fileName)+1);
	strcpy(fullPath,baseDir);
	strcat(fullPath,fileName);
	
	fd = open(fullPath,O_RDONLY);
	free(fullPath);
	
	if(fd == -1) {
		//Error al abrir el archivo solicitado.
		localError = errno;
		debug(1,"Can't open Requested File (%s)",strerror(localError));
		sprintf(writeBuffer,"NOT_FOUND %s\r\n\r\n",fileName);
		sendLine(clientSocket, writeBuffer);
		free(writeBuffer);
		return;
	} 

	sendLine(clientSocket, "OK\r\n");
	fstat(fd, &fs);
	sprintf(writeBuffer,"Size: %u\r\n",(u_int)fs.st_size);
	sendLine(clientSocket,writeBuffer);
	sendLine(clientSocket,"\r\n");	
	
	readBuffer = (char *) malloc(102400);	
	while((readBytes = read(fd,readBuffer,102400))>0) {
		writeBytes = 0;
		while(writeBytes < readBytes) {
			writeBytes += write(clientSocket,readBuffer+writeBytes,readBytes-writeBytes);
		}
	}
	
	free(readBuffer);
	free(writeBuffer);
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



