#include "syscalls.h"

#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/types.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>

void *Malloc(size_t size) {
    void *ptr;
    ptr = malloc(size);

    if (ptr == NULL) {
        fprintf(stderr, "error allocating memory\n");
        exit(errno);
    }
    return ptr;
}


size_t Read(int fildes, void *buffer, size_t nbytes) {
    ssize_t rd;
    rd = read(fildes, buffer, nbytes);

    if (rd < 0) {
        fprintf(stderr, "error reading: %s\n", strerror(errno));
        exit(errno);
    }
    return rd;
}

size_t Fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t rd;
    rd = fread(ptr, size, nmemb, stream);

    if (rd < nmemb && ferror(stream)) {
        fprintf(stderr, "error reading from file\n");
        exit(errno);
    }
    return rd;
}

size_t Write(int fildes, const void *buffer, size_t nbytes) {
    ssize_t wr;
    wr = write(fildes, buffer, nbytes);

    if (wr < 0) {
        fprintf(stderr, "error writing\n");
        exit(errno);
    }
    return wr;
}

size_t Fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t wd;
    wd = fwrite(ptr, size, nmemb, stream);

    if (wd < nmemb) {
        fprintf(stderr, "error writing\n");
        exit(errno);
    }
    return wd;
}

FILE *Fopen(const char *pathname, const char *mode) {
  FILE* fp = NULL;
  if ((fp = fopen(pathname, mode)) == NULL) {
    fprintf(stderr, "error opening file - %d (%s)\n", errno, strerror(errno));
    exit(errno);
  }
  return fp;
}

int Fclose(FILE *stream) {
  if (fclose(stream)) {
    fprintf(stderr, "error closing file - %d (%s)\n", errno, strerror(errno));
    exit(errno);
  }
  return 0;
}

// int Close(int fd) {
//     int cl;
//     cl = close(fd);

//     if (cl < 0) {
//         fprintf(stderr, "error closing\n");
//         exit(errno);
//     }
//     return cl;
// }

int Pipe(int pipefd[2]) {
    int p;
    p = pipe(pipefd);

    if (p < 0) {
        fprintf(stderr, "error with pipe");
        exit(errno);
    }
    return p;
}

int Nanosleep(const struct timespec *req, struct timespec *rem) {
    int nano;
    nano = nanosleep(req, rem);

    if (nano < 0) {
        fprintf(stderr, "Nanosleep failed");
        exit(errno);
    }
    return nano;
}

pid_t Fork(void) {
    pid_t f;
    f = fork();

    if (f < 0) {
        fprintf(stderr, "error with fork\n");
        exit(errno);
    }
    return f;
}

int Socket(int domain, int type, int protocol) {
    int sockfd;

    if ((sockfd = socket(domain, type, protocol)) < 0) {
        fprintf(stderr, "socket failed to create");
        exit(errno);
    }

    return sockfd;
}

void Setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) {
    if (setsockopt(sockfd, level, optname, optval, optlen) == -1) {
        perror("setsockopt failed");
        exit(errno);
    }
}

void Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    if (connect(sockfd, addr, addrlen) < 0) {
        fprintf(stderr, "connect error: %s\n", strerror(errno));
        exit(errno);
    }
}

ssize_t Recv(int sockfd, void *buf, size_t len, int flags) {
    ssize_t n;

    if ((n = recv(sockfd, buf, len, flags)) < 0) {
        fprintf(stderr, "recv error: %s\n", strerror(errno));
        exit(errno);
    }

    return n;
}

void Close(int fd) {
    if (close(fd) < 0) {
        fprintf(stderr, "close error: %s\n", strerror(errno));
        exit(errno);
    }
}

ssize_t Send(int sockfd, const void *buf, size_t len, int flags) {
    ssize_t n;

    if ((n = send(sockfd, buf, len, flags)) == -1) {
        fprintf(stderr, "send error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    return n;
}

void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    if (bind(sockfd, addr, addrlen) == -1) {
        perror("bind failed");
        exit(errno);
    }
}

void Listen(int sockfd, int backlog) {
    if (listen(sockfd, backlog) == -1) {
        perror("listen failed");
        exit(errno);
    }
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    int connfd;

    if ((connfd = accept(sockfd, addr, addrlen)) == -1) {
        // Error handling: Print to standard error and exit with errno
        fprintf(stderr, "Accept error: %s\n", strerror(errno));
        exit(errno);
    }

    return connfd;
}

int Socketpair(int domain, int type, int protocol, int sv[2]) {
    int n;
    if ((n = socketpair(domain, type, protocol, sv)) != 0) {
        fprintf(stderr, "socketpair error: %d", errno);
        exit(errno);
    }
    return n;
}

int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout) {
    int n;
    if ((n = select(nfds, readfds, writefds, exceptfds, timeout)) < 0) {
        perror("select error");
        exit(EXIT_FAILURE);
    }
    return n;
}