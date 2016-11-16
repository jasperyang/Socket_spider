#include<stdio.h>
#include<stdlib.h> 
#include "hash.hpp"

#ifndef LENGTH
#define LENGTH 44045
#endif
void bf_dataHash(const void *key,unsigned char *dataHash);
int bf_dataCheck(const void *key, unsigned char *dataHash);

