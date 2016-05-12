#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include "../HeaderDataBlock.h"

class HttpRequest
{
public:
    char* method;
    char* uri;    
    char* queryString;
    HeaderDataBlock* headerDataBlock;

    ~HttpRequest();
    HttpRequest(char buffer[], size_t bufferSize);
};

#endif /* HTTP_REQUEST_H */
