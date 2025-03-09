#ifndef SERVERROOMFUNCS_H_
#define SERVERROOMFUNCS_H_


void activeChatRoom(int firstClient, int chatRoomNumber, int socketWithParent);
int joinRoom(int chatRoomSocket, int clientToSend);


#endif