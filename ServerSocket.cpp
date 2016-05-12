#include <signal.h>

#include "inc/ServerSocket.h"

void ServerSocket::init(unsigned short portno) {
    sockaddr_in serveraddr;

    int optval = 1;
    setsockopt(this->fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));

    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(portno);
    bind(this->fd, (struct sockaddr *) &serveraddr, sizeof(serveraddr));
    listen(this->fd, 10);
}

ServerSocket::ServerSocket(std::string portno) : Socket() {
    this->init((unsigned short)atoi(portno.c_str()));
}

ServerSocket::ServerSocket(unsigned short portno) : Socket() {
    this->init(portno);
}

ClientSocket* ServerSocket::accept(bool nonBlocking) {
    if (nonBlocking) {
        fd_set rfds;
        timespec tv;
        int retval;
        sigset_t emptyset;

        FD_ZERO(&rfds);
        FD_SET(this->fd, &rfds);

        tv.tv_sec = 1;
        tv.tv_nsec = 0;

        sigemptyset(&emptyset);

        retval = pselect(this->fd + 1, &rfds, NULL, NULL, &tv, &emptyset);

        if (retval <= 0) {
            return NULL;
        }
    }
    
    sockaddr_in clientaddr;
    unsigned int clientlen = sizeof(clientaddr);
    
    int newFd = ::accept(this->fd, (struct sockaddr *) &clientaddr, &clientlen);
    
    return new ClientSocket(newFd);
}
