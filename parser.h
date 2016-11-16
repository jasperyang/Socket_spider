#ifndef _PARSER_H
#define _PARSER_H

#include <iostream>
#include "basic_properties.h"
#include "thpool.h"
using namespace std;

#ifdef __cpluscplus
extern "C" {
#endif

typedef struct regex_para {
    string buf;
    char *pattern;
    int type;
    string url_prefix;
} regexPara;


class parser {
public:
    static parser* getInstance() {   //饿汉模式的单例
        if (par == NULL) {
            par = new parser();
        }
        return par;
    };
    int getServerState(char *ch);
    int getField(string field, char *ch);
    char *getBody(char *ch);
    void saveToFile(char *buf, string filename);
    //void reptile_regex(string buf,char* pattern,int type,string url_prefix);
    struct regex_para *init_regex(string buf, char *pattern, int type, string url_prefix);
    char ch[1000000]; //下载得来的字符缓冲区
    queue<struct URL> qurl;
    queue<struct URL> qready;
    threadpool thpool;
private:
    static parser *par;
    parser();
    ~parser();
    class CGarbo {  //它的唯一工作就是在析构函数中删除parser的实例
    public:
        ~CGarbo() {
            if (parser::par)
                delete parser::par;
        }
    };
    static CGarbo Garbo;
};

void reptile_regex(string buf, char *pattern, int type, string url_prefix);

#ifdef __cpluscplus
}
#endif

#endif