#ifndef _PARSER_H
#define _PARSER_H

#include <iostream>
#include "basic_properties.h"
#include "thpool.h"
#include "Message_Queue.cpp"
#include <cstring>

using namespace std;

#ifdef __cpluscplus
extern "C" {
#endif

typedef struct regex_para {   //reptile_regex函数的参数结构体
    string buf;
    int type;
    string url_prefix;
    char *pattern;
} regexPara;


class parser {
private:
    static parser* par;
    parser();
    ~parser();
    class Garbo {  //它的唯一工作就是在析构函数中删除parser的实例
    public:
        ~Garbo() {
            if (parser::par)
                delete parser::par;
        }
    };
    static Garbo garbo;
public:
    static parser* getInstance();  //饿汉模式的单例
    int getServerState(char *ch);
    int getField(string field, char *ch);
    char *getBody(char *ch);
    void saveToFile(char *buf, string filename);
    void saveToFile(int counter,string buf,string url);
    //void reptile_regex(string buf,char* pattern,int type,string url_prefix);
    struct regex_para *init_regex(string buf, char *pattern, int type, string url_prefix);
    char ch[1000000]; //下载得来的字符缓冲区
    Message_Queue<struct URL>* qurl;
    Message_Queue<struct URL>* qready;
    threadpool thpool;
};

void reptile_regex(regexPara* reg);


#ifdef __cpluscplus
};
#endif

#endif