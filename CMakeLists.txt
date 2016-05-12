cmake_minimum_required(VERSION 3.5)
project(LiteServer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -O0")

set(SOURCE_FILES
    inc/ClientSocket.h
    inc/FcgiBridge.h
    inc/HttpRequest.h
    inc/HttpResponse.h
    inc/IniParser.h
    inc/ServerSocket.h
    inc/Socket.h
    inc/Tools.h
    ClientSocket.cpp
    FcgiBridge.cpp
    HttpRequest.cpp
    HttpResponse.cpp
    IniParser.cpp
    server.cpp
    ServerSocket.cpp
    Socket.cpp
    Tools.cpp inc/Mime.h Mime.cpp HeaderDataBlock.cpp HeaderDataBlock.h)

add_executable(LiteServer ${SOURCE_FILES})

TARGET_LINK_LIBRARIES(LiteServer pthread)
