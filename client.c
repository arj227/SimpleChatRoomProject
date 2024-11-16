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

    unsigned int port = 8080;
    struct sockaddr_in serverAddress;
    int socket = connectServer(port, &serverAddress, ipaddress);

    printf("Connected to the server.\n");

    const char *MESSAGE = "Connected";
    while (1) {
        Send(socket, MESSAGE, strlen(MESSAGE), 0);
        printf("Message sent: %s", MESSAGE);
        
        // Sleep for 2 seconds
        sleep(2);
    }



    Close(socket);
    return 0;
}
