#ifndef FCGI_BRIDGE_H
#define FCGI_BRIDGE_H

#include "ClientSocket.h"
#include "Tools.h"
#include "../HeaderDataBlock.h"
#include "HttpResponse.h"

class FcgiBridge
{
    ClientSocket* socket;
    HttpResponse* response;

    static const unsigned char FCGI_VERSION_1 = 1;
    
    static const unsigned char FCGI_BEGIN_REQUEST = 1;
    static const unsigned char FCGI_END_REQUEST = 3;
    static const unsigned char FCGI_PARAMS = 4;
    static const unsigned char FCGI_STDIN = 5;
    static const unsigned char FCGI_STDOUT = 6;
    static const unsigned char FCGI_STDERR = 7;

    typedef struct {
        unsigned char version;
        unsigned char type;
        unsigned char requestIdB1;
        unsigned char requestIdB0;
        unsigned char contentLengthB1;
        unsigned char contentLengthB0;
        unsigned char paddingLength;
        unsigned char reserved;
    } FCGI_Header;

    typedef struct {
        unsigned char roleB1;
        unsigned char roleB0;
        unsigned char flags;
        unsigned char reserved[5];
    } FCGI_BeginRequestBody;
    
    

    int init_header(int type, int requestId, size_t contentLength, size_t paddingLength, FCGI_Header * header);
    void init_begin_request_body(int role, FCGI_BeginRequestBody * begin_request_body);
    void addParams(char* buffer, unsigned int* lastPosition, const char* paramName, const char* paramValue);
    
public:
    FcgiBridge(ClientSocket * socket, HttpResponse* response);

    void query(const char *method, const char *file, const char *queryString, HeaderDataBlock *headerDataBlock);
};

#endif /* FCGI_BRIDGE_H */
