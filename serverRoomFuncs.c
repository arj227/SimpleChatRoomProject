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
        resetFD_SET(CNC, clients, &selectClient, socketWithParent);
        maxfd = calculateMaxfd(clients, socketWithParent, CNC) + 1;
        /*int selectReturnVal = */Select(maxfd, &selectClient, NULL, NULL, NULL);

        //checks for new messages from parent server (new clients)
        if (FD_ISSET(socketWithParent, &selectClient)) {
            fprintf(stdout, "%d: recieved new user\n", chatRoomNumber);
            Read(socketWithParent, &clients[CNC], sizeof(struct ClientData));


            fprintf(stdout, "%d: firstClient: %s -- %s -- %d", clients[CNC].chatRoom, clients[CNC].username, 
                clients[CNC].userPassword, clients[CNC].clientSocket);
            fprintf(stdout, "%d: firstClient: %s -- %s -- %d\n", firstClient->chatRoom, firstClient->username, 
                firstClient->userPassword, firstClient->clientSocket);

            CNC ++;
            fprintf(stdout, "----------------------------------\n\n");
            continue;
        }
        //checks for messages from the clients
        for (int i = 0; i < CNC; i ++) {
            if (FD_ISSET(clients[i].clientSocket, &selectClient)) {
                fprintf(stdout, "%d: client (%s) has sent info\n", chatRoomNumber, clients[i].username);
                readFromClient(clients, i, &CNC);
                continue;
            }
        }
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
void resetFD_SET(int CNC, struct ClientData *clients, fd_set *selectClient, int socketWithParent) {
    FD_ZERO(selectClient);

    fprintf(stdout, "%d: adding parent to FD_SET\n", clients[0].chatRoom);
    FD_SET(socketWithParent, selectClient);
    for (int i = 0; i < CNC; i ++) {
        fprintf(stdout, "%d: adding client: %s to FD_SET with socklet: %d\n", 
            clients[i].chatRoom, clients[i].username, clients[i].clientSocket);
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


/**
 * @brief Reads data from a client socket and handles client disconnection.
 *
 * @details
 * This function attempts to read data from the socket associated with the client at index
 * whatClient within the clients array. The read operation is performed using the custom Read()
 * function and assumes the data is null-terminated. After reading, it prints the message along
 * with the client’s chat room number and username.
 *
 * If no data is read (n <= 0) or if the client sends the special exit command "$exit", the function
 * handles the disconnection by:
 *  - Optionally echoing back the "$exit" command if it was received.
 *  - Closing the client’s socket.
 *  - Removing the client from the clients array by shifting subsequent client entries to fill
 *    the gap.
 *  - Decrementing the client count (*CNC).
 *  - If no clients remain (i.e. *CNC becomes 0), it closes the chat room by calling exit(0).
 *
 * @param clients An array of ClientData structures representing the connected clients.
 * @param whatClient The index of the client in the clients array from which data is to be read.
 * @param CNC Pointer to an integer representing the current number of clients.
 *
 * @note The function expects the client socket in clients[whatClient] to be valid and that the
 *       Read() and Send() functions work similarly to the standard read() and send() functions.
 * @note This function modifies the clients array and the CNC variable if a client disconnects.
 * @note The function exits the process if there are no more clients in the chat room.
 */
void readFromClient(struct ClientData *clients, int whatClient, int *CNC) {
    int chatRoom = clients[0].chatRoom;
    char buffer[32];
    ssize_t n = Read(clients[whatClient].clientSocket, buffer, sizeof(buffer) - 1);
    

    if (n <= 0 || strcmp(buffer, "$exit") == 0) {
        fprintf(stdout, "%d: user (%s) is exiting\n", clients[whatClient].chatRoom, clients[whatClient].username);
        if (strcmp(buffer, "$exit") == 0) {
            Send(clients[whatClient].clientSocket, buffer, sizeof(buffer), 0);
        }
        close(clients[whatClient].clientSocket);

        if (whatClient == (*CNC - 1)) {
            memset(&clients[whatClient], 0, sizeof(struct ClientData));
            (*CNC) --;
        } else {
            // Shift all clients to the left, overwriting the closed client slot
            for (int i = whatClient; i < *CNC - 1; i++) {
                clients[i] = clients[i + 1];
            }
            // Clear the now-extra slot at the end
            memset(&clients[*CNC - 1], 0, sizeof(struct ClientData));
            // Decrement the number of connected clients
            (*CNC) --;
        }

        // fprintf(stdout, "DEBUG: CNC = %d\n", *CNC);
        if (*CNC == 0) {
            fprintf(stdout, "%d: closing room, no more users\n", chatRoom);
            exit(0);
        }
    }
    buffer[n] = '\0';
    fprintf(stdout, "%d: from %s: %s\n",clients[whatClient].chatRoom, clients[whatClient].username, buffer);
}