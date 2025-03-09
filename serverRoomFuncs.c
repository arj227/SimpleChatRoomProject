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
void activeChatRoom(int firstClient, int chatRoomNumber, int socketWithParent) {
    int clients[32];
    clients[0] = firstClient;

    while(1) {
        char buffer[32] = "hello from child";

        fprintf(stdout, "Room %d: Sending Message\n", chatRoomNumber);
        Send(clients[0], buffer, sizeof(buffer), 0);
        sleep(3);
    }
}

int joinRoom(int chatRoomSocket, int clientToSend) {
    struct msghdr message;
    memset(&message, 0, sizeof(message));

    // Dummy data required for sendmsg().
    char buffer[1] = {0};
    struct iovec iov;
    iov.iov_base = buffer;
    iov.iov_len = sizeof(buffer);
    message.msg_iov = &iov;
    message.msg_iovlen = 1;

    // Allocate buffer for the ancillary data.
    char ancillaryBuffer[CMSG_SPACE(sizeof(clientToSend))];
    memset(ancillaryBuffer, 0, sizeof(ancillaryBuffer));
    message.msg_control = ancillaryBuffer;
    message.msg_controllen = sizeof(ancillaryBuffer);

    // Set up the ancillary data header.
    struct cmsghdr *cmsg = CMSG_FIRSTHDR(&message);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(clientToSend));
    memcpy(CMSG_DATA(cmsg), &clientToSend, sizeof(clientToSend));

    // Send the message.
    if (sendmsg(chatRoomSocket, &message, 0) < 0) {
        perror("sendmsg");
        return -1;
    }
    return 0;
}


int recieveNewClient(int parentSocket) {
    struct msghdr message;
    memset(&message, 0, sizeof(message));

    char buffer[1];
    struct iovec iov;
    iov.iov_base = buffer;
    iov.iov_len = sizeof(buffer);
    message.msg_iov = &iov;
    message.msg_iovlen = 1;

    // Buffer for the ancillary data.
    char ancillaryBuffer[CMSG_SPACE(sizeof(int))];
    memset(ancillaryBuffer, 0, sizeof(ancillaryBuffer));
    message.msg_control = ancillaryBuffer;
    message.msg_controllen = sizeof(ancillaryBuffer);

    if (recvmsg(socket, &message, 0) < 0) {
        perror("recvmsg");
        return -1;
    }

    // Retrieve the file descriptor from the ancillary data.
    struct cmsghdr *cmsg = CMSG_FIRSTHDR(&message);
    if (cmsg == NULL) {
        fprintf(stderr, "No ancillary data received\n");
        return -1;
    }
    if (cmsg->cmsg_level != SOL_SOCKET || cmsg->cmsg_type != SCM_RIGHTS) {
        fprintf(stderr, "Invalid ancillary data received\n");
        return -1;
    }

    int fd_received;
    memcpy(&fd_received, CMSG_DATA(cmsg), sizeof(fd_received));
    return fd_received;
}