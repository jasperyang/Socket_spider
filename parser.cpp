#include <regex>
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include "BL/bloom.hpp"
#include <fstream>
#include "Async_pirnt.h"
#include <cstring>

Async_print* ac = Async_print::getInstance();

parser::parser() {
    puts("Making threadpool with 10 threads");
    thpool = thpool_init(10);
    qurl = new Message_Queue<struct URL>;
    qready = new Message_Queue<struct URL>;
}
parser::~parser() {
    puts("killing threadpool");
    thpool_destroy(thpool);
}

int parser::getServerState(char *ch) {
    while('\0' != *ch) {
        if(*ch == 'H' && *(ch+1) == 'T' && *(ch+2) == 'T' && *(ch+3) == 'P') {
            break;
        }
        ch++;
    }
    ch += 9; //找到200的位置
    char result[3] = {*ch,*(ch+1),*(ch+2)};
    return atoi(result);
}

int parser::getField(string field, char *ch) {

}

char* parser::getBody(char *ch) {
    while('\0' != *ch) {
        if(*ch == '\r' && *(ch+1) == '\n' && *(ch+2) == '\r' && *(ch+3) == '\n') {
            break;
        }
        ch++;
    }
    ch += 4;
    return ch;
}


void parser::saveToFile(char *buf, string filename) {
    ofstream file(filename.c_str());
    if(file.is_open()) {
        file << buf;
    }
    file.close();
}

void parser::saveToFile(int counter,string buf, string url) {
    ofstream file;
    file.open(result_file,ios::app);
    if(!file) {
        ac->print("can't open "+result_file);
        abort();
    }
    else {
        file << counter + "  " + url + "  " + buf + "\n";
        file.close();
    }
}

struct regex_para* parser::init_regex(string b,char* p,int t,string url_prex) {
    regexPara* reg;
    reg = new regexPara;
    reg->buf = b;
    reg->pattern = p;
    reg->type = t;
    if (url_prex != "") {
        reg->url_prefix = url_prex;
    }
    return reg;
}


parser* parser::par = NULL;
parser* par = parser::getInstance();



parser* parser::getInstance() {   //饿汉模式的单例
    if (par == NULL) {
        par = new parser();
    }
    return par;
};

parser::Garbo parser::garbo;            //初始化垃圾工人



void reptile_regex(regexPara* reg) {
    regex img_regex(reg->pattern);

    // 使用类 regex_iterator 来进行多次搜索.
    //cout << " =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  = " << endl;
    auto words_begin = sregex_iterator(reg->buf.begin(), reg->buf.end(), img_regex);
    auto words_end = sregex_iterator();
    if (reg->type == 1) {
        for (sregex_iterator i = words_begin; i != words_end; ++i) {
            smatch match = *i;
            string match_str = match.str();
            match_str = match_str.substr(2,match_str.size()-1);
            struct URL url;
            strcpy(url.host,basic_url);
            url.url = reg->url_prefix + match_str;
            if (bf_dataCheck(url.url.c_str(),dataHash) != 1) {
                bf_dataHash(url.url.c_str(),dataHash);
                par->qready->push_msg(url);
            }
            else {
                ac->print("duplicate\n");
            }
        }
    }
    else {
        for (sregex_iterator i = words_begin; i != words_end; ++i) {
            smatch match = *i;
            string match_str = match.str();
            match_str = match_str.substr(6,match_str.size()-1);
            if(match_str == "/") {          //第一个匹配到的是"/"这个特例
                continue;
            }
            struct URL url;
            strcpy(url.host,basic_url);
            url.url = reg->url_prefix + match_str;
            if (bf_dataCheck(url.url.c_str(),dataHash) != 1) {
                bf_dataHash(url.url.c_str(),dataHash);
                par->qurl->push_msg(url);
            }
            else {
                ac->print("duplicate\n");
            }
            cout<<url.url<<endl;
        }
    }
}