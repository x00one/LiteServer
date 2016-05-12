#include "inc/Socket.h"

Socket::Socket(int fd) {
    if (fd == -1) {
        this->fd = socket(AF_INET, SOCK_STREAM, 0);
    } else {
        this->fd = fd;
    }
}
Socket::~Socket() {
    if (this->fd > -1) {
        this->close();
    }
}

void Socket::close() {
    ::close(this->fd);
    this->fd = -2;
}
