#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <string>
#include <vector>

class HttpResponse
{
public:
    const char* status;
    const char* reason;    
    size_t contentLength;
    std::vector<const char*> headers;

    HttpResponse();
    std::string compileHeader();
};

#endif /* HTTP_RESPONSE_H */
