/** 
 * @file server.c
 * @brief TCP server programming fun project!
 * 
 * @author Austin James
 * @date Created: 11/4/24
 * @date Last Modified: 3/10/25
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

    pid_t chatRooms[16];
    int chatRoomsSockets[16];
    for (int i = 0; i < 16; i ++) chatRoomsSockets[i] = -1;
    fd_set serverSelectfd;
    int maxFD;

    while (1) { // !! BIG TODO: Change this to a select so we can listen for the rooms closing!
        resetFD_SETParentSide(chatRoomsSockets, serverSocket, &serverSelectfd);
        maxFD = calculateMaxfdParentSide(chatRoomsSockets, serverSocket);
        Select(maxFD, &serverSelectfd, NULL, NULL, NULL);

        if (FD_ISSET(serverSocket, &serverSelectfd)) {
            struct ClientData client;
            client.clientSocket  = Accept(serverSocket, (struct sockaddr*) &address, &addrlen);
            fprintf(stdout, "\n\n----------------------------------\nClient Found!\n");
            Read(client.clientSocket, buffer, 1024 - 1);
            __uint128_t package;
            memcpy(&package, buffer, sizeof(package));
            fprintf(stdout, "unpacking user data\n");
            unpackage(&package, client.username, client.userPassword, &client.chatRoom);

            if (strcmp(password, client.userPassword) != 0) {
                fprintf(stdout, "User gave wrong password\n");
                continue;
                // TODO probably should tell the user this
            }
            fprintf(stdout, "Correct Password from client!\n");
            
            // sends the client to the room by either creating a new room or joining that room
            if (chatRooms[client.chatRoom] == 0) chatRoomsSockets[client.chatRoom] = createRoom(chatRooms, &client);
            else if (chatRooms[client.chatRoom] != 0) joinRoom(chatRoomsSockets[client.chatRoom], &client);
        }

        // looks for rooms that are closing
        for (int i = 0; i < 16; i ++) {
            if (chatRoomsSockets[i] == -1) continue;
            if (FD_ISSET(chatRoomsSockets[i], &serverSelectfd)) {
                char buffer[16];
                Read(chatRoomsSockets[i], buffer, sizeof(buffer));
                // fprintf(stdout, "DEBUG: %s\n", buffer);

                if (strcmp("$exit", buffer) == 0) {
                    Close(chatRoomsSockets[i]);
                    chatRoomsSockets[i] = -1;
                    chatRooms[i] = 0;
                    fprintf(stdout, "chat room %d closed\n", i);
                }
            }
        }
        
        
        
    }
    // closing the connected socket
    Close(clientSocket);
    // closing the listening socket
    Close(serverSocket);
    return 0;
}