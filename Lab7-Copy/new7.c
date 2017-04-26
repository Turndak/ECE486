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
		DcacheSets[i] = newDArray(displayC);//creates an array for each cache set
		QcacheSets[i] = newQueue(displayC);//creates a queue for each cache set
		for (j = 0; j < Associativity(c); ++j)
		{
			insertDArray(DcacheSets[i], NULL);//initializes the array with NULL
		}
	}

	int hits = 0;
	for (i = 0; i < num_of_addresses; ++i)//loops through each address
	{
		int found = 0;
		memLoc *Op = getDArray(memOps, i);//get the current operation from the darray
		
		//initializes a new cache block
		cacheBlock *cblock = newCBlock(getMMOp(Op),getMMBlk(Op),getOpNum(Op),1, tagBits(c));
		//sets the tag for the cache block
		setTag(cblock, getAddress(Op), tagBits(c), addressLines(m));
		//If there is an item in the array check the array for a hit
		if (isNotEmpty(DcacheSets[getCSet(Op)]))  
		{
			for (j = 0; j < filledSlots(DcacheSets[getCSet(Op)]); ++j)
			{	
				if (getMMBlk(Op) == getData(getDArray(DcacheSets[getCSet(Op)],j)))
				{ 
					//if the address is found in the correct set then sets it as hit
					updateAge(getDArray(DcacheSets[getCSet(Op)],j),Op->opNum);
					Op->hit_miss="hit";
					found = 1;
					hits++;
					break;
				}
			}
		}
		if (found != 1)   //If there were not hits 
		{	//Set the next empty slot if the memory block isn't full
			if (!(isFull(DcacheSets[getCSet(Op)])))  
			{
				if (replacement == 'F')
				{
					//enqueue the operation for use with FIFO
					enqueue(QcacheSets[getCSet(Op)], cblock);
				}
				//Set next empty slot in CacheSet for DArray
				setDArray(DcacheSets[getCSet(Op)], filledSlots(DcacheSets[getCSet(Op)]), cblock);
			}
			//If cache set is full replace something  
			//This conditional is the only place where the replacement policy matters
			else     
			{
				if (replacement == 'F')
				{	//dequeues the first operation preformed
					cacheBlock *removed = dequeue(QcacheSets[getCSet(Op)]);
					for (j = 0; j < Associativity(c); ++j)
					{	//break out if there is a hit
						if (getData(removed) == getData(getDArray(DcacheSets[getCSet(Op)],j)))
						{
							break;
						}
					}
					//if not hit, replace that position in the darray 
					setDArray(DcacheSets[getCSet(Op)], j, cblock);
					//and enqueue the new operation to track it
					enqueue(QcacheSets[getCSet(Op)], cblock);
				}
				//LIU replacement policy
				else if(replacement == 'L')
				{	//gets the age of the current cache block(it's OpNum)
					int smallest = getAge(cblock), index = 0;
					//loops through set
					for (j = 0; j < Associativity(c); ++j)
					{	//gets the age of the current operation
						int age = getAge(getDArray(DcacheSets[getCSet(Op)],j));
						//if the cache blocks age is greater than the new operation
						//then reset the cache blocks age and update index for the setDArray
						if (smallest > age)
						{
							smallest = age;
							index = j;
						}
					}
					//alwas inserts something if any conditionals above were met
					//then a new memory  block was inserted
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
				//initializes empty slots in the set
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
			//sets the cache blocks with correct data
			cacheBlock *block = getDArray(DcacheSets[i],j);
			setCBNum(block, blockNumber);
			blockNumber++;
		}
	}

/*********************************Print tables*********************************/

	displayDArray(stdout,memOps);//displays the memory table

	//caclculate the highest possible and actual hit rates
	float hphr = ((float)count/(float)num_of_addresses)*100;
	float ahr = ((float)hits/(float)num_of_addresses)*100;

	printf("\nHighest possible hit rate %d/%d = %.02f%%\n", count, num_of_addresses, hphr);
	printf("Actual hit rate = %d/%d = %.02f%%\n\n", hits, num_of_addresses, ahr);

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
//returns which slots are filled in the darray
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
