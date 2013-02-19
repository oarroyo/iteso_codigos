/**
  @author Alvaro Parres
  @date Feb/2013

*/

#include "defines.h"

#ifndef SERVER_H
#define SERVER_H

int start_server(const ip_version_enum ip_version, const char iface[], const u_short port, const u_short maxClients);

#endif