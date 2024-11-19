#ifndef CLIENTFUNCS_H
#define CLIENTFUNCS_H

#include <sys/socket.h>
#include <netinet/in.h> 

void parseArgs(int argc, char const *argv[], char *ipArray);
int userLogIn(char* username, char* password);

int connectServer(int port, struct sockaddr_in *serverAddress, char *ipAdress);

#endif