/*
 * Name: Dakota Turner
 * CWID: 1131453
 * Class: ECE486 Lab7 Memory simulation(cache)
 * Date: April 27, 2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "cache.h"
#include "mainMem.h"


/////////////////Cache Stuff/////////////////////////////
//initializing the new cache
cache *newCache(int aLines,int size, int linesize, int n, char r)
{ 
	cache *c = malloc(sizeof(cache));
	c->size = size;
	c->blockSize = linesize;
	c->cacheBlocks = size / linesize;
	c->assoc = n;
	c->cacheSets = c->cacheBlocks / c->assoc;
	c->replacement = r;
	c->offbits = numberofbits(linesize);
	c->indexbits = numberofbits(c->cacheSets);
	c->tagbits = aLines - (c->offbits + c->indexbits);
	c->totalCache = size + (c->tagbits+2) * (c->cacheBlocks/8);
	return c;
}

int offBits(cache *c)
{
	return c->offbits;
}

int indexBits(cache *c)
{
	return c->indexbits;
}

int tagBits(cache *c)
{
	return c->tagbits;
}

int totCacheSize(cache *c)
{
	return c->totalCache;
}

int CacheBlockSize(cache *c)
{
	return c->blockSize;
}

int CacheSets(cache *c)
{
	return c->cacheSets;
}
int Associativity(cache *c)
{
	return c->assoc;
}

//////////////////////////End Cache Stuff////////////////////////////////////

/////////////////cacheBlock stuff//////////////////////////
//initializing the new cache block
cacheBlock *newCBlock(char oper, int mmblk, int age, int valid, int tagSize)
{
	cacheBlock *cb = malloc(sizeof(cacheBlock));
	if (oper == 'R')
	{
		cb->dirty = 0;
	}
	else
	{
		cb->dirty = 1;
	}
	cb->blk_num = 0;
	cb->age = age;
	cb->data = mmblk;
	cb->valid = valid;
	cb->itag = 0;
	int i;
	for (i = 0; i < tagSize; ++i)
	{
		cb->ctag[i] = 'x';
	}
	return cb;
}

cacheBlock *getCBlock(cacheBlock *cb)
{
	return cb;
}

//display the cache block
void displayC(FILE *fp, void *v)
{
	cacheBlock *cb = getCBlock(v);
	if (cb->data == -1)
	{
		fprintf(fp, "\t%d \t%d \t%d \t%s \tx \t", cb->blk_num, cb->dirty, cb->valid, cb->ctag);
	}
	else
	{
		fprintf(fp, "\t%d \t%d \t%d \t%s \tmmblk # %d \t", cb->blk_num, cb->dirty, cb->valid, cb->ctag, cb->data);
	}
}

int getData(cacheBlock *cb)
{
	return cb->data;
}

int getAge(cacheBlock *cb)
{
	return cb->age;
}

void updateAge(cacheBlock *cb, int age)
{
	cb->age = age;
}

void setCBNum(cacheBlock *cb, int number)
{
	cb->blk_num = number;
}

//converts the address to binary, singles out the tag bits
void setTag(cacheBlock *cb, int address, int tagSize, int totalSize)
{
	//converting to binary
	long tempTag = DectoBin(address);
	//singling out the tag bits
	int i;
	for (i = 0; i < totalSize-tagSize; ++i)
	{
		tempTag /= 10;
	}
	cb->itag = tempTag;
	
	//converting to binary string(backwards)
	char temp[tagSize];
	for (i = 0; i < tagSize; ++i)
	{
		int remainder = tempTag % 10;
		tempTag/=10;
		if (remainder == 0)
		{
			cb->ctag[i] = '0';
			temp[i] = '0';
		}
		else
		{
			cb->ctag[i] = '1';
			temp[i] = '1';
		}
	}
	//flips the binary string the right way around
	long j = tagSize;
	for (i = 0; i < tagSize; ++i)
	{
		cb->ctag[i] = temp[j-1];
		j--;
	}
}