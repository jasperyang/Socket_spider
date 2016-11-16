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

Downloader::Downloader() {}
Downloader::~Downloader() {}


int Downloader::recvHttpRespond(int sockfd,char *ch) {
    char recvBuf[BUF_SIZE];
    int recvSize;
    //获取http应答信息
    memset(recvBuf,0,sizeof(recvBuf));
    memset(ch,0,sizeof(ch));
    int count = 0;
    cout<<"start receiving...\n";
    while(recvSize = recv(sockfd,recvBuf,BUF_SIZE,0)>0) {
        count++;
        strcat(ch,recvBuf);
        memset(recvBuf,0,sizeof(recvBuf));
    }
    return 0;
}

int Downloader::recvHttpRespond(int sockfd,char *ch,string filename,parser* par) {
    char recvBuf[BUF_SIZE];
    int recvSize;
    //获取http应答信息
    memset(recvBuf,0,sizeof(recvBuf));
    memset(ch,0,sizeof(ch));
    cout<<"start receiving...\n";
    while(recvSize = recv(sockfd,recvBuf,BUF_SIZE,0)>0) {
        strcat(ch,recvBuf);
        memset(recvBuf,0,sizeof(recvBuf));
    }
    if(par->getServerState(ch) == 200) {
        //ch = par->getBody(ch);
        std::size_t found = filename.find_first_of("/");
        while (found != std::string::npos) {
            filename[found] = '_';
            found = filename.find_first_of("/", found + 1);
        }
        filename = "downloaded/" + filename;
        cout << filename << endl;
        par->saveToFile(ch, filename);
    }
    return 0;
}




