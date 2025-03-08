#ifndef SERVERFUNCS_H_
#define SERVERFUNCS_H_

#include <sys/socket.h>
#include <netinet/in.h>

void StringToLower(char *str);
void parseArgs(int argc, char const *argv[], char *hexArray);

void createSocket(int *serverSocket, int *socketOption, struct sockaddr_in *address);
void printLocalIP();

void unpackage(__uint128_t *package, char* username, char* password, uint8_t *chatRoom);

void activeChatRoom(int clientSocket, int chatRoomNumber);


#endif