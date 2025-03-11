#ifndef SERVERROOMFUNCS_H_
#define SERVERROOMFUNCS_H_

#include "serverFuncs.h"

void activeChatRoom(struct ClientData *client, int chatRoomNumber, int socketWithParent);
int joinRoom(int chatRoomSocket, struct ClientData *client);

void addNewClient(int CNC, struct ClientData *clients, fd_set *selectClient);

#endif