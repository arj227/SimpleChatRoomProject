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
        fprintf(stdout, "\n");
        resetFD_SET(CNC, clients, &selectClient, socketWithParent);
        maxfd = calculateMaxfd(clients, socketWithParent, CNC);
        /*int selectReturnVal = */Select(maxfd, &selectClient, NULL, NULL, NULL);

        //checks for new messages from parent server (new clients)
        if (FD_ISSET(socketWithParent, &selectClient)) {
            fprintf(stdout, "%d: recieved new user\n", chatRoomNumber);
            Read(socketWithParent, &clients[CNC], sizeof(struct ClientData));
            clients[CNC].clientSocket = recv_fd(socketWithParent);


            // fprintf(stdout, "DEBUG: %d: newClient: %s -- %s -- %d\n", clients[CNC].chatRoom, clients[CNC].username, 
            //     clients[CNC].userPassword, clients[CNC].clientSocket);
            // fprintf(stdout, "DEBUG: %d: firstClient: %s -- %s -- %d\n", firstClient->chatRoom, firstClient->username, 
            //     firstClient->userPassword, firstClient->clientSocket);

            CNC ++;
            fprintf(stdout, "----------------------------------\n\n");
            continue;
        }
        //checks for messages from the clients
        for (int i = CNC - 1; i >= 0; i --) {
            // fprintf(stdout, "DEBUG:%d: checking for data from %s\n", clients[i].chatRoom, clients[i].username);
            if (FD_ISSET(clients[i].clientSocket, &selectClient)) {
                fprintf(stdout, "%d: client (%s) has sent info\n", chatRoomNumber, clients[i].username);

                char message[64];
                readFromClient(clients, i, &CNC, message, socketWithParent);
                sendToClients(clients, i, CNC, message);
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
    send_fd(chatRoomSocket, client->clientSocket);
    return 0;
}

/**
 * @brief adds new client to the fd_set structure for the select()
 */
void resetFD_SET(int CNC, struct ClientData *clients, fd_set *selectClient, int socketWithParent) {
    FD_ZERO(selectClient);

    // fprintf(stdout, "DEBUG:%d: adding parent to FD_SET\n", clients[0].chatRoom);
    FD_SET(socketWithParent, selectClient);
    for (int i = 0; i < CNC; i ++) {
        // fprintf(stdout, "DEBUG:%d: adding client: %s to FD_SET with socklet: %d\n", 
            // clients[i].chatRoom, clients[i].username, clients[i].clientSocket);
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
    return maxfd + 1;
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
void readFromClient(struct ClientData *clients, int whatClient, int *CNC, char *messageToReturn, int socketWithParent) {
    int chatRoom = clients[0].chatRoom;
    uint16_t messageLength;
    ssize_t n = Read(clients[whatClient].clientSocket, &messageLength, sizeof(messageLength));
    char *message = malloc(messageLength);
    if (message == NULL) {
        perror("malloc");
        exit(1);
    }
    Read(clients[whatClient].clientSocket, message, messageLength);


    if (n <= 0 || strcmp(message, "$exit") == 0) {
        fprintf(stdout, "%d: user (%s) is exiting\n", clients[whatClient].chatRoom, clients[whatClient].username);
        if (strcmp(message, "$exit") == 0) {
            Send(clients[whatClient].clientSocket, message, sizeof(message), 0);
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
            Send(socketWithParent, "$exit", sizeof("$exit"), 0);
            // fprintf(stdout, "DEBUG: %d: sent\n", chatRoom);
            exit(0);
        }
    }
    fprintf(stdout, "%d: from %s: %s\n",clients[whatClient].chatRoom, clients[whatClient].username, message);
    strcpy(messageToReturn, message);
}

void sendToClients(struct ClientData *clients, int whatClient, int CNC, char *message) {
    char finalString[96];
    snprintf(finalString, sizeof(finalString), "%s: %s", clients[whatClient].username, message);

    struct MessagePacket messagePacket;
    snprintf(messagePacket.message, sizeof(messagePacket.message), "%.63s", finalString);
    messagePacket.messageLength = strlen(messagePacket.message) + 1;

    for (int i = 0; i < CNC; i ++) {
        if (i == whatClient) continue;
        Send(clients[i].clientSocket, &messagePacket, sizeof(messagePacket.messageLength) + messagePacket.messageLength, 0);
    }
}

/**
 * @brief Sends a file descriptor over a Unix domain socket.
 *
 * This function uses the SCM_RIGHTS mechanism to send a file descriptor (fd_to_send)
 * over a Unix domain socket specified by unix_socket. It attaches the file descriptor
 * as ancillary data using sendmsg(). A small payload (e.g. "FD") is sent along with the
 * file descriptor, though the payload content is not critical.
 *
 * @param unix_socket The Unix domain socket file descriptor used for sending the FD.
 * @param fd_to_send The file descriptor to be sent.
 * @return 0 on success, or -1 if an error occurs (an error message is printed via perror).
 *
 * @note Ensure that unix_socket is a valid, connected Unix domain socket.
 * @note The file descriptor passed in (fd_to_send) will be duplicated by the OS when
 *       received on the other side.
 */
int send_fd(int unix_socket, int fd_to_send) {
    struct msghdr msg = {0};
    char buf[CMSG_SPACE(sizeof(fd_to_send))];
    memset(buf, '\0', sizeof(buf));

    // You can send any data with the FD; here we send a simple dummy payload.
    struct iovec io = { .iov_base = (void *)"FD", .iov_len = 2 };

    msg.msg_iov = &io;
    msg.msg_iovlen = 1;
    msg.msg_control = buf;
    msg.msg_controllen = sizeof(buf);

    struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type  = SCM_RIGHTS;
    cmsg->cmsg_len   = CMSG_LEN(sizeof(fd_to_send));

    memcpy(CMSG_DATA(cmsg), &fd_to_send, sizeof(fd_to_send));

    msg.msg_controllen = cmsg->cmsg_len;

    if (sendmsg(unix_socket, &msg, 0) < 0) {
        perror("sendmsg");
        return -1;
    }
    return 0;
}

/**
 * @brief Receives a file descriptor from a Unix domain socket.
 *
 * This function receives a file descriptor that was sent via SCM_RIGHTS over a Unix domain socket.
 * It uses recvmsg() to retrieve both the payload and the ancillary data, from which it extracts the
 * transferred file descriptor. The received file descriptor is stored in the integer pointed to by fd_received.
 *
 * @param unix_socket The Unix domain socket file descriptor to receive the FD from.
 * @param fd_received Pointer to an integer where the received file descriptor will be stored.
 * @return 0 on success, or -1 if an error occurs (an error message is printed via perror).
 *
 * @note Ensure that unix_socket is a valid, connected Unix domain socket.
 */
int recv_fd(int unix_socket) {
    struct msghdr msg = {0};
    char m_buffer[256];
    struct iovec io = { .iov_base = m_buffer, .iov_len = sizeof(m_buffer) };
    
    char c_buffer[CMSG_SPACE(sizeof(int))];
    msg.msg_control = c_buffer;
    msg.msg_controllen = sizeof(c_buffer);
    msg.msg_iov = &io;
    msg.msg_iovlen = 1;

    if (recvmsg(unix_socket, &msg, 0) < 0) {
        perror("recvmsg");
        return -1;
    }

    struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
    int fd_received = -1;
    if (cmsg && cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_RIGHTS) {
        memcpy(&fd_received, CMSG_DATA(cmsg), sizeof(fd_received));
    } else {
        fprintf(stderr, "Did not receive file descriptor\n");
        return -1;
    }
    return fd_received;
}