#include "syscalls.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h> 



/**
 * @brief creates a socket and connects to a server
 * 
 * @param port string with port number
 * @param serverAddress sockaddr_in struct
 * 
 * @note this function has the IP address hard coded in
*/
int connectServer(int port, struct sockaddr_in *serverAddress) {
    int portINT = port;

    serverAddress->sin_family = AF_INET;
    serverAddress->sin_port = htons(portINT);
    serverAddress->sin_addr.s_addr = inet_addr("128.180.120.87");

    int clientSocket;
    clientSocket = Socket(AF_INET, SOCK_STREAM, 0);

    Connect(clientSocket, (struct sockaddr*) serverAddress, sizeof(struct sockaddr_in));

    fprintf(stdout, "Connected to Server!");

    return clientSocket;
}