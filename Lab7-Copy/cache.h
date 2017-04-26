/*
 * Name: Dakota Turner
 * CWID: 1131453
 * Class: ECE486 Lab7 Memory simulation(cache header)
 * Date: April 27, 2017
 */

//interface for the cache file

#ifndef CACHE_INCLUDED_
#define CACHE_INCLUDED_

typedef struct cache
{
	int size;
	int blockSize;
	int cacheBlocks;
	int assoc;
	char replacement;
	int cacheSets;
	int offbits;
	int indexbits;
	int tagbits;
	int totalCache;
	
}cache;

typedef struct cacheBlock
{
	int data;
	int blk_num;
	int age;
	int dirty;
	int valid;
	double itag;
	char ctag[16];
}cacheBlock;

extern cache *newCache(int aLines, int size, int linesize, int assoc, char replacement);
extern int offBits(cache *c);
extern int indexBits(cache *c);
extern int tagBits(cache *c);
extern int totCacheSize(cache *c);
extern int CacheBlockSize(cache *c);
extern int CacheSets(cache *c);
extern int Associativity(cache *c);

extern cacheBlock *newCBlock(char oper, int mmblk, int age, int valid, int tagSize);
extern cacheBlock *getCBlock(cacheBlock *cb);
extern void displayC(FILE *fp, void *v);
extern int getData(cacheBlock *cb);
extern int getAge(cacheBlock *cb);
extern void updateAge(cacheBlock *cb, int age);
extern void setCBNum(cacheBlock *cb, int number);
extern void setTag(cacheBlock *cb, int address, int tagSize, int totalSize);


#endif