cmake_minimum_required(VERSION 3.2)
project(Socket_spider)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -levent")

set(SOURCE_FILES
    BL/bloom.cpp
    BL/bloom.hpp
    BL/hash.cpp
    BL/hash.hpp
    Downloader.cpp
    Downloader.hpp
    gethostbyname.c
        SocketMaker.cpp
    SocketMaker.hpp
        basic_properties.h
        parser.cpp
        parser.h basic_properties.cpp start.cpp
        thpool.c
        thpool.h
        Message_Queue.cpp Async_pirnt.cpp Async_pirnt.h)

add_executable(Socket_spider ${SOURCE_FILES} start.cpp)