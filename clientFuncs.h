#ifndef CLIENTFUNCS_H
#define CLIENTFUNCS_H

#include <sys/socket.h>
#include <netinet/in.h> 

typedef struct {
    char username[8];
    int chatRoom;
    int socket;
} client;

void parseArgs(int argc, char const *argv[], char *ipArray);
int userLogIn(char* username, char* password, uint8_t *chatRoom);
void assemblyPackage(__uint128_t *package, char* username, char* password, uint8_t *chatRoom);

int connectServer(int port, struct sockaddr_in *serverAddress, char *ipAdress);

int readFromServer(int socket);
void sendToServer(int socket);
void userCommand(char *buffer, int socket);

#endif