/**
  @author Alvaro Parres
  @date Feb/2013

*/

#ifndef PING_H
#define PING_H 

#include <sys/types.h>
int PingListen(int clientSocket, const char *clientIP, const u_int clientPort);
int pingSend(int clientSocket, u_int msg_size);

#endif