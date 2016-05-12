#ifndef CLIENT_SOCKET_H
# define CLIENT_SOCKET_H

#include <fstream>

#include "Socket.h"

class ClientSocket : public Socket
{
public:
    ClientSocket(int fd = -1);
    ClientSocket(const char* ip, u_int16_t port);

    void setReceiveTimeout(unsigned long seconds);

    ssize_t send(const void* buffer, size_t bufferSize);
    void sendFile(const char* filePath, unsigned int fileSize);
    ssize_t receive(void* buffer, size_t bufferSize);
};

#endif /* CLIENT_SOCKET_H */
