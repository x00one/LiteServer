#ifndef LITESERVER_MIME_H
#define LITESERVER_MIME_H


class Mime {
protected:
    std::map<std::string, const char*> mapping;

public:
    Mime();

    const char* parsePath(const char * path);
};


#endif //LITESERVER_MIME_H
