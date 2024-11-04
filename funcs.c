
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080

#include <ctype.h>

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
    /* takes the argc argv and an array for storing the file names in order
     * makes the value of argv lowercase and then checks to see if its -f or -i
     * and then saves the input to an array
     * 
     * I also put effort into writing clean code here!
     */
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


