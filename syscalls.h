#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <sys/socket.h>

void *Malloc(size_t size);
 
size_t Read(int fildes, void *buffer, size_t nbytes);
size_t Write(int fildes, const void *buffer, size_t nbytes);

size_t Fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t Fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);

FILE *Fopen(const char *pathname, const char *mode);
int Fclose(FILE *stream);

int Pipe(int pipefd[2]);
int Nanosleep(const struct timespec *req, struct timespec *rem);
pid_t Fork(void);

void Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int Socket(int domain, int type, int protocol);
ssize_t Recv(int sockfd, void *buf, size_t len, int flags);
void Close(int fd);
ssize_t Send(int sockfd, const void *buf, size_t len, int flags);

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

#endif