#ifndef _SOCKETMAKER_H
#define _SOCKETMAKER_H


#ifdef __cpluscplus
extern "C" {
#endif


#include <iostream>
#include "basic_properties.h"
#include <event.h>
#include "Async_pirnt.h"

using namespace std;


typedef struct arg {
	char ch[1000000];
	struct URL url;
	char str[50];
	char str2[50];
	int flag;
    struct event* func;
}argument;


class SocketMaker
{
private:
	static SocketMaker* sc;
	SocketMaker();
	~SocketMaker();
	class Garbo {  //它的唯一工作就是在析构函数中删除parser的实例
	public:
		~Garbo() {
			if (SocketMaker::sc)
				delete SocketMaker::sc;
		}
	};
	static Garbo garbo;
public:
	struct event_base* base;
	static SocketMaker* getInstance();
	int createSocket(char *hostname,int port,argument* arg,int flag);
	int sendHttpRequest(int sockfd,struct URL url);
	int closeSocket(int sockfd);
};

void on_read1(int sock,short event,void* arg);
void on_read2(int sock,short event,void* arg);
void on_send(int sock,short event,void* arg);

#ifdef __cpluscplus
};
#endif

#endif