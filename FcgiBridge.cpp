#include <vector>
#include "inc/FcgiBridge.h"

FcgiBridge::FcgiBridge(ClientSocket *socket, HttpResponse* response) {
    this->socket = socket;
    this->response = response;
}

int FcgiBridge::init_header(int type, int requestId, size_t contentLength, size_t paddingLength, FCGI_Header * header)
{
    if (contentLength > 65535 || paddingLength > 255)
        return 0;
    header->version = FCGI_VERSION_1;
    header->type = (unsigned char) type;
    header->requestIdB1 = (unsigned char) ((requestId >> 8) & 0xff);
    header->requestIdB0 = (unsigned char) (requestId & 0xff);
    header->contentLengthB1 =
        (unsigned char) ((contentLength >> 8) & 0xff);
    header->contentLengthB0 = (unsigned char) ((contentLength) & 0xff);
    header->paddingLength = (unsigned char) paddingLength;
    header->reserved = 0;
    return 1;
}

void FcgiBridge::init_begin_request_body(int role, FCGI_BeginRequestBody * begin_request_body)
{
    begin_request_body->roleB1 = (unsigned char) (((role >> 8) & 0xff));
    begin_request_body->roleB0 = (unsigned char) (role & 0xff);
    begin_request_body->flags = 0;
    memset(begin_request_body->reserved, 0,
           sizeof(begin_request_body->reserved));
}

void FcgiBridge::addParams(char* buffer, unsigned int* lastPosition, const char* paramName, const char* paramValue) {
    size_t nameLength = strlen(paramName);
    size_t valueLength = strlen(paramValue);
    int pos = *lastPosition;
    
    buffer[pos] = nameLength;

    if (valueLength <= 127) {
        buffer[pos + 1] = valueLength;

        memcpy(buffer + pos + 2, paramName, nameLength);
        memcpy(buffer + pos + 2 + nameLength, paramValue, valueLength);

        *lastPosition = (*lastPosition) + 2 + nameLength + valueLength;
    } else {
        buffer[pos + 1] = (char)((valueLength >> 24) | 0x80);
        buffer[pos + 2] = (char)(valueLength >> 16);
        buffer[pos + 3] = (char)(valueLength >> 8);
        buffer[pos + 4] = (char)(valueLength);

        memcpy(buffer + pos + 5, paramName, nameLength);
        memcpy(buffer + pos + 5 + nameLength, paramValue, valueLength);

        *lastPosition = (*lastPosition) + 5 + nameLength + valueLength;
    }
}


void FcgiBridge::query(const char *method, const char *file, const char *queryString, HeaderDataBlock *headerDataBlock) {
    ClientSocket* socket = new ClientSocket("127.0.0.1", 9000);
    
    char fcgiBuffer[8192 * 4];
    size_t fcgiBufferSaveIndex = 0;
    
    // Build the BEGIN_REQUEST frame.
    FCGI_Header header;
    FCGI_BeginRequestBody beginBody;
    
    init_header(FCGI_BEGIN_REQUEST, 1, sizeof(FCGI_BeginRequestBody), 0, &header);
    appendToBuffer(fcgiBuffer, &fcgiBufferSaveIndex, (char*)&header, sizeof(header));
    
    init_begin_request_body(1, &beginBody);
    appendToBuffer(fcgiBuffer, &fcgiBufferSaveIndex, (char*)&beginBody, sizeof(beginBody));
    
    
    // Build the PARAMS frame.
    char bufferParams[4096];
    unsigned int saveIndex = 0;
    addParams(bufferParams, &saveIndex, "REQUEST_METHOD", method);
    addParams(bufferParams, &saveIndex, "SCRIPT_FILENAME", file);
    addParams(bufferParams, &saveIndex, "HTTP_ACCEPT", "*/*");
    
    if (queryString != NULL) {
        addParams(bufferParams, &saveIndex, "QUERY_STRING", queryString);
    }

    std::vector<const char*>::iterator it;
    char headerName[1024];
    char headerValue[1024];
    int index;
    int indexName;
    int indexValue;
    bool parsingName;

    for (it=headerDataBlock->headers.begin(); it < headerDataBlock->headers.end(); it++) {
        index = 0;
        indexName = 0;
        indexValue = 0;
        parsingName = true;

        while((*it)[index] != 0) {
            if (parsingName) {
                if ((*it)[index] == ':') {
                    headerName[indexName] = 0;
                    parsingName = false;

                    if ((*it)[index + 1] == ' ') {
                        index += 1;
                    }
                } else {
                    if ((*it)[index] == '-') {
                        headerName[indexName] = '_';
                    } else {
                        headerName[indexName] = toupper((*it)[index]);
                    }

                    indexName += 1;
                }
            } else {
                headerValue[indexValue] = (*it)[index];
                indexValue += 1;
            }

            index += 1;
        }

        headerValue[indexValue] = 0;

        printf("name = [%s] value = [%s]\n", headerName, headerValue);

        if (strcmp(headerName, "COOKIE") == 0) {
            addParams(bufferParams, &saveIndex, "HTTP_COOKIE", headerValue);
        } else {
            addParams(bufferParams, &saveIndex, headerName, headerValue);
        }
    }

    init_header(FCGI_PARAMS, 1, saveIndex, 0, &header);
    appendToBuffer(fcgiBuffer, &fcgiBufferSaveIndex, (char*)&header, sizeof(header));
    appendToBuffer(fcgiBuffer, &fcgiBufferSaveIndex, bufferParams, saveIndex);
 
    init_header(FCGI_PARAMS, 1, 0, 0, &header);
    appendToBuffer(fcgiBuffer, &fcgiBufferSaveIndex, (char*)&header, sizeof(header));
    
    
    // Build the STDIN frame.
    // TODO Check the coherence between 'content-length' header and headerDataBlock->dataSize.
    if (headerDataBlock->dataSize > 0) {
        init_header(FCGI_STDIN, 1, headerDataBlock->dataSize, 0, &header);
        appendToBuffer(fcgiBuffer, &fcgiBufferSaveIndex, (char*)&header, sizeof(header));
        appendToBuffer(fcgiBuffer, &fcgiBufferSaveIndex, headerDataBlock->data, headerDataBlock->dataSize);
    }

    init_header(FCGI_STDIN, 1, 0, 0, &header);
    appendToBuffer(fcgiBuffer, &fcgiBufferSaveIndex, (char*)&header, sizeof(header));
    
    
    // Send the query to FastCGI processor.
    socket->send(fcgiBuffer, fcgiBufferSaveIndex);
    

    // Read back the answer.
    FCGI_Header responseHeader;
    unsigned int packetCount = 0;

    do {
        socket->receive(&responseHeader, sizeof(FCGI_Header));
        printf("FCGI packet type [%i]\n", responseHeader.type);

        if (responseHeader.type == 0) { // Bad parsing.
            break;
        }

        if (responseHeader.type == FCGI_STDERR) {
            int bodysize = responseHeader.contentLengthB1;
            bodysize <<= 8;
            bodysize += responseHeader.contentLengthB0;

            printf("bodysize = %i\n", bodysize);
            char answerBuffer[bodysize];
            socket->receive(answerBuffer, bodysize);


            // Read padding.
            char paddingContent[responseHeader.paddingLength];
            socket->receive(paddingContent, responseHeader.paddingLength);
        } else if (responseHeader.type == FCGI_STDOUT) {
            packetCount += 1;

            int bodysize = responseHeader.contentLengthB1;
            bodysize <<= 8;
            bodysize += responseHeader.contentLengthB0;

            printf("bodysize = %i\n", bodysize);
            char answerBuffer[bodysize];
            socket->receive(answerBuffer, bodysize);

            if (packetCount == 1) {
                HeaderDataBlock *headerDataBlock = new HeaderDataBlock(answerBuffer, bodysize);

                this->response->headers.insert(this->response->headers.end(), headerDataBlock->headers.begin(),
                                         headerDataBlock->headers.end());
                this->response->contentLength = -1;

                std::string answer = this->response->compileHeader();
                this->socket->send(answer.c_str(), answer.length());

                this->socket->send(headerDataBlock->data, headerDataBlock->dataSize);

                delete headerDataBlock;
            } else {
                this->socket->send(answerBuffer, bodysize);
            }


            // Read padding.
            char paddingContent[responseHeader.paddingLength];
            socket->receive(paddingContent, responseHeader.paddingLength);
        }
    } while(responseHeader.type != FCGI_END_REQUEST);

    
    socket->close();
    delete socket;
}
