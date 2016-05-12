#ifndef LITESERVER_HEADERDATABLOCK_H
#define LITESERVER_HEADERDATABLOCK_H

#include <vector>

class HeaderDataBlock {
public:
    std::vector<const char*> headers;
    const char* data;
    size_t dataSize;

    HeaderDataBlock(char *buffer, size_t bufferSize);
};


#endif //LITESERVER_HEADERDATABLOCK_H
