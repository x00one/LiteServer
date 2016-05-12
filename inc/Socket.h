#ifndef SOCKET_H
#define SOCKET_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class Socket
{
public:
    int fd;

    Socket(int fd = -1);
    ~Socket();
    
    void close();
};

#endif /* SOCKET_H */
