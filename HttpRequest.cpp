#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "inc/HttpRequest.h"

HttpRequest::HttpRequest(char buffer[], size_t bufferSize) {
    char* saveptr;
    char* subsaveptr;
    char* token;
    char* subtoken;
    
    this->method = NULL;
    this->uri = NULL;
    this->queryString = NULL;
    
    token = strtok_r(buffer, "\n", &saveptr);
    
    unsigned int i = 0;
    subtoken = strtok_r(token, " ", &subsaveptr);
    
    while(subtoken != NULL) {
        if (i == 0) {
            this->method = subtoken;
        } else if (i == 1) {
            this->uri = subtoken;
        }
        
        subtoken = strtok_r(NULL, " ", &subsaveptr);
        
        i += 1;
    }
    
    if (this->method != NULL) {
        printf("Method = %s\n", this->method);
    }
    
    if (this->uri != NULL) {
        unsigned int index = 0;
        while(this->uri[index] != 0) {
            if (this->uri[index] == '?') {
                this->uri[index] = 0;
                this->queryString = this->uri + index + 1;
                break;
            }
            
            index += 1;
        }
        
        printf("URI    = %s\n", this->uri);
        
        if (this->queryString != NULL) {
            printf("QUERY_STRING = %s\n", this->queryString);
        }
    }

    this->headerDataBlock = new HeaderDataBlock(saveptr, bufferSize - (saveptr-buffer));
}

HttpRequest::~HttpRequest() {
    delete this->headerDataBlock;
}


