#ifndef SERVERFUNCS_H_
#define SERVERFUNC_H_

void createSocket(int *serverSocket, int *socketOption, struct sockaddr_in *address);
void StringToLower(char *str);


#endif