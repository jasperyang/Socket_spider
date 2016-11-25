#include "Downloader.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <map>
#include "Async_pirnt.h"
#include <pthread.h>


struct event_base* base;

Downloader::Downloader() {}
Downloader::~Downloader() {}


char* Downloader::recvHttpRespond(int sockfd,char *ch) {
    Async_print* ac = Async_print::getInstance();
    char recvBuf[BUF_SIZE];
    int recvSize;
    //获取http应答信息
    memset(recvBuf,0,sizeof(recvBuf));
    memset(ch,0,sizeof(ch));
    ac->print("start receiving...\n");
    while(recvSize = recv(sockfd,recvBuf,BUF_SIZE,0)>0) {
        strcat(ch,recvBuf);
        memset(recvBuf,0,sizeof(recvBuf));
    }
    return ch;
}

int Downloader::recvHttpRespond(int sockfd,char *ch,string filename,parser* par) {
    Async_print* ac = Async_print::getInstance();

    char recvBuf[BUF_SIZE];
    string sh;
    int recvSize;
    //获取http应答信息
    memset(recvBuf,0,sizeof(recvBuf));
    memset(ch,0,sizeof(ch));
    ac->print("start receiving...\n");
    recvSize = recv(sockfd,recvBuf,BUF_SIZE,0);
    strcat(ch,recvBuf);
    memset(recvBuf,0,sizeof(recvBuf));
    sh = ch;
    if(sh.find_first_of("Content-Length") != std::string::npos) {       //这个判断按理说是可以有效的，暂定。
        while(recvSize = recv(sockfd,recvBuf,BUF_SIZE,0)>0) {
            strcat(ch,recvBuf);
            memset(recvBuf,0,sizeof(recvBuf));
        }
    }

    if(par->getServerState(ch) == 200) {
        pthread_mutex_t mtx;
        pthread_mutex_init(&mtx,NULL);
        pthread_mutex_lock(&mtx);
        counter++;
        pthread_mutex_unlock(&mtx);
//        //ch = par->getBody(ch);
//        std::size_t found = filename.find_first_of("/");
//        while (found != std::string::npos) {
//            filename[found] = '_';
//            found = filename.find_first_of("/", found + 1);
//        }
//        filename = "downloaded/" + filename;
//        cout << filename << endl;
//        par->saveToFile(ch, filename);
        filename = "/" + filename;

        if(sh.find_first_of("Content-Length") != -1) {
            string tmp = strstr(ch,"Content-Length:") + 16;
            string content_length ;
            std::size_t found = tmp.find_first_of("\r");
            content_length = tmp.substr(0,found);
            content_length[found] = '\0';
            par->saveToFile(counter,content_length,basic_url+filename);       //按照实验要求的 content_length + url绝对路径的存储方式
        }
        else {
            sh = ch;
            par->saveToFile(counter,sh.size() + "",basic_url+filename);
        }
    }
    return 0;
}




