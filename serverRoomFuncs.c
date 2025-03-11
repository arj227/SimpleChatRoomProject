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

        int selectReturnVal = select(maxfd, &selectClient, NULL, NULL, NULL);

        if (selectReturnVal == -1) {
            fprintf(stderr, "select Failure: %d\n", EXIT_FAILURE);
            exit(EXIT_FAILURE);
        } else if (selectReturnVal == 0) {
            // shouldn't happen
            fprintf(stderr, "timeout\b");
            continue;
        }

        if (FD_ISSET(socketWithParent, &selectClient)) {
            fprintf(stdout, "%d: parent has sent info\n", chatRoomNumber);
            addNewClient(CNC, &clients, &selectClient);
        }

        for (int i = 0; i < CNC; i ++) {
            if (FD_ISSET(clients[i].clientSocket, &selectClient)) {
                fprintf(stdout, "%d: client (%s) has sent info\n", chatRoomNumber, clients[i].username);
            }
        }


        sleep(1);
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



void addNewClient(int CNC, struct ClientData *clients, fd_set *selectClient) {

}