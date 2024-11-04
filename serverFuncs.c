#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080

#include <ctype.h>

/**
 * @brief Creates and configures a server socket for listening to incoming connections.
 * 
 * This function initializes a socket, sets it up with the necessary options, binds it to a
 * specified address and port, and then prepares it to listen for incoming client connections.
 * It is intended for use in setting up a TCP server socket on an IPv4 network.
 * 
 * @param serverSocket Pointer to an integer where the created socket file descriptor will be stored.
 *                     This file descriptor will be used to accept incoming client connections.
 * 
 * @param socketOption Pointer to an integer representing socket options. Typically set to 1 to
 *                     enable certain options, such as address reuse. This function uses it to set
 *                     `SO_REUSEADDR` and `SO_REUSEPORT` on the socket.
 * 
 * @param address Pointer to a `struct sockaddr_in` structure where the function will set the
 *                address family, IP address, and port number. It should be pre-allocated by the
 *                caller. The `sin_family` will be set to `AF_INET`, `sin_addr.s_addr` to `INADDR_ANY`
 *                (all network interfaces), and `sin_port` to the specified `PORT`.
 * 
 * @note This function exits the program with `EXIT_FAILURE` if any step fails, printing an
 *       appropriate error message to `stderr` using `perror`.
 */
void createSocket(int *serverSocket, int *socketOption, struct sockaddr_in *address) {
    // Creating socket file descriptor
    if ((*serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // prepares the socket to be bound to the port (changes the settings)
    if (setsockopt(*serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, socketOption, sizeof(socketOption))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(*serverSocket, (struct sockaddr*) address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // sets the socket to a listening state
    if (listen(serverSocket, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
}


void StringToLower(char *str) {
    int counter = 0;
    while (str[counter] != '\0') {
        str[counter] = tolower(str[counter]);
        counter++;
    }
}

/**
 * @brief takes argc and argv and saves file names to sorted array
 * 
 * @param argc argc from cmd line
 * @param argv argv from cmd line
 * @param filename pointer array to strings which are the file names length of 2
 */
int parseArgs(int argc, char const *argv[], char *hexArray[]) {
    const char* intVar = "-i";
    const char* floatVar = "-f";

    if (argc != 3) {
        fprintf(stderr, "not enough arguments!\n");
        return 1;
    }

    // checks to see if -i -f is present and then sets it to lowercase
    if (argv[1][0] != '-') {
        fprintf(stderr, "No var type given\n");
        return 1;
    }
    char argvVal[8];
    strncpy(argvVal, argv[1], 7);
    argvVal[7] = '\0';
    StringToLower(argvVal);
    
    if (strcmp(argvVal, intVar) == 0) { // integer hex logic
        strcpy(hexArray[0], intVar);
    } else if (strcmp(argvVal, floatVar) == 0) { // float hex logic
        strcpy(hexArray[0], floatVar);
    }

    // checks if argv is small enough before copying it to the array
    if (strlen(argv[2]) < 32) {
        strcpy(hexArray[1], argv[2]);
    } else {
        fprintf(stderr, "string of hexadecimals too long :(");
        return 1;
    }

    if (hexArray[0] == NULL) {
        fprintf(stderr, "failed to save to array");
        return 1;
    }

    return 0;
}


