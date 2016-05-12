#include "inc/ClientSocket.h"

ClientSocket::ClientSocket(int fd) : Socket(fd) {
    printf("ClientSocket constructor\n");
}

ClientSocket::ClientSocket(const char* ip, u_int16_t port) : Socket() {
    sockaddr_in serveraddr;
    bzero((char *) &serveraddr, sizeof(serveraddr));
    
    serveraddr.sin_addr.s_addr = inet_addr(ip);
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
 
    printf("socket fd = %i\n", this->fd);
 
    //Connect to remote server
    int res = connect(this->fd , (struct sockaddr *)&serveraddr , sizeof(serveraddr));
    
    printf("connect res = %i\n", res);
}

void ClientSocket::setReceiveTimeout (unsigned long seconds) {
    timeval tv;

    tv.tv_sec = seconds;
    tv.tv_usec = 0;

    setsockopt(this->fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(struct timeval));
}

ssize_t ClientSocket::send(const void* buffer, size_t bufferSize) {
    return ::send(this->fd, buffer, bufferSize, 0);
}

void ClientSocket::sendFile(const char* filePath, unsigned int fileSize)
{
    std::ifstream is;
    is.open(filePath, std::ios::binary);
    
    char fileChunk[1024];
    unsigned int totalBytesSent = 0;
    unsigned int bytesSent = 0;
    // unsigned int n;

    // read data as a block:
    while (totalBytesSent < fileSize) {
        is.read(fileChunk, 1024);
        bytesSent = is.gcount();

        // n = ::send(this->fd, fileChunk, bytesSent, 0);
        ::send(this->fd, fileChunk, bytesSent, 0);

        totalBytesSent += bytesSent;
    }
    
    is.close();
}

ssize_t ClientSocket::receive(void* buffer, size_t bufferSize) {
    return ::recv(this->fd, buffer, bufferSize, 0);
}
