#include <cstring>
#include <map>
#include <string>
#include "inc/Mime.h"

const char *Mime::parsePath(const char *path) {
    const char* dot = strrchr(path, '.');

    std::map<std::string, const char*>::iterator item = this->mapping.find(dot);

    if (item != this->mapping.end()) {
        return item->second;
    } else {
        return "application/octet-stream";
    }
}

Mime::Mime() {
    this->mapping[".png"] = "image/png";
    this->mapping[".jpg"] = "image/jpeg";
    this->mapping[".html"] = "text/html";
    this->mapping[".php"] = NULL;
    this->mapping[".css"] = "text/css";
    this->mapping[".js"] = "text/javascript";
}


