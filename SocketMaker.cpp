#include "SocketMaker.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <regex>//正则表达式
#include <map>
#include <fstream>
#include "gethostbyname.c"
#include <event.h>


SocketMaker::SocketMaker() {}
SocketMaker::~SocketMaker() {}

int SocketMaker::createSocket(char *hostname,int port) {
    struct sockaddr_in servAddr;
    struct hostent * host;
    int sockfd;
    host = gethostbyname(hostname);
    if(host == NULL) {
        perror("dns 解析失败");
    }
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr = *((struct in_addr *)host->h_addr);
    servAddr.sin_port = htons(port);
    bzero(&(servAddr.sin_zero),8);

    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd == -1) {
        perror("socket 创建失败");
    }

    if(connect(sockfd,(struct sockaddr *)&servAddr,sizeof(struct sockaddr_in)) == -1) {
        perror("connect 失败");
    }
    return sockfd;
}

int SocketMaker::sendHttpRequest(int sockfd,struct URL url) {
    char sendBuf[BUF_SIZE];
    int sendSize;
    //构建一个http请求
    sprintf(sendBuf,"GET %s HTTP/1.1 \r\nHost: %s \r\nConnection: Close \r\n\r\n",url.url.c_str(),url.host);
    if((sendSize = send(sockfd,sendBuf,BUF_SIZE,0)) == -1) {
        perror("send 失败");
    }
    return 0;
}

int SocketMaker::closeSocket(int sockfd) {
    close(sockfd);

    return 0;
}

