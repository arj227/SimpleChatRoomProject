#ifndef SERVERFUNCS_H_
#define SERVERFUNCS_H_

#include <sys/socket.h>
#include <netinet/in.h>

struct ClientData {
    char username[32];
    char userPassword[32];
    u_int8_t chatRoom;
    int clientSocket;
};


void StringToLower(char *str);
void parseArgs(int argc, char const *argv[], char *hexArray);

void createSocket(int *serverSocket, int *socketOption, struct sockaddr_in *address);
void printLocalIP();

void unpackage(__uint128_t *package, char* username, char* password, uint8_t *chatRoom);


#endif