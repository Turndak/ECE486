///////////////////////////////////////////
///										///
///		sll.c for CS201 assignment 0	///
///										///
///		Dakota Turner January 2017		///
///										///
///	 Creates, inserts & removes nodes   ///
///	 	from a singly linked list		///
///	 Returns value at index and size 	///
///		of list							///
///										///
///////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include "sll.h"
//#include "integer.h"

sll *newSLL(void (*d)(FILE *, void *)) //d is the display function
{
	sll *items = malloc(sizeof(sll));
    if (items == 0)
    	{
    		fprintf(stderr,"out of memory");
        	exit(-1);
        }
    items->head = 0;
    items->tail = 0;
    items->size = 0;
    items->display = d;
    return items;
}

void insertSLL(sll *items, int index, void *value) //inserts a new node into the linked list
{
	sllnode *new_node = malloc(sizeof(sllnode));
	new_node -> value = value;
	new_node->next = NULL;
	//new_node -> next= items->head;
	//items->head=new_node;

	if (items->head == NULL || items->tail == NULL) //only inserting at the head
	{
		items->head = new_node;
		items->tail = new_node;

	}
	else if (items->head != NULL && index == 0)
	{
		new_node->next = items->head;
		items->head = new_node;
	}
	else if(items->size == index)
	{
		new_node->next = NULL;
		items->tail->next = new_node;
		items->tail = new_node;	
	}
	else
	{
		int currpos = 0;
		sllnode *curr_node = items->head;
		for(; currpos<index-1 && items->head != NULL; ++currpos)
		{
			curr_node = curr_node->next;
		}
			if(curr_node->next != NULL)
			{
				new_node->next = curr_node->next;
				curr_node->next = new_node;
			}
			else
			{
				curr_node->next = new_node;
			}
	}
	items->size++;
}

void *removeSLL(sll *items, int index)	//remove item at desired index
{
	sllnode *tmp = items->head;		//store head in a temp

	if(items->head != NULL && index == 0)	//to remove head
	{
		items->head = items->head->next;
		tmp->next = NULL;

		items->size--;
		return tmp->value;
	}
	else
	{
		sllnode *remove_node;
		for(int currpos = 0; currpos < index-1; ++currpos)
		{
			tmp = tmp->next;
		}
		remove_node = tmp->next;
		tmp->next = remove_node->next;
		remove_node->next = NULL;

		return remove_node->value;
	}

	items->size--;
}

void unionSLL(sll *recipient, sll *donor)
{
	if (donor->head == NULL && recipient->head == NULL)
	{
		return;
	}
	else if(donor->head == NULL)
	{
		return;
	}
	else if (recipient->head == NULL && donor->head != NULL)
	{
		recipient->head = donor->head;
	}
	else
	{
		recipient->tail->next = donor->head;
	}
									//sets tail pointer of recipient
									//to head of donor
	recipient->tail = donor->tail;	//this allows union in constatnt time
	recipient->size += donor->size;
	donor->head = donor->tail = 0;
	donor->size = 0;
}

void *getSLL(sll *items, int index) //returns the value at an index
{
	sllnode *tmp = items->head;
	int posi = 0;
	if(index == 0)
	{
		return tmp->value;
	}
	else{
		while(tmp != NULL && posi < index-1)
		{
			tmp = tmp->next;
			posi++;
		}
		return tmp->next->value;//display value at tmp
	}
}

int sizeSLL(sll *items)
{
	// sllnode *tmp = items->head;	//tmp node
	// int count = 0;
	// while (tmp != NULL)			//counts the #of items in list
	// {
	// 	++count;
	// 	tmp=tmp->next;
	// }
	// return count;
	return items->size;
}

void displaySLL(FILE *fp, sll *items)
{
	fprintf(fp,"[");
	sllnode *tmp = items->head;
	while (tmp != NULL)					//while not at the end of list
	{
		items -> display(fp, tmp->value);//display value at tmp
		if(tmp->next != NULL)			//as long as tmp's next pointer
		{								//doesn't point to null
			fprintf(fp,",");				
		}
		tmp = tmp->next;				//set tmp equal to tmp's next
	}
	fprintf(fp,"]");
}

