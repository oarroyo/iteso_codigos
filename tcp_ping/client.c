/**
  @file client.c

  @author Alvaro Parres
  @date Feb/2013

*/

#include <sys/types.h>
#include <stdio.h>
#include <sys/time.h>

#include "client.h"
#include "netlib/tcp.h"
#include "ping.h"

u_long currentTimeMillis() ;

int start_client(const char *ip_dst, const u_int port, const u_int msg_count, const u_int msg_size) {

	int clientSocket;
	u_int i=0;
	u_long start, end;
		
	for(i=0; i<msg_count; i++) {
		start = currentTimeMillis();
		clientSocket = newTCPClientSocket4(ip_dst,port);
		end = currentTimeMillis();
		if(pingSend(clientSocket,msg_size)) {
			printf("%i\tSent\t %i KB\t to %s on\t %lu ms\n",i,msg_size,ip_dst,(end-start));
		} else {
			printf("%i\tPing Error\n",i);
		}
		closeTCPSocket(clientSocket);
	}
	return true;
}

u_long currentTimeMillis() {
        u_long t;
        struct timeval tv;
        gettimeofday(&tv, 0);
        t = (tv.tv_sec*1000)+tv.tv_usec;
        return t;
}