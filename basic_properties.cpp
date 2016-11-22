//
// Created by Jacob Timber on 11/14/16.
//
#include "basic_properties.h"

extern char* basic_url = "10.108.87.34";
extern unsigned char *dataHash = (unsigned char *)malloc(LENGTH*17*sizeof(unsigned char));//布隆过滤器的hash表
extern int counter = 0;
extern string result_file = "Result.txt";
