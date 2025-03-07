#include "serverFuncs.h"
#include "syscalls.h"

#include <netinet/in.h>   // Needed for sockaddr_in structure
#include <stdio.h>         // Needed for printf, fprintf, perror
#include <stdlib.h>        // Needed for exit, EXIT_FAILURE
#include <string.h>        // Needed for strcmp, strlen, strcpy
#include <sys/socket.h>    // Needed for socket, bind, listen, setsockopt
#include <unistd.h>        // Needed for close, if used to close sockets
#include <arpa/inet.h>
#include <netdb.h>


// Optional: Include ctype.h if StringToLower is used
#include <ctype.h>         // Needed for tolower (if StringToLower is used)
#define PORT 8080

/**
 * @brief Converts a string to lowercase.
 * 
 * This function takes a string and converts each character to lowercase.
 * It modifies the string in place, meaning the original string is overwritten
 * with its lowercase version.
 * 
 * @param str A pointer to the string to be converted. This string must be null-terminated,
 *            and it will be modified by the function to contain only lowercase characters.
 */
void StringToLower(char *str) {
    int counter = 0;
    while (str[counter] != '\0') {
        str[counter] = tolower(str[counter]);
        counter++;
    }
}

/**
 * @brief Parses command-line arguments to extract a password after the -p flag.
 * 
 * This function checks if the command-line arguments contain the "-p" flag followed by
 * a password string. If the conditions are met, it saves the password string to `hexArray`.
 * The function also ensures the password length is within acceptable limits.
 * 
 * @param argc The count of command-line arguments (from `main`).
 * @param argv Array of command-line arguments (from `main`).
 * @param hexArray A pointer to a character array where the password will be stored.
 *                 The array should be pre-allocated with sufficient space for the
 *                 password (maximum 31 characters).
 * 
 * @note This function exits the program if:
 * - The `-p` flag is not present.
 * - The password is too long (more than 31 characters).
 * - There are not enough command-line arguments.
 * 
 * Example Usage:
 * ```c
 * int main(int argc, char const *argv[]) {
 *     char hexArray[32];  // Allocate enough space for the password
 *     parseArgs(argc, argv, hexArray);
 *     printf("Password: %s\n", hexArray);
 *     return 0;
 * }
 * ```
 */
void parseArgs(int argc, char const *argv[], char *hexArray) {
    const char* passwordFlag = "-p";

    if (argc < 3) {
        fprintf(stderr, "not enough arguments!\n");
        exit(1);
    }

    // checks to see if -i -f is present and then sets it to lowercase
    if (strcmp(argv[1], passwordFlag) != 0) {
        fprintf(stderr, "no -p flag found\n");
        exit(1);
    }

    // checks if the given password is short enough
    if (strlen(argv[2]) > 31) {
        fprintf(stderr, "string of hexadecimals too long :(");
        exit(1);
    }
    
    strcpy(hexArray, argv[2]);
    hexArray[31] = '\0';

    if (hexArray == NULL) {
        fprintf(stderr, "failed to save to array");
        exit(1);
    }
}



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
    *serverSocket = Socket(AF_INET, SOCK_STREAM, 0);

    // prepares the socket to be bound to the port (changes the settings)
    Setsockopt(*serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEADDR, socketOption, sizeof(int));
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    Bind(*serverSocket, (struct sockaddr*) address, sizeof(*address));

    // sets the socket to a listening state
    Listen(*serverSocket, 3);
}

/**
 * @brief Retrieves and prints the local IP address of the host machine.
 * 
 * This function obtains the hostname of the local machine, retrieves the associated
 * IP address, and prints it in a readable format. It uses `gethostname` to get the
 * machine's hostname and `gethostbyname` to obtain the IP address associated with that
 * hostname. The IP address is then converted to a string using `inet_ntoa` and displayed.
 * 
 * @note This function is designed for IPv4 addresses. If the machine has multiple network
 *       interfaces, this function will print the IP address of the first interface.
 * 
 * @error The function exits with a status of `1` if:
 * - The hostname cannot be obtained (error in `gethostname`).
 * - Host information cannot be retrieved (error in `gethostbyname`).
 * 
 * @example
 * ```c
 * int main() {
 *     printLocalIP();
 *     return 0;
 * }
 * ```
 */
void printLocalIP() {
    char hostname[256];
    struct hostent *host_entry;
    char *IP;

    // Get the hostname
    if (gethostname(hostname, sizeof(hostname)) == -1) {
        perror("gethostname");
        exit(1);
    }

    // Get host information
    host_entry = gethostbyname(hostname);
    if (host_entry == NULL) {
        perror("gethostbyname");
        exit(1);
    }

    // Convert the IP to a string and print it
    IP = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0]));
    printf("Local IP address: %s\n", IP);
}


/**
 * @brief un packs the username, password, and chatroom from the client
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
 * @patempPackageram chatRoom uint8_t
 */
void unpackage(__uint128_t *package, char* username, char* password, uint8_t *chatRoom) {
    // *package = 0;
    // *package |= (usernameHolder & (((__uint128_t)1 << 60) - 1)) << 68;  // top 60 bits
    // *package |= (passwordHolder & (((__uint128_t)1 << 60) - 1)) << 8;   // middle 60 bits
    // *package |= (uint8_t) *chatRoom;                                      // bottom 8 bits

    __uint128_t tempPackage = *package;
    *chatRoom = (uint8_t) tempPackage & 0xFF;
    tempPackage = tempPackage >> 8;
    uint64_t passwordPlaceholder = tempPackage & 0xFFFFFFFFFFFFFFF;
    tempPackage = tempPackage >> 60;
    uint64_t usernamePlaceholder = tempPackage & 0xFFFFFFFFFFFFFFF;

    for (size_t i = 0; i < 7; i++) {
        char currentChar = passwordPlaceholder & 0xFF;
        passwordPlaceholder >>= 8;

        if (currentChar == '\0') {
            password[i] = '\0';
            break;
        }
        password[i] = currentChar;
    }
    password[7] = '\0';

    for (size_t i = 0; i < 7; i++) {
        char currentChar = usernamePlaceholder & 0xFF;
        usernamePlaceholder >>= 8;

        if (currentChar == '\0') {
            username[i] = '\0';
            break;
        }
        username[i] = currentChar;
    }
    username[7] = '\0';

    fprintf(stdout, "Username: %s\nPassword: %s\nChat Room: %hhu\n", username, password, *chatRoom);
}