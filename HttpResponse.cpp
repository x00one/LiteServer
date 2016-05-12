#include <string.h>
#include <sstream>

#include "inc/HttpResponse.h"

HttpResponse::HttpResponse() {
}

std::string HttpResponse::compileHeader() {
    std::stringstream headers;
    headers << "HTTP/1.1 " << this->status << " " << this->reason << "\r\n";

    if (this->contentLength > 0) {
        headers << "Content-Length: " << this->contentLength << "\r\n";
    }
    
    for(std::vector<const char*>::iterator it = this->headers.begin(); it != this->headers.end(); ++it) {
        headers << *it << "\r\n";
    }
    
    headers << "\r\n";
    
    return headers.str();
}
