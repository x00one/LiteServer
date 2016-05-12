#include <string.h>
#include <string>

void appendToBuffer(char* buffer, size_t *lastPosition, const char* data, size_t dataLength) {
    int pos = *lastPosition;
    memcpy(buffer + pos, data, dataLength);
    *lastPosition = (*lastPosition) + dataLength;
}

void trim(std::string &str)
{
    while(str[0] == ' ') {
        str.erase(0, 1);
    }
    
    while((str[str.size() - 1] == ' ') || (str[str.size() - 1] == '\r')) {
        str.erase(str.size() - 1, 1);
    }
}
