/** 
 * @file server.c
 * @brief TCP server programming fun project!
 * 
 * @author Austin James
 * @date Created: 11/4/24
 * @date Last Modified: 3/12/25
 * 
 * @copyright
 * Copyright (c) 2024 Austin James
 * Licensed under the MIT License. See LICENSE file for details.
*/

#include "clientFuncs.h"
#include "syscalls.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char const* argv[]) {
    char ipaddress[32];
    parseArgs(argc, argv, ipaddress);

    char username[32];
    char password[32];
    u_int8_t chatRoom = 0;
    int attempts = 0;
    while(userLogIn(username, password, &chatRoom) == -1 && attempts < 5) {
        printf("Invalid Input try again\n");
    } if (attempts == 5) {
        printf("To many failed attempts\n"); 
        return -1;
    }
    printf("credentials inputted!\n");

    __uint128_t package = 0;
    assemblyPackage(&package, username, password, &chatRoom);

    unsigned int port = 8080;
    struct sockaddr_in serverAddress;
    int socket = connectServer(port, &serverAddress, ipaddress);

    printf("Connected to the server...Sending package\n");
    Send(socket, &package, 128, 0);
    printf("Package Sent\n--> ");
    fflush(stdout);
    
    fd_set selectFD;
    int maxfd = 0;
    while (1) {
        resetFD_SET(socket, &selectFD);
        maxfd = socket + 1; // this is done like this since the other option is always 0
        Select(maxfd, &selectFD, NULL, NULL, NULL);

        if (FD_ISSET(socket, &selectFD)) {
            int fromServerReturn = readFromServer(socket);
            if (fromServerReturn == 1) break;
        } else if(FD_ISSET(0, &selectFD)) sendToServer(socket);
    }
    Close(socket);
    return 0;
}
