#include "serverFuncs.h"
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

#include <ifaddrs.h>
#include <sys/types.h>
#include <net/if.h>


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
 * @throw The function exits with a status of `1` if:
 * - The hostname cannot be obtained (error in `gethostname`).
 * - Host information cannot be retrieved (error in `gethostbyname`).
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
 * @brief Prints non-loopback IPv4 addresses available on the host.
 *
 * This function retrieves all network interfaces using getifaddrs() and then iterates
 * through the list to find interfaces with an IPv4 address that are not flagged as loopback.
 * It then prints the interface name and its corresponding IP address.
 *
 * @note If getifaddrs() fails, the function prints an error message and exits.
 */
void printExternalIP() {
    struct ifaddrs *ifaddr, *ifa;
    char host[NI_MAXHOST];

    // Retrieve the linked list of network interfaces.
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }

    printf("Available non-loopback IPv4 addresses:\n");

    // Loop through all interfaces.
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;

        // Check if the interface address is IPv4.
        if (ifa->ifa_addr->sa_family == AF_INET) {
            // Skip loopback interfaces.
            if (ifa->ifa_flags & IFF_LOOPBACK)
                continue;
            
            // Convert the interface address to a numeric string.
            int s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
            if (s != 0) {
                fprintf(stderr, "getnameinfo() failed: %s\n", gai_strerror(s));
                exit(EXIT_FAILURE);
            }
            printf("%s: %s\n", ifa->ifa_name, host);
        }
    }
    freeifaddrs(ifaddr);
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
 */
void unpackage(__uint128_t *package, char* username, char* password, uint8_t *chatRoom) {
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

/**
 * @brief Creates a new chat room and returns the communication socket.
 *
 * This function establishes a new chat room by creating a Unix domain socket pair and then
 * forking the process. The child process becomes the new chat room by calling activeChatRoom(),
 * while the parent process closes the child's end of the socket pair and returns its own end, which
 * is used for communicating with the chat room.
 *
 * The forked child process does not return from this function because it enters the chat room's main loop.
 *
 * @param chatRooms An array of pid_t used to store the process IDs of chat room processes. The index
 *                  corresponding to the client's chat room number is updated with the child's PID.
 * @param client A pointer to a ClientData structure that contains the client's information, including
 *               the chat room number.
 *
 * @return int On the parent process, returns the socket file descriptor that will be used for
 *             communicating with the newly created chat room. In the child process, the function does
 *             not return.
 */
int createRoom(pid_t *chatRooms, struct ClientData *client) {
    fprintf(stdout, "running fork!\n");

    int socketPairHolder[2];
    Socketpair(AF_UNIX, SOCK_DGRAM, 0, socketPairHolder);

    chatRooms[client->chatRoom] = Fork();

    // CREATING A CHILD AND A NEW CHAT ROOM
    if (chatRooms[client->chatRoom] == 0) {
        fprintf(stdout, "activating new room: %d\n", client->chatRoom);
        fprintf(stdout, "----------------------------------\n\n");

        Close(socketPairHolder[0]);
        activeChatRoom(client, client->chatRoom, socketPairHolder[1]);
        exit(0);
    // PARENT CODE AFTER CHATROOM IS CREATED
    } else if (chatRooms[client->chatRoom] != 0) {
        Close(socketPairHolder[1]);
        return socketPairHolder[0];
    }
    __builtin_unreachable();
}

/**
 * @brief Resets the file descriptor set for the parent process.
 *
 * This function clears the provided file descriptor set and then adds the server socket along with
 * all active chat room sockets (stored in the chatRoomSockets array) to the set. A chat room socket
 * is considered active if its value is not -1.
 *
 * @param chatRoomSockets An array of 16 integers representing chat room socket file descriptors.
 *                        A value of -1 indicates that the slot is unused.
 * @param serverSocket The file descriptor for the server (listening) socket.
 * @param selectFDParent A pointer to the fd_set structure that will be reset and populated.
 */
void resetFD_SETParentSide(int *chatRoomSockets, int serverSocket, fd_set *selectFDParent) {
    FD_ZERO(selectFDParent);

    FD_SET(serverSocket, selectFDParent);
    for (int i = 0; i < 16; i ++) {
        if (chatRoomSockets[i] != -1) {
            FD_SET(chatRoomSockets[i], selectFDParent);
        }
    }
}

/**
 * @brief Calculates the maximum file descriptor value plus one for select() on the parent side.
 *
 * This function iterates through the server socket and an array of chat room socket file descriptors,
 * and returns the value of the highest valid file descriptor plus one. This value is used as the first
 * argument to the select() function.
 *
 * @param chatRoomSockets An array of 16 integers representing chat room socket file descriptors.
 *                        A value of -1 indicates an inactive or unused socket.
 * @param serverSocket The file descriptor for the server (listening) socket.
 * @return int The maximum file descriptor value found among the server socket and chat room sockets,
 *         plus one.
 */
int calculateMaxfdParentSide(int *chatRoomSockets, int serverSocket) {
    int maxfd = serverSocket;
    for (int i = 0; i < 16; i++) {
        if (chatRoomSockets[i] > maxfd && chatRoomSockets[i] != -1) {
            maxfd = chatRoomSockets[i];
        }
    }
    return maxfd + 1;
}