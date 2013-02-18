/**
  @file server.c

  @author Alvaro Parres
  @date Feb/2013

*/

#include <sys/types.h>
#include <strings.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "netlib/tcp.h"
#include "utils/debug.h"
#include "defaults.h"

int start_server(const char iface[], const u_short port, const u_short maxClients, const char *filename) {

	int serverSocket;
	int clientSocket;
	
	char clientIP[18];
	u_int clientPort;
	
	int file;
	
	int localerror;
	
	int readBytes = 0;
	int totalReadBytes = 0;	
	int totalWriteBytes = 0;
	int writeBytes = 0;
	char *readBuffer = NULL;
	
	serverSocket = newTCPServerSocket4(iface,port,maxClients);
	
	if(serverSocket == -1) {
		return false;
	}
	
	if((file = open(filename,O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) == -1) {
		localerror = errno;
		fprintf(stderr,"Can't open file for write (%s)\n",strerror(localerror));
		return false;
	}
	
	bzero(clientIP,sizeof(clientIP));
	clientPort = 0;
		
	debug(5,"%s","Waiting for a Client...");
	clientSocket = waitConnection4(serverSocket,clientIP,&clientPort);
	debug(4,"Connected Client %s:%u",clientIP,clientPort);

	readBuffer = (char *) calloc(1,BUFFERSIZE);
	
	while((readBytes = read(clientSocket,readBuffer,BUFFERSIZE)) > 0) {
		debug(5,"\tSe leyeron de %s:%u %u bytes",clientIP,clientPort,readBytes);
		totalWriteBytes = 0;
		while(totalWriteBytes < readBytes) {
			writeBytes = write(file,readBuffer+totalWriteBytes,readBytes-totalWriteBytes);
			totalWriteBytes += writeBytes;
		}
		totalReadBytes += readBytes;
	}	

	debug(3,"\t Se leyeron un total de %i de bytes\n",totalReadBytes);
	
	free(readBuffer);	
	close(file);
	closeTCPSocket(clientSocket);
	debug(4,"Close connection (%s:%u)",clientIP,clientPort);

	closeTCPSocket(serverSocket);	
	return true;
	
}

