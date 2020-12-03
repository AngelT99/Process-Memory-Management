scheduler: scheduler.c linked_list helperFunctions
	gcc -o scheduler -Wall scheduler.c linked_list.o helperFunctions.o -g -lm

clean:
	rm -f *.o scheduler

linked_list: linked_list.h linked_list.c
	gcc -c linked_list.c -Wall -g

helperFunctions: helperFunctions.h helperFunctions.c linked_list
	gcc -c helperFunctions.c -Wall -g
