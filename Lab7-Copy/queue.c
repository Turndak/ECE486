///////////////////////////////////////////
///										///
///		queue.c for CS201 assignment 0	///
///										///
///		Dakota Turner January 2017		///
///										///
///	 LILO / FIFO Queue implementation	///
///										///
///////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include "sll.h"

queue *newQueue(void (*d)(FILE *,void *))   //constructor
{
	queue *items = malloc(sizeof(queue));
	if(items == 0)
	{
		fprintf(stderr, "out of memory.");
		exit(-1);
	}

	items->list = newSLL(d);
	return items;
}

void enqueue(queue *items,void *value)      //stores a generic value
{
	insertSLL(items->list, sizeSLL(items->list), value);
}

void *dequeue(queue *items)                 //returns a generic value
{
	return removeSLL(items->list, 0);
}

void *peekQueue(queue *items)               //returns a generic value
{
	return getSLL(items->list, 0);
}

int sizeQueue(queue *items)
{
	return sizeSLL(items->list);
}

void displayQueue(FILE *fp,queue *items)
{
	displaySLL(fp, items->list);
}
