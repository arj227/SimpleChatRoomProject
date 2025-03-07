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

    printf("Connected to the server.\n");

    int test = 1;
    while (test == 1) {
        Send(socket, &package, 128, 0);
        printf("Package Sent");
        
        // Sleep for 2 seconds
        sleep(2);
    }


    Close(socket);
    return 0;
}
