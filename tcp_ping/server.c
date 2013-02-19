/**
  @file server.c

  @author Alvaro Parres
  @date Feb/2013

*/

#include <sys/types.h>
#include <strings.h>
#include "netlib/tcp.h"
#include "utils/debug.h"
#include "ping.h"
#include "server.h"



int start_server(const ip_version_enum ip_version, const char iface[], const u_short port, const u_short maxClients) {

	int serverSocket;
	int clientSocket;
	
	char clientIP[18];
	u_int clientPort;
	
	serverSocket = newTCPServerSocket4(iface,port,maxClients);
	
	if(serverSocket == -1) {
		return 1;
	}
	
	while(true) {
		bzero(clientIP,sizeof(clientIP));
		clientPort = 0;
		
		debug(5,"%s","Waiting for a Client...");
		clientSocket = waitConnection4(serverSocket,clientIP,&clientPort);
		debug(4,"Connected Client %s:%u",clientIP,clientPort);
		
		//Start Server Protocol
		PingListen(clientSocket,clientIP,clientPort);
		
		closeTCPSocket(clientSocket);
		debug(4,"Close connection (%s:%u)",clientIP,clientPort);
	}

	closeTCPSocket(serverSocket);	
	return 0;
	
}