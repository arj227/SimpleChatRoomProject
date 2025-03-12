#ifndef CLIENTFUNCS_H
#define CLIENTFUNCS_H

#include <sys/socket.h>
#include <netinet/in.h> 

typedef struct {
    char username[8];
    int chatRoom;
    int socket;
} client;

typedef struct MessagePacket {
    uint16_t messageLength;
    char message[64];
} MessagePacket;


void parseArgs(int argc, char const *argv[], char *ipArray);
int userLogIn(char* username, char* password, uint8_t *chatRoom);
void assemblyPackage(__uint128_t *package, char* username, char* password, uint8_t *chatRoom);

int connectServer(int port, struct sockaddr_in *serverAddress, char *ipAdress);

int readFromServer(int socket);
void sendToServer(int socket);
void userCommand(char *buffer);

#endif