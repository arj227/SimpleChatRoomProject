#include "serverRoomFuncs.h"
#include "syscalls.h"

#include <netinet/in.h>   // Needed for sockaddr_in structure
#include <stdio.h>         // Needed for printf, fprintf, perror
#include <stdlib.h>        // Needed for exit, EXIT_FAILURE
#include <string.h>        // Needed for strcmp, strlen, strcpy
#include <sys/socket.h>    // Needed for socket, bind, listen, setsockopt
#include <unistd.h>        // Needed for close, if used to close sockets
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>


/**
 * @brief activates a new room from the forked server process
 * 
 * @details takes the forked process and sticks it in this function
 *          looping -- right now it just sends out a hello to the 
 *          connected server but it will be the function that runs
 *          the chat room
 * 
 * @todo this is in an incomplete function
 * 
 * @param firstClient int socket for the client that asked to open the room
 * @param chatRoomNumber int what room number this is, used mostly for terminal prints
 */
void activeChatRoom(struct ClientData *firstClient, int chatRoomNumber, int socketWithParent) {
    struct ClientData clients[10];
    // STANDS FOR CurrentNumberClients
    int CNC = 1;
    for (int i = 0; i < 10; i++) {
        clients[i].username[0] = '\0';
        clients[i].userPassword[0] = '\0';
        clients[i].chatRoom = 0;
    }
    clients[0] = *firstClient;

    //SET UP STRUCTURE FOR SELECT()
    fd_set selectClient;
    FD_ZERO(&selectClient);
    FD_SET(socketWithParent, &selectClient);
    FD_SET(clients[0].clientSocket, &selectClient);
    int maxfd = (socketWithParent > clients[0].clientSocket ? socketWithParent : clients[0].clientSocket) + 1;

    while(1) {
        /*int selectReturnVal = */Select(maxfd, &selectClient, NULL, NULL, NULL);

        //checks for new messages from parent server (new clients)
        if (FD_ISSET(socketWithParent, &selectClient)) {
            fprintf(stdout, "%d: recieved new user\n", chatRoomNumber);
            Read(socketWithParent, &clients[CNC], sizeof(struct ClientData));
            fprintf(stdout, "%d: new user: %s\n", clients[CNC].chatRoom, clients[CNC].username);
            CNC ++;
            fprintf(stdout, "----------------------------------\n\n");
        }

        //checks for messages from the clients
        for (int i = 0; i < CNC; i ++) {
            if (FD_ISSET(clients[i].clientSocket, &selectClient)) {
                fprintf(stdout, "%d: client (%s) has sent info\n", chatRoomNumber, clients[i].username);
                readFromClient(&clients[i]);
                // resetFD_SET(&CNC, clients, &selectClient, socketWithParent);
                // maxfd = calculateMaxfd(clients, socketWithParent, CNC);
            }
        }
        // sleep(1);
        resetFD_SET(&CNC, clients, &selectClient, socketWithParent);
        maxfd = calculateMaxfd(clients, socketWithParent, CNC) + 1;
    }
}

/**
 * @brief sends the new client to an open room
 * 
 * @param chatRoomSocket int the chatroom that is open
 * @param clientToSend int* the new client that wants to join
 */
int joinRoom(int chatRoomSocket, struct ClientData *client) {
    
    fprintf(stdout, "sending new client\n");
    Send(chatRoomSocket, client, sizeof(struct ClientData), 0);
    return 0;
}


/**
 * @brief adds new client to the fd_set structure for the select()
 */
void resetFD_SET(int *CNC, struct ClientData *clients, fd_set *selectClient, int socketWithParent) {
    FD_ZERO(selectClient);

    FD_SET(socketWithParent, selectClient);
    for (int i = 0; i < *CNC; i ++) {
        FD_SET(clients[i].clientSocket, selectClient);
    }

    
}

/**
 * @brief calculates the maxfd
 * 
 * @param clients struct ClientData
 * @param parentSocket int
 * @param CNC int number of connected clients (how big is the clients array)
 */
int calculateMaxfd(struct ClientData *clients, int parentSocket, int CNC) {
    int maxfd = parentSocket;
    for (int i = 0; i < CNC; i++) {
        if (clients[i].clientSocket > maxfd) {
            maxfd = clients[i].clientSocket;
        }
    }
    return maxfd;
}

void readFromClient(struct ClientData *currentClient) {
    char buffer[32];
    ssize_t n = Read(currentClient->clientSocket, buffer, sizeof(buffer) - 1);
    buffer[n] = '\0';
    fprintf(stdout, "from %s: %s\n", currentClient->username, buffer);
}