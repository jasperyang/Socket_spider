#ifndef _SOCKETMAKER_H
#define _SOCKETMAKER_H

#include <iostream>
#include "basic_properties.h"

using namespace std;


class SocketMaker
{
public:
	SocketMaker();
	~SocketMaker();
	int createSocket(char *hostname,int port);
	int sendHttpRequest(int sockfd,struct URL url);
	int closeSocket(int sockfd);
};

#endif