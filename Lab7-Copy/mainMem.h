/*
 * Name: Dakota Turner
 * CWID: 1131453
 * Class: ECE486 Lab7 Memory simulation(mainMem header)
 * Date: April 27, 2017
 */

//this file is the interface to the mainMem file

#ifndef MAINMEM_INCLUDED_
#define MAINMEM_INCLUDED_
//////////////Main Mem////////////////
typedef struct mainMem
{
	int size;
	int lines;
	
}mainMem;

extern mainMem *newMem(int size);
extern int addressLines(mainMem *m);

/////////////////////////////////////

///////////Memory locations////////

typedef struct memLoc
{
	int opNum;
	char operation;
	int address;
	int mmblk;
	int cmset;
	int cmblk_top;
	int cmblk_bottom;
	char *hit_miss;
}memLoc;

extern memLoc *newMemLoc(int opNum, char opType, int addr, int blksize, int cachesets, int N);
extern memLoc *getMemLoc(memLoc *m);
extern void displayMem(FILE *, void *);
extern int getMMBlk(memLoc *m);
extern int getCSet(memLoc *m);
extern int getAddress(memLoc *m);
extern int getOpNum(memLoc *m);
extern char getMMOp(memLoc *m);

///////////////////////////////////

////////////Helper functions///////
extern int numberofbits(int num);
extern long DectoBin(int address);
extern char* itoa(int num, char* str, int base);
////////////////////////////////////
#endif