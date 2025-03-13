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

    pid_t chatRooms[255];
    int chatRoomsSockets[255];
    fd_set serverSelectfd;

    while (1) {
        struct ClientData client;

        // !! BIG TODO: Change this to a select so we can listen for the rooms closing!
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
        if (chatRooms[client.chatRoom] == 0) chatRoomsSockets[client.chatRoom] = createRoom(&chatRooms, &client);
        else if (chatRooms[client.chatRoom] != 0) joinRoom(chatRoomsSockets[client.chatRoom], &client);
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