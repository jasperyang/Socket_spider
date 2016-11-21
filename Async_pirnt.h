//
// Created by Jacob Timber on 11/18/16.
// 这个类是用于互斥地打印用的，因为我发现多线程的情况下，打印信息是很容易就混乱的。
//

#ifndef SOCKET_SPIDER_ASYNC_PIRNT_H
#define SOCKET_SPIDER_ASYNC_PIRNT_H

#ifdef __cpluscplus
extern "C" {
#endif

#include <iostream>
#include <mutex>
using namespace std;

class Async_print {
private:
    static Async_print* ac;
    mutex mtx;
    int control;
    Async_print(){
        control = 1;        //控制是否要print
    };
    ~Async_print();
public:
    void print(string str);
    static Async_print* getInstance() {
      if(ac == NULL) {
          ac = new Async_print();
      }
      return ac;
    };
};

#ifdef __cpluscplus
};
#endif

#endif //SOCKET_SPIDER_ASYNC_PIRNT_H
