#ifndef _DOWNLOADER_H
#define _DOWNLOADER_H

#ifdef __cpluscplus
extern "C" {
#endif

#include <iostream>
#include "basic_properties.h"
#include "parser.h"

using namespace std;

class Downloader {
public:
	Downloader();

	~Downloader();

	int recvHttpRespond(int sockfd, char *ch);

	int recvHttpRespond(int sockfd, char *ch, string filename, parser *par);
};

#ifdef __cpluscplus
};
#endif
#endif