#include "syscalls.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>



/**
 * @brief Parses command-line arguments to extract an IP address after the -i flag.
 * 
 * This function checks if the command-line arguments contain the "-i" flag followed by
 * an IP address string. If the conditions are met, it saves the IP address string to `ipArray`.
 * The function also ensures the IP address length is within acceptable limits for IPv4.
 * 
 * @param argc The count of command-line arguments (from `main`).
 * @param argv Array of command-line arguments (from `main`).
 * @param ipArray A pointer to a character array where the IP address will be stored.
 *                The array should be pre-allocated with sufficient space for the IP 
 *                address (maximum 15 characters for IPv4).
 * 
 * @note This function exits the program if:
 * - The `-i` flag is not present.
 * - The IP address is too long (more than 15 characters).
 * - There are not enough command-line arguments.
 * 
 * Example Usage:
 * ```c
 * int main(int argc, char const *argv[]) {
 *     char ipArray[16];  // Allocate enough space for the IP address
 *     parseArgs(argc, argv, ipArray);
 *     printf("IP Address: %s\n", ipArray);
 *     return 0;
 * }
 * ```
 */
void parseArgs(int argc, char const *argv[], char *ipArray) {
    const char* ipFlag = "-i";

    if (argc < 3) {
        fprintf(stderr, "Not enough arguments!\n");
        exit(1);
    }

    // Check if the -i flag is present
    if (strcmp(argv[1], ipFlag) != 0) {
        fprintf(stderr, "No -i flag found\n");
        exit(1);
    }

    // Check if the given IP address is short enough for IPv4
    if (strlen(argv[2]) > 15) {
        fprintf(stderr, "IP address too long\n");
        exit(1);
    }

    // Copy IP address to ipArray
    strcpy(ipArray, argv[2]);
    ipArray[15] = '\0';  // Ensure null termination

    if (ipArray == NULL) {
        fprintf(stderr, "Failed to save to array\n");
        exit(1);
    }
}

/**
 * @brief Prompts the user to enter a username and password, with error checking.
 * 
 * This function reads the username and password from the terminal and performs
 * basic error checking to ensure input was successful. It limits the input to
 * prevent buffer overflow.
 * 
 * @param username Pointer to a pre-allocated character array for the username.
 *                 It should be large enough to hold up to 15 characters plus
 *                 a null terminator (at least 50 bytes).
 * @param password Pointer to a pre-allocated character array for the password.
 *                 It should be large enough to hold up to 15 characters plus
 *                 a null terminator (at least 50 bytes).
 * 
 * @return Returns 0 if successful, -1 if input fails or an error occurs.
 */
int userLogIn(char* username, char* password, uint8_t *chatRoom) {
    printf("Enter username: ");
    if (scanf("%15s", username) != 1) {  // Limit to 15 chars to avoid overflow
        fprintf(stderr, "Error reading username.\n");
        return -1;
    }

    printf("Enter password: ");
    if (scanf("%15s", password) != 1) {  // Limit to 15 chars to avoid overflow
        fprintf(stderr, "Error reading password.\n");
        return -1;
    }

    // Optional: Clear the newline character if there’s one in the buffer
    if (strlen(username) == 32 || strlen(password) == 32) {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF); // Flush remaining input
    }

    printf("Which room to connect to: ");
    if (scanf("%d", chatRoom) != 1) {
        fprintf(stderr, "error reading chatroom selection.\n");
        return -1;
    }

    return 0;
}

void assemblyPackage(uint32_t *package, char* username, char* password, int *chatRoom)


/**
 * @brief Creates a TCP socket and connects to a server at the specified port.
 * 
 * This function configures a socket for IPv4 (AF_INET) and TCP (SOCK_STREAM),
 * binds it to the provided port, and attempts to connect to the server with
 * the IP address `127.0.1.1`.
 * 
 * @param port Integer specifying the port number to connect to.
 * @param serverAddress Pointer to a `sockaddr_in` structure that is configured
 *                      with the family, port, and IP address.
 * 
 * @return Returns the socket file descriptor if the connection is successful;
 *         returns a negative value if there’s a failure in socket creation or connection.
 * 
 * @note The IP address is hardcoded to `127.0.1.1` (localhost). Modify it to connect
 *       to other servers.
 * 
 * @example
 * struct sockaddr_in serverAddr;
 * int clientSocket = connectServer(8080, &serverAddr);
 * if (clientSocket < 0) {
 *     fprintf(stderr, "Connection failed\n");
 * }
 */
int connectServer(int port, struct sockaddr_in *serverAddress, char *ipAddress) {
    int portINT = port;

    serverAddress->sin_family = AF_INET;
    serverAddress->sin_port = htons(portINT);
    serverAddress->sin_addr.s_addr = inet_addr(ipAddress);

    int clientSocket;
    clientSocket = Socket(AF_INET, SOCK_STREAM, 0);

    Connect(clientSocket, (struct sockaddr*) serverAddress, sizeof(struct sockaddr_in));

    fprintf(stdout, "Connected to Server!");

    return clientSocket;
}