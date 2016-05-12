#include <cstdio>
#include "HeaderDataBlock.h"

HeaderDataBlock::HeaderDataBlock(char *buffer, size_t bufferSize) {
    unsigned int index = 0;
    unsigned int start = 0;
    bool rDetected = false;

    while (buffer[index] != 0) {
        if (buffer[index] == '\r') {
            buffer[index] = 0;
            rDetected = true;
        }

        if (buffer[index] == '\n') {
            buffer[index] = 0;

            if ((!rDetected && buffer[index - 1] == 0) || (rDetected && buffer[index - 2] == 0)) {
                start = index + 1;

                this->data = buffer + start;
                this->dataSize = bufferSize - start;

                break;
            } else {
                this->headers.push_back(buffer + start);

                printf("header -> %s\n", buffer + start);

                start = index + 1;
            }

            rDetected = false;
        }

        index += 1;
    }
}
