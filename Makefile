# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Wwrite-strings -g

# Executable names
CLIENT_EXEC = client
SERVER_EXEC = server

# Source files
CLIENT_SRC = client.c clientFuncs.c syscalls.c
SERVER_SRC = server.c serverFuncs.c syscalls.c

# Header files
CLIENT_HEADERS = clientFuncs.h syscalls.h
SERVER_HEADERS = serverFuncs.h syscalls.h

# Default target
all: $(CLIENT_EXEC) $(SERVER_EXEC)

# Client executable
$(CLIENT_EXEC): $(CLIENT_SRC) $(CLIENT_HEADERS)
	$(CC) $(CFLAGS) -o $(CLIENT_EXEC) $(CLIENT_SRC)

# Server executable
$(SERVER_EXEC): $(SERVER_SRC) $(SERVER_HEADERS)
	$(CC) $(CFLAGS) -o $(SERVER_EXEC) $(SERVER_SRC)

# Clean up build files
clean:
	rm -f $(CLIENT_EXEC) $(SERVER_EXEC)

# Phony targets
.PHONY: all clean
