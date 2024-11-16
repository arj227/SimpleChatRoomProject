#ifndef SERVERFUNCS_H_
#define SERVERFUNC_H_

#include <sys/socket.h>
#include <netinet/in.h> 

void createSocket(int *serverSocket, int *socketOption, struct sockaddr_in *address);
void StringToLower(char *str);

void parseArgs(int argc, char const *argv[], char *hexArray);
void printLocalIP();


#endif