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
#include "serverRoomFuncs.h"
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

    pid_t chatRooms[255];

    while (1) {
        clientSocket  = Accept(serverSocket, (struct sockaddr*) &address, &addrlen);
        fprintf(stdout, "\n\n----------------------------------\nClient Found!\n");


        Read(clientSocket, buffer, 1024 - 1);
        __uint128_t package;
        memcpy(&package, buffer, sizeof(package));

        fprintf(stdout, "unpacking user data\n");
        char username[32];
        char userPassword[32];
        u_int8_t chatRoom = 0;
        unpackage(&package, username, userPassword, &chatRoom);

        if (strcmp(password, userPassword) != 0) {
            fprintf(stdout, "User gave wrong password\n");
            continue;
            // TODO probably should tell the user this
        }
        fprintf(stdout, "Correct Password from client!\n");

        if (chatRooms[chatRoom] == 0) {
            fprintf(stdout, "running fork!\n");
            chatRooms[chatRoom] = Fork();

            if (chatRooms[chatRoom] == 0) {
                fprintf(stdout, "activating new room: %d\n", chatRoom);
                fprintf(stdout, "----------------------------------\n\n");
                fflush(stdout);
                activeChatRoom(clientSocket, chatRoom);
                // !!TODO reap this child :)
            } else if (chatRooms[chatRoom] != 0) {
                fprintf(stdout, "this is the parent\n");
            } else {
                fprintf(stdout, "\n\n\nBig Fail\n\n\n");
            }
        } else if (chatRooms[chatRoom] != 0) {
            fprintf(stdout, "would run joinroom\n");
            // joinRoom();
        }
    }
    // closing the connected socket
    Close(clientSocket);
    // closing the listening socket
    Close(serverSocket);
    return 0;
}


/*



**** current thinking ****

going to take in clients if a chat room doesn't exist fork the server and create one
if it does connect that client to that room








*/