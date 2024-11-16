#ifndef SERVERFUNCS_H_
#define SERVERFUNC_H_

#include <sys/socket.h>
#include <netinet/in.h>

void StringToLower(char *str);
void parseArgs(int argc, char const *argv[], char *hexArray);

void createSocket(int *serverSocket, int *socketOption, struct sockaddr_in *address);
void printLocalIP();


#endif