#include "syscalls.h"
#include "clientFuncs.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <inttypes.h>


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
 * @details
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
    fprintf(stdout, "username must be 7 or less characters all extra characters will be removed\n");
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
    int placeHolder = 0;
    if (scanf("%d", &placeHolder) != 1) {
        fprintf(stderr, "error reading chatroom selection.\n");
        return -1;
    }
    if (placeHolder > 255) {
        fprintf(stderr, "chatroom selected out of bounds.\n");
        return -1;
    }
    *chatRoom = (u_int8_t) placeHolder;

    return 0;
}

/**
 * @brief Packages the username, password, and chatroom for the server
 * 
 * @details Package Format (128 total bits):
 *              - bits 127 - 68 --> username
 *              - bits 67  - 8  --> password
 *              - bits 7   - 0  --> chatroom
 *          the char strings are stored little endien i guess where value 0 is the
 *          least significant bits of the password section, making unpacking a bit
 *          easier i think, I haven't written that code yet
 * 
 * @param package __uint128_t the package to be sent
 * @param username char*
 * @param password char*
 * @param chatRoom uint8_t
 */
void assemblyPackage(__uint128_t *package, char* username, char* password, uint8_t *chatRoom) {
    if (username == NULL || password == NULL || chatRoom == NULL) {
        fprintf(stderr, "Invalid input: NULL pointer.\n");
        exit(-1);
    }
    // fprintf(stdout, "begin Packaging");

    uint64_t usernameHolder = 0;
    uint64_t passwordHolder = 0;

    size_t usernameLen = strlen(username);
    size_t passwordLen = strlen(password);

    if (usernameLen > 15) usernameLen = 15;  // Only up to 15 bytes (60 bits)
    if (passwordLen > 15) passwordLen = 15;

    // Pack username into the top 60 bits
    for (size_t i = 0; i < usernameLen; i++) {
        usernameHolder |= ((__uint128_t)username[i] & 0xFF) << (i * 8);
    }

    // Pack password into the next 60 bits
    for (size_t i = 0; i < passwordLen; i++) {
        passwordHolder |= ((__uint128_t)password[i] & 0xFF) << (i * 8);
    }

    *package = 0;
    *package |= (usernameHolder & (((__uint128_t)1 << 60) - 1)) << 68;  // top 60 bits
    *package |= (passwordHolder & (((__uint128_t)1 << 60) - 1)) << 8;   // middle 60 bits
    *package |= (uint8_t) *chatRoom;                                      // bottom 8 bits

    // printf("Package: 0x%032" PRIX64 "%016" PRIX64 "\n", (uint64_t)(*package >> 64), (uint64_t)(*package));

}


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

    // fprintf(stdout, "Connected to Server!\n");

    return clientSocket;
}


int readFromServer(int socket) {
    uint16_t messageLength;
    ssize_t n = Read(socket, &messageLength, sizeof(messageLength));
    char *message = malloc(messageLength);
    if (message == NULL) {
        perror("malloc");
        exit(1);
    }
    Read(socket, message, messageLength);

    if (n > 0) {
        if (message[0] == '$') {
            exit(0);
        }

        fprintf(stdout, "\n%s\n--> ", message);
        fflush(stdout);
        return 0;
    } else if (n == 0) {
        fprintf(stdout, "\nServer closed the connection.\n");
        return 1;
    } else {
        perror("Read error");
        return 1;
    }
}

void sendToServer(int socket) {
    struct MessagePacket messagePacket;
    if (fgets(messagePacket.message, sizeof(messagePacket.message), stdin) != NULL) {
        // Remove the trailing newline, if present.
        messagePacket.message[strcspn(messagePacket.message, "\n")] = '\0';
    }

    messagePacket.messageLength = strlen(messagePacket.message) + 1;
    Send(socket, &messagePacket, sizeof(messagePacket.messageLength) + messagePacket.messageLength, 0);

    if (messagePacket.message[0] == '$') {
        userCommand(messagePacket.message);
    }
    fprintf(stdout, "--> ");
    fflush(stdout);
}

void userCommand(char *buffer) {
    if (strcmp(buffer, "$exit") == 0) {
        exit(0);
    }
}