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
    printf("Package Sent\n");

    pid_t forkReturn = Fork();
    fprintf(stdout, "--> ");
    fflush(stdout);
    while (1) {
        if (forkReturn != 0) {
            int fromServerReturn = readFromServer(socket);
            if (fromServerReturn == 1) break;

        } else if (forkReturn == 0) {
            sendToServer(socket);
        }
    }
    Close(socket);
    return 0;
}
