#include <pthread.h>
#include <stdio.h>
#include <sys/stat.h>
#include <signal.h>

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#include "inc/ServerSocket.h"
#include "inc/HttpRequest.h"
#include "inc/HttpResponse.h"
#include "inc/FcgiBridge.h"
#include "inc/IniParser.h"
#include "inc/Mime.h"

bool stop = false;
std::string documentRoot;
Mime mineMapping;

void* clientThread (void* arg) {
    ClientSocket* client = (ClientSocket*)arg;
    client->setReceiveTimeout(5);

    char buffer[(1024 * 4) + 1];
    ssize_t size = client->receive(buffer, 1024 * 4);
    buffer[size] = 0;
    
    if (size > 0) {
        HttpRequest* request = new HttpRequest(buffer, (size_t)size);
        HttpResponse* response = new HttpResponse();

        if ((request->uri != NULL) && (request->method != NULL)) {
            std::string path = documentRoot;
            path += request->uri;

            struct stat fileInfo;
            int statRes = stat(path.c_str(), &fileInfo);

            if ((statRes == -1) || ((fileInfo.st_mode & S_IFMT) != S_IFREG)) {
                response->status = "404";
                response->reason = "Not found";
                response->contentLength = 9;

                response->headers.push_back("Content-Type: text/html");

                std::string answer = response->compileHeader();
                client->send(answer.c_str(), answer.length());
                client->send("Not found", response->contentLength);
            } else {
                response->status = "200";
                response->reason = "OK";

                // Get content-type.
                const char * contentType = mineMapping.parsePath(path.c_str());

                if (contentType != NULL) {
                    std::string contentTypeHeader = "Content-Type: ";
                    contentTypeHeader += contentType;
                    response->headers.push_back(contentTypeHeader.c_str());

                    response->contentLength = fileInfo.st_size;

                    std::string answer = response->compileHeader();
                    client->send(answer.c_str(), answer.length());
                    client->sendFile(path.c_str(), fileInfo.st_size);
                } else {
                    FcgiBridge* fcgi = new FcgiBridge(client, response);
                    fcgi->query(request->method, path.c_str(), request->queryString, request->headerDataBlock);
                    delete fcgi;
                }
            }
        }

        delete request;
        delete response;
    }
    
    client->close();
    delete client;
    
    return NULL;
}

void sig_handler (int signo) {
    if (signo == SIGINT) {
        std::cout << "Received ctrl-c, shutting down..." << std::endl;
        stop = true;
    }
}

int main (int argc, char* argv[]) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &sig_handler;

    sigaction(SIGINT, &sa, NULL);


    IniParser* ini = new IniParser("server.ini");
    documentRoot = ini->get("document_root");
    std::string port = ini->get("port");
    delete(ini);
    
    ServerSocket* server = new ServerSocket(port);
    ClientSocket* client = NULL;

    while (!stop) {
        client = server->accept(true);
        
        if (client != NULL) {
            pthread_t tid;
            pthread_create(&tid, NULL, &clientThread, client);
            pthread_detach(tid);
        }
    }
    
    delete(server);

    std::cout << "Shutdown complete." << std::endl;
    
    return 0;
}
