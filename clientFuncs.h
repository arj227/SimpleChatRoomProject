#ifndef CLIENTFUNCS_H
#define CLIENTFUNCS_H

#include <sys/socket.h>
#include <netinet/in.h> 

int connectServer(int port, struct sockaddr_in *serverAddress);

#endif