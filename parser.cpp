#include <regex>
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include "BL/bloom.hpp"
#include <fstream>
#include <mutex>

std::mutex mtx;

parser::parser() {
    puts("Making threadpool with 10 threads");
    thpool = thpool_init(10);
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
    cout<<"saving..."<<endl;
    cout<< strlen(buf)<<endl;
    ofstream file(filename.c_str());
    if(file.is_open()) {
        file << buf;
    }
    file.close();
}


struct regex_para* parser::init_regex(string buf,char* pattern,int type,string url_prefix) {
    regexPara* reg;
    reg = (struct regex_para*)malloc(sizeof(struct regex_para));
    reg->buf = buf;
    reg->pattern = pattern;
    reg->type = 1;
    reg->url_prefix = url_prefix;
    return reg;
}

parser* par = parser::getInstance();    //给下面的函数使用


void reptile_regex(string buf,char* pattern,int type,string url_prefix) {
    regex img_regex(pattern);
    string splash = "/";

    // 使用类 regex_iterator 来进行多次搜索.
    cout << " =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  = " << endl;
    auto words_begin =
            sregex_iterator(buf.begin(), buf.end(), img_regex);
    auto words_end = sregex_iterator();
    if (type == 1) {
        for (sregex_iterator i = words_begin; i != words_end; ++i) {
            smatch match = *i;
            string match_str = match.str();
            match_str = match_str.substr(2,match_str.size()-1);
            //cout<<match_str<<endl;
            struct URL url;
            strcpy(url.host,basic_url);
            string tmp = url_prefix + splash + match_str;
            url.url = tmp;
            //cout<<url.host<<url.url<<endl;
            if (bf_dataCheck(url.url.c_str(),dataHash) != 1) {
                bf_dataHash(url.url.c_str(),dataHash);
                par->qready.push(url);
            }
            else {
                printf("duplicate\n");
            }
        }
    }
    else {
        for (sregex_iterator i = words_begin; i != words_end; ++i) {
            smatch match = *i;
            string match_str = match.str();
            match_str = match_str.substr(6,match_str.size()-1);
            //cout<<match_str<<endl;
            struct URL url;
            strcpy(url.host,basic_url);
            string tmp = url_prefix + splash + match_str;
            url.url = tmp;
            if (bf_dataCheck(url.url.c_str(),dataHash) != 1) {
                bf_dataHash(url.url.c_str(),dataHash);
                par->qurl.push(url);
            }
            else {
                printf("duplicate\n");
            }
            //cout<<url.host<<url.url<<endl;
        }
    }
}