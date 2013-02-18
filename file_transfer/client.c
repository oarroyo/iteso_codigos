/**
  @file client.c

  @author Alvaro Parres
  @date Feb/2013

*/

#include <sys/types.h>
#include <stdio.h>
#include <sys/time.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <string.h>

#include "client.h"
#include "netlib/tcp.h"
#include "utils/debug.h"
#include "defaults.h"

u_long currentTimeMillis() ;

int start_client(const char *ip_dst, const u_int port, const char *filename) {

	int clientSocket;
	u_long start, end;
	int file;
	char *buffer;
	
	int localerror;
	
	int readBytes,totalReadBytes;
	int writeBytes,totalWriteBytes;
	
	if((file = open(filename,O_RDONLY)) == -1) {
		localerror = errno;
		fprintf(stderr,"Can't open filename (%s)",strerror(localerror));
		return false;
	}	
	
	clientSocket = newTCPClientSocket4(ip_dst,port);
	
	buffer = (char *) calloc(1,BUFFERSIZE);
	start = currentTimeMillis();
	while((readBytes = read(file,buffer,BUFFERSIZE))>0) {
		debug(5,"\tSe leyeron de archivo %u bytes",readBytes);		
		totalWriteBytes = 0;
		while(totalWriteBytes < readBytes) {
			writeBytes = write(clientSocket,buffer+totalWriteBytes,readBytes-totalWriteBytes);
			debug(5,"\tSe escribieron a %s:%u %u bytes",ip_dst,port,writeBytes);
			totalWriteBytes += writeBytes;
		}
		end = currentTimeMillis();

		printf("\rTransfiriendo a %s:%u\t%lu\tKBps",ip_dst,port,(readBytes/(end-start)*1000/1024));
		totalReadBytes += readBytes;
		start = currentTimeMillis();
	}
	
	close(file);
	free(buffer);
	closeTCPSocket(clientSocket);
	
	return true;
}

u_long currentTimeMillis() {
        u_long t;
        struct timeval tv;
        gettimeofday(&tv, 0);
        t = (tv.tv_sec*1000)+tv.tv_usec;
        return t;
}