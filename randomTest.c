#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "linked_list.h"

typedef long long int LLI;

int main(int argc, char *argv[])
{
	// /* code */
	// double i = pow(2,32);
	// LLI j = 3;

	// printf("%lf\n", i/j);

	// double k = i/j;

	// double z = ceil(k);
	// printf("%lf\n", z);

	// LLI q = (LLI)z;
	// printf("%lld\n", q);


	// LLI time = 61;
	// // LLI inter = 60;
	// LLI intervals = (LLI)ceil((double)time/(double)60);
	// printf("intervals: %lld \n", intervals);


	// double a = 3.1415926;
	// printf("%.2lf\n", a);

	// LLI memory_pages = (LLI)(40/4);
	// LLI *memory;
	// memory = (LLI*)malloc(sizeof(LLI)*memory_pages);
	// for(LLI i = 0; i<memory_pages; i++){
	// 	memory[i] = 2*i;
	// }

	// for(LLI i = 0; i<memory_pages; i++){
	// 	printf("memory:%lld: %lld\n", i, memory[i]);
	// }

	// free(memory);
	// LLI *memor = (LLI*)malloc(sizeof(LLI)*memory_pages);
	// for(LLI i = 0; i<memory_pages; i++){
	// 	printf("memory:%lld: %lld\n", i, memory[i]);
	// }
 
 	LLI size = 5;
    memory_node *array_pagePointers_list = malloc(size*sizeof(memory_node));
    for(LLI i = 0; i < size; i++){
    	array_pagePointers_list[i] = NULL;
    }

    free(array_pagePointers_list);
    




	return 0;
}