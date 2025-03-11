#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stddef.h>
#include <stdio.h>
#include <sys/time.h>
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
void Setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
ssize_t Recv(int sockfd, void *buf, size_t len, int flags);
void Close(int fd);
ssize_t Send(int sockfd, const void *buf, size_t len, int flags);
void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
void Listen(int sockfd, int backlog);

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int Socketpair(int domain, int type, int protocol, int sv[2]);
int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

#endif