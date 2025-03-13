#ifndef SERVERROOMFUNCS_H_
#define SERVERROOMFUNCS_H_

#include "serverFuncs.h"

typedef struct MessagePacket {
    uint16_t messageLength;
    char message[64];
} MessagePacket;

void activeChatRoom(struct ClientData *client, int chatRoomNumber, int socketWithParent);
int joinRoom(int chatRoomSocket, struct ClientData *client);

void resetFD_SET(int CNC, struct ClientData *clients, fd_set *selectClient, int socketWithParent);
int calculateMaxfd(struct ClientData *clients, int parentSocket, int CNC);
void readFromClient(struct ClientData *currentClient, int whatClient, int *CNC, char *message, int socketWithParent);
void sendToClients(struct ClientData *clients, int whatClient, int CNC, char *message);

int send_fd(int unix_socket, int fd_to_send);
int recv_fd(int unix_socket);

#endif