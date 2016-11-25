//
// Created by Jacob Timber on 11/14/16.
//
#include "SocketMaker.hpp"
#include "parser.h"
#include "Downloader.hpp"
#include <pthread.h>
#include <unistd.h>
#include "Async_pirnt.h"
#include <mutex>


char str[] = {"=\"[A-Za-z0-9]*[^\"].s?html"};
char str2[] = {"href=\"[A-Za-z0-9]*/"};
int sockfd;
int counterer = 0;



//---------这是用来检查qurl里面东西的函数----------
void* qurl_check(struct URL url) {

    pthread_mutex_t mtx;
    pthread_mutex_init(&mtx,NULL);
    Async_print* ac = Async_print::getInstance();
    parser* par = parser::getInstance();
    argument* arg;
    arg = new argument;
    //现在开始从队列里提取出url开始递归
    ac->print("starting qurl_check()...");
    while(1) {                                                  //这块是无限循环，没有做些线程应有的信号量的处理。
        SocketMaker* sm = SocketMaker::getInstance();
        Downloader* dow = new Downloader();
            if(par->qurl->get_msg(url) == 0) {
                ac->print("looking into:\n" + arg->url.url + "\n");
                arg->url = url;
                strcpy(arg->str,str);
                strcpy(arg->str2,str2);
                strcpy(arg->url.host, basic_url);    //这里是不是可以优化？
                arg->flag = 1;
                sockfd = sm->createSocket(arg->url.host, 80,arg,0);
                pthread_mutex_lock(&mtx);
                counterer++;
                pthread_mutex_unlock(&mtx);
                if(counterer %= 500) {
                    sleep(2);
                }
            }
        delete dow;
    }
    delete arg;

}



//----------这是用来弹出qready队列里东西来处理的函数---------------
void* qready_pop(struct URL url) {    //开始从par->qready里下载html文件

    Async_print* ac = Async_print::getInstance();
    parser* par = parser::getInstance();
    argument* arg;
    arg = new argument;
    ac->print("starting qready_pop()...");
    while(1) {
        Downloader* dow = new Downloader();
        SocketMaker* sm = SocketMaker::getInstance();
            if(par->qready->get_msg(url) == 0) {
                ac->print(url.url + "\n");
                arg->url = url;
                strcpy(arg->url.host, basic_url);    //这里是不是可以优化？
                arg->flag = 2;
                sockfd = sm->createSocket(url.host, 80,arg,0);
            }
        delete dow;
    }
    delete arg;
}





int main(int argc, char const *argv[])
{
    memset(dataHash,0,LENGTH*17*sizeof(unsigned char));
    pthread_t     k;

    SocketMaker* sm = SocketMaker::getInstance();
    Downloader* dow = new Downloader();
    Async_print* ac = Async_print::getInstance();
    argument* arg;
    arg = new argument;

    strcpy(arg->url.host,basic_url);
    arg->url.url = "/sohu/";
    ac->print("looking into:\n"+arg->url.url+"\n");

    arg->flag = 1;
    strcpy(arg->str,str);
    strcpy(arg->str2,str2);

    sockfd = sm->createSocket(arg->url.host,80,arg,1);

    //cout<<"starting the qurl queue pushing..."<<endl;
    pthread_create(&k, NULL, (void *(*)(void *))qurl_check, &arg->url);


    //cout<<"starting the qready queue popping..."<<endl;
    pthread_create(&k, NULL,(void *(*)(void *))qready_pop,&arg->url);

    sleep(10000);            //给子线程执行时间

    delete dow;
    return 0;
}