/*
 * Name: Dakota Turner
 * CWID: 1131453
 * Class: ECE486 Lab7 Memory simulation(main)
 * Date: April 27, 2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mainMem.h"
#include "cache.h"
#include "queue.h"
#include "sll.h"
#include "darray.h"

int isNotEmpty(DArray *array);
int isFull(DArray *array);
int filledSlots(DArray *array);
void printCacheHeading();

int main()
{	
	char cont = 'y';

	while (cont == 'y')
	{

	int mainbytes, cachebytes, blocksize, N;
	char replacement, input[20];


	printf("\nEnter the size of the main memory in bytes:");
	scanf("%d", &mainbytes);    //Main memory size is stored in mainbytes.
	//printf("%d\n", mainbytes);

	printf("Enter the size of the cache in bytes:");
	scanf("%d", &cachebytes);   //Cache size is stored in cachebytes.
	//printf("%d\n", cachebytes);

	printf("Enter the cache block/line size:");
	scanf("%d", &blocksize);    //Block size is store in blocksize.
	//printf("%d\n", blocksize);
	
	printf("\nEnter the degree of set-associativity(input n for an n-way set-associative mapping):");
	scanf("%d", &N);            //N for associativity
	//printf("%d\n", N);

	printf("Enter the replacement policy (L=LRU, F=FIFO):");
	scanf(" %c", &replacement);  //Replacement holds the replacement policy for the cache.
	
	printf("Enter the name of the input file containing the list of memory references generated\n");
	printf("by the CPU: ");
	scanf("%s", input);          //Input stores the file name for the memory references.

	printf("Replacement: %c  Input File: %s\n", replacement,input); //For error checking

	mainMem *m = newMem(mainbytes);

	printf("\nSimulator output: \n");
	printf("Total address lines required = %d \n", addressLines(m));

	cache *c = newCache(addressLines(m),cachebytes,blocksize,N,replacement);
	printf("Number of bits for offset= %d \n", offBits(c));
	printf("Number of bits for index= %d \n", indexBits(c));
	printf("Number of bits for tag= %d \n", tagBits(c));
	printf("Total cache size required= %d \n\n", totCacheSize(c));
	printf("main memory address\t mmblk#\t cmset#\t cmblk#\t hit/miss\n");
	printf("_________________________________________________________________\n");

/**********************************Read in Operations*********************************/
	FILE *fp;
	fp = fopen(input,"r");

	int num_of_addresses=0,i,j, count=0;
	fscanf(fp,"%d", &num_of_addresses);

	int address;  char operation;
	DArray *memOps = newDArray(displayMem);

	int mmblk_list[num_of_addresses];

	for (i = 0; i < num_of_addresses; ++i)
	{
		fscanf(fp," %c %d", &operation, &address);
		memLoc *loc = newMemLoc(i, operation,address,CacheBlockSize(c), CacheSets(c), Associativity(c));
		mmblk_list[i] = getMMBlk(loc);
		for (j=i-1; j >= 0; --j)
		{	
			if (mmblk_list[i] == mmblk_list[j])
			{
				count++;
				// loc->hit_miss = "hit";
				break;
			}
		}
		insertDArray(memOps,loc);
	}
	fclose(fp);

/************************************Creating cache***************************************/

	DArray *DcacheSets[CacheSets(c)];
	queue *QcacheSets[CacheSets(c)];

	for (i = 0; i < CacheSets(c); ++i)
	{
		DcacheSets[i] = newDArray(displayC);
		QcacheSets[i] = newQueue(displayC);
		for (j = 0; j < Associativity(c); ++j)
		{
			insertDArray(DcacheSets[i], NULL);
		}
	}

	int hits = 0;
	for (i = 0; i < num_of_addresses; ++i)
	{
		int found = 0;
		memLoc *Op = getDArray(memOps, i);
		
		cacheBlock *cblock = newCBlock(getMMOp(Op),getMMBlk(Op),getOpNum(Op),1, tagBits(c));
		setTag(cblock, getAddress(Op), tagBits(c), addressLines(m));
		if (isNotEmpty(DcacheSets[getCSet(Op)]))  //If there is an item in the array check the array for a hit
		{
			for (j = 0; j < filledSlots(DcacheSets[getCSet(Op)]); ++j)
			{
				if (getMMBlk(Op) == getData(getDArray(DcacheSets[getCSet(Op)],j)))
				{ 
					updateAge(getDArray(DcacheSets[getCSet(Op)],j),Op->opNum);
					Op->hit_miss="hit";
					found = 1;
					hits++;
					break;
				}
			}
		}
		if (found != 1)   //If there were not hits 
		{
			if (!(isFull(DcacheSets[getCSet(Op)])))  //Set the next empty slot if the memory block isn't full
			{
				if (replacement == 'F')
				{
					enqueue(QcacheSets[getCSet(Op)], cblock);
				}
				//Set next empty slot in CacheSet for DArray
				setDArray(DcacheSets[getCSet(Op)], filledSlots(DcacheSets[getCSet(Op)]), cblock);
			}
			else      //If cache set is full replace something  //This conditional is the only place where the replacement policy matters
			{
				if (replacement == 'F')
				{
					cacheBlock *removed = dequeue(QcacheSets[getCSet(Op)]);
					for (j = 0; j < Associativity(c); ++j)
					{
						if (getData(removed) == getData(getDArray(DcacheSets[getCSet(Op)],j)))
						{
							break;
						}
					}
					setDArray(DcacheSets[getCSet(Op)], j, cblock);
					enqueue(QcacheSets[getCSet(Op)], cblock);
				}
				else if(replacement == 'L')
				{
					int smallest = getAge(cblock), index = 0;
					for (j = 0; j < Associativity(c); ++j)
					{
						int age = getAge(getDArray(DcacheSets[getCSet(Op)],j));
						if (smallest > age)
						{
							smallest = age;
							index = j;
						}
					}
					setDArray(DcacheSets[getCSet(Op)], index, cblock);
				}
			}
		}
	}

	//Fills in untouched slots with default values
	for (i = 0; i < CacheSets(c); ++i)
	{
		for (j = 0; j < Associativity(c); ++j)
		{
			if (getDArray(DcacheSets[i],j)==NULL)
			{
				cacheBlock *emptySlot = newCBlock('R',-1, -1,0, tagBits(c));
				setDArray(DcacheSets[i], j, emptySlot);
			}
		}
	}
	//Sets cacheblock numbers
	int blockNumber = 0;
	for (i = 0; i < CacheSets(c); ++i)
	{
		for (j = 0; j < Associativity(c); ++j)
		{
			cacheBlock *block = getDArray(DcacheSets[i],j);
			setCBNum(block, blockNumber);
			blockNumber++;
		}
	}

/*********************************Print tables*********************************/

	displayDArray(stdout,memOps);//displays the memory table
	printf("\nHighest possible hit rate %d/%d = %.02f%%\n", count, num_of_addresses, ((float)count/(float)num_of_addresses)*100);
	printf("Actual hit rate = %d/%d = %.02f%%\n\n", hits, num_of_addresses, ((float)hits/(float)num_of_addresses)*100);
	printCacheHeading();
	printf("_________________________________________________________________\n");
	for (i = 0; i < CacheSets(c); ++i)
	{
		displayDArray(stdout,DcacheSets[i]);
	}

	free(c);//free cache
	free(m);//free mem
	free(*DcacheSets);
	free(*QcacheSets);

	printf("Continue? (y = yes, n = no): \n");
	scanf("\n%c", &cont);
	}

	return 0;
}
//checks for an item in the array (lets us know if something has been written yet or not)
int isNotEmpty(DArray *array)
{
	int i;
	for (i = 0; i < sizeDArray(array); ++i)
	{
		if (getDArray(array, i)!=NULL)
		{
			return 1;
		}
	}
	return 0;
}
//checks if the set is full
int isFull(DArray *array)
{
	int i;
	for (i = 0; i < sizeDArray(array); ++i)
	{
		if (getDArray(array, i)==NULL)
		{
			return 0;
		}
	}
	return 1;
}

int filledSlots(DArray *array)
{
	int i;
	for (i = 0; i < sizeDArray(array); ++i)
	{
		if (getDArray(array,i)==NULL)
		{
			return i;
		}
	}
	return sizeDArray(array);
}

void printCacheHeading()
{
	printf("Cache blk# \tDirty \tValid \ttag \tData\n");
}