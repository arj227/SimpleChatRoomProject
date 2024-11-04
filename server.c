/** 
 * @file server.c
 * @brief TCP server programing fun project!
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

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080

int main(int argc, char const* argv[])
{
    int serverSocket, new_socket;
    ssize_t valread;
    struct sockaddr_in address;
    int socketOption = 1;
    socklen_t addrlen = sizeof(address);
    char buffer[1024] = { 0 };
    char* hello = "Hello from server";

    creat

    if ((new_socket = accept(serverSocket, (struct sockaddr*) &address, &addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    valread = read(new_socket, buffer, 1024 - 1); // subtract 1 for the null terminator at the end
    printf("%s\n", buffer);
    send(new_socket, hello, strlen(hello), 0);
    printf("Hello message sent\n");

    // closing the connected socket
    close(new_socket);
    // closing the listening socket
    close(serverSocket);
    return 0;
}
