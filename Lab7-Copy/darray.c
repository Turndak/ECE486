#include <stdio.h>
#include <stdlib.h>
#include "darray.h"
// #include "integer.h"

//private struct members
struct DArray{
	void **array; //an array of void pointers
	int capacity; //total number of slots
	int size; 	  //number of filled slots
	void (*display)(FILE *, void *); 
};
//creates a new element of the DArray type
DArray *newDArray(void (*display)(FILE *,void *))
{
	DArray *new = malloc(sizeof(DArray)); //allocate memory for the size of the new element
	new->array = malloc(sizeof(void *)*1);
	new->capacity = 1;
	new->size = 0;
	new->display = display;

	return new;
}

void insertDArray(DArray *a,void *v)
{
	if(a->size == a->capacity) //if array is full
	{
		a->capacity *= 2; //double the capacity
		a->array = realloc(a->array, a->capacity * sizeof(void *)); //reaalocate memory for the new array size
	}
	a->array[a->size++] = v;//if array is not full, then store v in the next slot in the array
}

void *removeDArray(DArray *a)
{
	//capacity will shrink by half when size is 25% of the capacity

	void *remVal = a->array[a->size-1];
	if(a->size == 0)//error checking
	{
		fprintf(stderr, "Attempting to remove from an empty array\n");
		exit(-1); //exits the progrom
	}

	a->array[a->size-1] = NULL;//set end value to NULL
	--a->size;//decrease size 

	double size = a->size;
	double capacity = a->capacity;

	//handle the resize
	if(size < capacity/4.0 && capacity > 2)
	{
		a->capacity /= 2; //cut capacity by a factor of 2
		a->array = realloc(a->array, a->capacity * sizeof(void *)); //reaalocate memory for the new array size
	}

	return remVal;
}

void *getDArray(DArray *a,int index)
{
	return a->array[index];
}

void setDArray(DArray *a,int index,void *value)
{
	if(index == a->size) 
	{
		return insertDArray(a, value);
	}

	a->array[index] = value;
}

int sizeDArray(DArray *a)
{
	return a->size;
}

/*int capacityDArray(DArray *a)
{
	return a->capacity;
}*/

void displayDArray(FILE *fp,DArray *a)
{	//while size is greater than 0

	int index = 0;
	while(index < a->size)
	{
		if(a->array[index] != NULL) //if size is greater than index + 1
		{
			a->display(fp,a->array[index]);
			fprintf(fp, "\n");
		}

		index++;
	}
/*    int i;
    
    for (i = 0; i < a->size; ++i)
    {
        if (a->array[i] != NULL)
        {
            a->display(fp,a->array[i]);
            fprintf(fp, "\n");
        }
        
    }
*/}