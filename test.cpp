//
// Created by Jacob Timber on 11/14/16.
//
#include "SocketMaker.hpp"
#include "parser.h"
#include "Downloader.hpp"


int main(int argc, char const *argv[])
{
    memset(dataHash,0,LENGTH*17*sizeof(unsigned char));


    char str[] = {"=\"[A-Za-z0-9]*[^\"].s?html"};
    char str2[] = {"href=\"[A-Za-z0-9]*/"};
    int sockfd;
    char pattern_url[128] = {0};
    struct URL url;
    string url_prefix = "/sohu";
    strcpy(url.host,basic_url);
    url.url = "/sohu/";
    cout<<"looking into:\n";
    cout<<url.host<<url.url<<endl;

    SocketMaker* sm = new SocketMaker();
    parser* par = parser::getInstance();
    Downloader* dow = new Downloader();
    regexPara* reg;
    memset(par->ch,0,sizeof(par->ch));

    sockfd = sm->createSocket(url.host,80);
    sm->sendHttpRequest(sockfd,url);
    dow->recvHttpRespond(sockfd,par->ch);
    strcpy(pattern_url,str);
    string sh = string(par->ch);
    reg = par->init_regex(sh,pattern_url,1,url_prefix);
    cout<<reg->url_prefix<<" "<<reg->type<<" "<<reg->pattern<<endl;
    thpool_add_work(par->thpool,(void(*)(void*))reptile_regex,(void*)reg);
    strcpy(pattern_url,str2);
    reg = par->init_regex(sh,pattern_url,2,url_prefix);
    thpool_add_work(par->thpool,(void(*)(void*))reptile_regex,(void*)reg);
    sm->closeSocket(sockfd);

    //现在开始从队列里提取出url开始递归
    while(!par->qurl.empty()) {
        url = par->qurl.front();
        par->qurl.pop();
        url_prefix = url.url;
        cout<<"looking into:\n";
        cout<<url.host<<url.url<<endl;
        strcpy(url.host,basic_url);    //这里
        // 是不是可以优化？
        url.url = url_prefix + "/";
        sockfd = sm->createSocket(url.host,80);
        sm->sendHttpRequest(sockfd,url);
        dow->recvHttpRespond(sockfd,par->ch);
        strcpy(pattern_url,str);
        string sh = string(par->ch);
        reg = par->init_regex(sh,pattern_url,1,url_prefix);
        thpool_add_work(par->thpool,(void(*)(void*))reptile_regex,(void*)reg);
        strcpy(pattern_url,str2);
        reg = par->init_regex(sh,pattern_url,2,url_prefix);
        thpool_add_work(par->thpool,(void(*)(void*))reptile_regex,(void*)reg);
        sm->closeSocket(sockfd);
        break;
    }

    //开始从par->qready里下载html文件
    while(!par->qready.empty()) {
        url = par->qready.front();
        par->qready.pop();
        cout<<"downloading...\n";
        cout<<url.url<<endl;
        strcpy(url.host,basic_url);    //这里是不是可以优化？
        sockfd = sm->createSocket(url.host,80);
        sm->sendHttpRequest(sockfd,url);
        dow->recvHttpRespond(sockfd,par->ch,url.url.substr(1,url.url.size()-1),par);
        sm->closeSocket(sockfd);
        break;
    }

    delete sm,dow,par;
    return 0;
}