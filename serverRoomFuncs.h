#ifndef SERVERROOMFUNCS_H_
#define SERVERROOMFUNCS_H_

#include "serverFuncs.h"

void activeChatRoom(int firstClient, int chatRoomNumber, int socketWithParent);
int joinRoom(int chatRoomSocket, struct ClientData *client);


#endif