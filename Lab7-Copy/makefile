FLAGS = -Wall -Wextra -std=c99

lab7: new7.o cache.o mainMem.o darray.o queue.o sll.o
	gcc $(FLAGS) new7.o cache.o mainMem.o darray.o queue.o sll.o -o lab7

new7.o: new7.c cache.c mainMem.c darray.c queue.c sll.c
	gcc $(FLAGS) -c new7.c
darray.o: darray.c darray.h
	gcc $(FLAGS) -c darray.c
sll.o: sll.c sll.h
	gcc $(FLAGS) -c sll.c
queue.o: queue.c queue.h sll.h
	gcc $(FLAGS) -c queue.c
cache.o: cache.c cache.h mainMem.h
	gcc $(FLAGS) -c cache.c
mainMem.o: mainMem.c mainMem.h 
	gcc $(FLAGS) -c mainMem.c

clean: 
	rm -f lab7.o new7.o darray.o sll.o queue.o cache.o mainMem.o lab7 

test: ./lab7

	./lab7 < inputL