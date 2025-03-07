/** 
 * @file server.c
 * @brief TCP server programming fun project!
 * 
 * @author Austin James
 * @date Created: 11/4/24
 * @date Last Modified: 11/5/24
 * 
 * @copyright
 * Copyright (c) 2024 Austin James
 * Licensed under the MIT License. See LICENSE file for details.
*/

#include "serverFuncs.h"
#include "syscalls.h"

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080

int main(int argc, char const* argv[])
{
    int serverSocket, clientSocket;
    struct sockaddr_in address;
    int socketOption = 1;
    socklen_t addrlen = sizeof(address);
    char buffer[1024] = { 0 };

    char password[32];

    parseArgs(argc, argv, password);

    createSocket(&serverSocket, &socketOption, (struct sockaddr_in*) &address);
    
    printLocalIP();

    clientSocket = Accept(serverSocket, (struct sockaddr*) &address, &addrlen);
    fprintf(stdout, "Client Accepted!");

    Read(clientSocket, buffer, 1024 - 1);
    printf("%s\n", buffer);

    // closing the connected socket
    Close(clientSocket);
    // closing the listening socket
    Close(serverSocket);
    return 0;
}
