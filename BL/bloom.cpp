#include"bloom.hpp"
#include<string.h>

int A[17] = {2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59};

void bf_dataHash(const void *key,unsigned char *dataHash)
{
	const char *data = (const char *)key;
	for(int i = 0; i <= 16; i++)
	{
		unsigned int resultHash = MurmurHash(data,strlen(data),A[i]);
		*(dataHash + (i*LENGTH) + ((resultHash%(LENGTH<<3))>>3)) |= (1<<(resultHash%(LENGTH<<3)&0x7));
	}
	return ;
 }
 
 
int bf_dataCheck(const void *key, unsigned char *dataHash)
{
	const char *data = (const char *)key;
	for(int i = 0; i <= 16; i++)
	{
		unsigned int resultHash = MurmurHash(data,strlen(data),A[i]);
		if((*(dataHash + (i*LENGTH) + ((resultHash%(LENGTH<<3))>>3)) & (1<<(resultHash%(LENGTH<<3)&0x7)))==0)
		return 0;//wrong
	}
	return 1;//right 
}

