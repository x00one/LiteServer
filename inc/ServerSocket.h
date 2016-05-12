#ifndef SERVER_SOCKET_H
# define SERVER_SOCKET_H

#include <string>
#include "ClientSocket.h"

class ServerSocket : public Socket
{
protected:
    void init(unsigned short portno);

public:
    ServerSocket(std::string portno);
    ServerSocket(unsigned short portno);
    
    ClientSocket* accept(bool nonBlocking = false);
};

#endif /* SERVER_SOCKET_H */
