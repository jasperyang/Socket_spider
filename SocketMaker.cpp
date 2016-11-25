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
#include "parser.h"
#include "Downloader.hpp"
#include <fcntl.h>


SocketMaker::SocketMaker() {
    base = event_base_new();
    event_base_dispatch(base);              //初始dispatch,没有这个就会出问题，没太懂，以后再看吧
}
SocketMaker::~SocketMaker() {
    event_base_free(base);
}

SocketMaker* SocketMaker::getInstance() {   //饿汉模式的单例
    if (sc == NULL) {
        sc = new SocketMaker();
    }
    return sc;
};

SocketMaker* SocketMaker::sc = NULL;              //单例初始化，不写这个编译就会报undefined identifier
SocketMaker::Garbo SocketMaker::garbo;            //初始化垃圾工人



void on_send(int sock,short event,void* arg) {
    struct event* read_ev;

    argument* argv = (argument*)arg;
    parser* par = parser::getInstance();
    SocketMaker* sc = SocketMaker::getInstance();
    Async_print* ac = Async_print::getInstance();


    sc->sendHttpRequest(sock,argv->url);

    ac->print("send  " + argv->url.url);

    event_del(argv->func);                          //删除调用这个回调函数的事件
    argv->func = read_ev;


    if (argv->flag == 1) {
        event_set(read_ev,sock,EV_READ|EV_PERSIST,on_read1,(void*)argv);
        event_base_set(sc->base,read_ev);
        event_add(read_ev,NULL);
    }
    else if(argv->flag == 2) {
        event_set(read_ev,sock,EV_READ|EV_PERSIST,on_read2,(void*)argv);
        event_base_set(sc->base,read_ev);
        event_add(read_ev,NULL);
    }
    else {
        ac->print("Exception of on_send");
    }
}


void on_read1(int sock,short event,void* arg) {
    Downloader* dow = new Downloader();
    parser* par = parser::getInstance();
    SocketMaker* sm = SocketMaker::getInstance();
    argument* argv = (argument*)arg;
    char* p = argv->ch;
    p = dow->recvHttpRespond(sock,argv->ch);
    thpool_add_work(par->thpool, (void (*)(void *)) reptile_regex, (void *) par->init_regex(p, argv->str, 1, argv->url.url));
    thpool_add_work(par->thpool, (void (*)(void *)) reptile_regex, (void *) par->init_regex(p, argv->str2, 2, argv->url.url));
    sm->closeSocket(sock);
    event_del(argv->func);
}

void on_read2(int sock,short event,void* arg) {
    Downloader* dow = new Downloader();
    parser* par = parser::getInstance();
    SocketMaker* sm = SocketMaker::getInstance();
    argument* argv = (argument*)arg;
    dow->recvHttpRespond(sock, argv->ch, argv->url.url.substr(1, argv->url.url.size()-1), par);
    sm->closeSocket(sock);
    event_del(argv->func);
}

int SocketMaker::createSocket(char *hostname,int port,argument* arg,int flag) {
    struct event listen_ev;
    Async_print* ac = Async_print::getInstance();
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

    arg->func = &listen_ev;

    //evutil_make_socket_nonblocking(sockfd);

    ac->print("create " + arg->url.url);

    event_set(&listen_ev,sockfd,EV_WRITE|EV_PERSIST,on_send,(void*)arg);
    event_base_set(base,&listen_ev);
    event_add(&listen_ev,NULL);

    if (flag == 1) {
        event_base_dispatch(base);
    }
    else {
        event_base_dispatch(base);
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

