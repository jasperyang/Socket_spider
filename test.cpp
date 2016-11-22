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




void* qurl_check(URL url) {

    sleep(1); //简单的反爬虫

    pthread_mutex_t mtx_qurl;
    pthread_mutex_init(&mtx_qurl,NULL);
    Async_print* ac = Async_print::getInstance();
    parser* par = parser::getInstance();
    //现在开始从队列里提取出url开始递归
    cout<<"starting qurl_check()..."<<endl;
    while(1) {
        SocketMaker* sm = new SocketMaker();
        Downloader* dow = new Downloader();
        sleep(1);
        pthread_mutex_lock(&mtx_qurl);
            if(par->qurl->get_msg(url) == 0) {
                ac->print("looking into:\n" + url.url + "\n");
                strcpy(url.host, basic_url);    //这里是不是可以优化？
                sockfd = sm->createSocket(url.host, 80);
                sm->sendHttpRequest(sockfd, url);
                dow->recvHttpRespond(sockfd, par->ch);
                thpool_add_work(par->thpool, (void (*)(void *)) reptile_regex, (void *) par->init_regex(par->ch, str, 1, url.url));
                thpool_add_work(par->thpool, (void (*)(void *)) reptile_regex, (void *) par->init_regex(par->ch, str, 2, url.url));
                sm->closeSocket(sockfd);
            }
        delete dow,sm;
        pthread_mutex_unlock(&mtx_qurl);
    }
}

void* qready_pop(URL url) {    //开始从par->qready里下载html文件
    sleep(1);

    pthread_mutex_t mtx_qready;
    pthread_mutex_init(&mtx_qready, NULL);
    Async_print* ac = Async_print::getInstance();
    parser* par = parser::getInstance();
    cout<<"starting qready_pop()..."<<endl;
    while(1) {
        Downloader* dow = new Downloader();
        SocketMaker* sm = new SocketMaker();
        sleep(1);
        pthread_mutex_lock(&mtx_qready);
            if(par->qready->get_msg(url) == 0) {
                ac->print(url.url + "\n");
                strcpy(url.host, basic_url);    //这里是不是可以优化？
                sockfd = sm->createSocket(url.host, 80);
                sm->sendHttpRequest(sockfd, url);
                dow->recvHttpRespond(sockfd, par->ch, url.url.substr(1, url.url.size() - 1), par);
                sm->closeSocket(sockfd);
            }
        delete dow,sm;
        pthread_mutex_unlock(&mtx_qready);
    }
}





int main(int argc, char const *argv[])
{
    memset(dataHash,0,LENGTH*17*sizeof(unsigned char));
    pthread_t     k;

    SocketMaker* sm = new SocketMaker();
    regexPara* reg;
    Downloader* dow = new Downloader();
    parser* par = parser::getInstance();
    Async_print* ac = Async_print::getInstance();
    URL url;

    strcpy(url.host,basic_url);
    url.url = "/sohu/";
    ac->print("looking into:\n"+url.url+"\n");

    memset(par->ch,0,sizeof(par->ch));          //缓冲区开辟空间

    sockfd = sm->createSocket(url.host,80);
    sm->sendHttpRequest(sockfd,url);
    dow->recvHttpRespond(sockfd,par->ch);
    reg = par->init_regex(par->ch,str,1,url.url);
    thpool_add_work(par->thpool,(void(*)(void*))reptile_regex,(void*)reg);
    reg = par->init_regex(par->ch,str2,2,url.url);
    thpool_add_work(par->thpool,(void(*)(void*))reptile_regex,(void*)reg);
    sm->closeSocket(sockfd);

    //cout<<"starting the qurl queue pushing..."<<endl;
    pthread_create(&k, NULL, (void *(*)(void *))qurl_check, &url);


    //cout<<"starting the qready queue popping..."<<endl;
    pthread_create(&k, NULL,(void *(*)(void *))qready_pop,&url);

    sleep(1000);            //给子线程执行时间

    delete sm,dow,par;
    return 0;
}