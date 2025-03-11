#ifndef SERVERROOMFUNCS_H_
#define SERVERROOMFUNCS_H_

#include "serverFuncs.h"

void activeChatRoom(struct ClientData *client, int chatRoomNumber, int socketWithParent);
int joinRoom(int chatRoomSocket, struct ClientData *client);

void resetFD_SET(int CNC, struct ClientData *clients, fd_set *selectClient, int socketWithParent);
int calculateMaxfd(struct ClientData *clients, int parentSocket, int CNC);
void readFromClient(struct ClientData *currentClient, int whatClient, int *CNC);

#endif