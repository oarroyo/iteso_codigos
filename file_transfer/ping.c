/**
  @author Alvaro Parres
  @date Feb/2013

*/

#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/uio.h>
#include <stdio.h>
#include <stdarg.h>

#include "ping.h"
#include "utils/debug.h"

int PingListen(int clientSocket, const char *clientIP, const u_int clientPort) {
	char *readData=NULL;
	char *buffer=NULL;
	
	u_int readbytes=0;
	u_int totalbytes=0;
	
	int localerror;
	
	u_int bytesSent = 0;
	u_int totalBytesSent = 0;
		
	buffer = (char *) calloc(1024,sizeof(char));
	if(buffer == NULL) {
		localerror = errno;
		fprintf(stderr,"Can't allocate Memory for Buffer (%s)",strerror(localerror));
		return -1;
	}

	while((readbytes=read(clientSocket,buffer,1024))>0) {
		debug(5,"\tSe leyeron de %s:%u %u bytes",clientIP,clientPort,readbytes);
		readData = (char *) realloc(readData,totalbytes+readbytes+1);
		memcpy(readData+totalbytes,buffer,readbytes);
		totalbytes += readbytes;
		if(readData[totalbytes-1]=='\0') { break; }
	}
	
	debug(3,"Se leyeron de %s:%u un total de %u bytes",clientIP,clientPort,totalbytes);

    while(totalBytesSent < totalbytes) {
		bytesSent = write(clientSocket,readData+totalBytesSent,totalbytes-totalBytesSent);
		debug(3,"Se envian a %s:%u  %u de %u bytes",clientIP,clientPort,bytesSent,totalbytes);
		totalBytesSent += bytesSent;
	}
	
	free(readData);
	free(buffer);
	return readbytes;
}

int pingSend(int clientSocket, u_int msg_size) {
	
	char *bufferSend;
	char *bufferRecv;
	
	u_int bytesSent = 0;
	u_int totalBytes = 0;
	u_int i;
	u_int realsize = msg_size*1024;
	
	u_int readBytes = 0;
	u_int totalBytesRecv = 0;
	
	u_short ret;
	
	bufferSend = (char *) calloc(realsize+1,sizeof(char));
	for(i=0; i<(realsize-1); i++) {
		bufferSend[i] = ((i%26)+65);
	}
	
	while(totalBytes < strlen(bufferSend)) {
		bytesSent = write(clientSocket,bufferSend+totalBytes,strlen(bufferSend)-totalBytes+1);
		debug(5,"Se lenviaron %u de %u bytes",bytesSent,realsize);
		totalBytes += bytesSent;
	}	
	
	
	debug(3,"Se enviaron un total de %u bytes",totalBytes);

	bufferRecv = (char *) calloc(realsize+1,sizeof(char));
	while((readBytes=read(clientSocket,bufferRecv+totalBytesRecv,realsize))>0) {
		debug(5,"\tSe leyeron de %u de %u bytes",readBytes,realsize);
		totalBytesRecv += readBytes;
		if(bufferRecv[totalBytesRecv-1]=='\0') { break; }
	}
	
	debug(3,"Se leyeron un total de %u bytes",totalBytesRecv);
	
	if(memcmp(bufferSend,bufferRecv,realsize)==0) {
		ret = true;
		debug(3,"%s","Ping Correcto");
	} else {
		ret = false;
		debug(3,"%s","Ping Incorrecto");		
	}
	
	free(bufferSend);
	free(bufferRecv);

	return ret;
}
