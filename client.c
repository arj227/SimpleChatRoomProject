#include "clientFuncs.h"
#include "syscalls.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(/*int argc, char const* argv[]*/) {
    unsigned int port = 443;
    struct sockaddr_in serverAddress;
    int socket = connectServer(port, &serverAddress);

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
