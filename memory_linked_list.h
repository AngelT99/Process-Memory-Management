#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include <ctype.h>

#define INUSED = 1;
#define NOT_INUSED = 0
#define EMPTY = -1

typedef long long int LLI;


typedef struct memory memory_t;
// This is a shared structure between processInMemory(layer2), pagePointers(layer3), pages(layer4).
struct memory{
		memory_t* next;
		memory_t* prev;
		// processInMemory: Layer 2 (from top to bottom, layer 1 is Processes)
		memory_list* pagePointers;
		node_t* process;
		// pagePointers: Layer 3
		memory_t* page;
		// Page: Layer 4
		int inUsed;
		LLI indexStart;
		LLI len;
		
};

typedef struct{
		memory_t* head;
		memory_t* tail;
		// processInMemory:
		LLI numProcessInMemory = EMPTY;
		node_t* runningProcess;
		// pagePointers:
		LLI numPages = EMPTY;
		// pages:
		LLI emptyPages;
		LLI totalPages;

}memory_list;



void free_list_memory(memory_list* list);
// create an empty processInMemory linked list
memory_list* make_new_processInMemory_list();
// initialise a processInMemory node
memory_t* make_new_processInMemory_node(memory_list* pagePointers, node_t* process);
// create an empty pagePointers linked list
memory_list* make_new_pagePointers_list();
// initialise a pagePointer node
memory_t* make_new_pagePointer_node(memory_t* page);
// create an empty pages linked list
memory_list* make_new_pages_list(LLI memorySize);
// initialise a page node
memory_t* make_new_page_node(int inUsed, LLI indexStart, LLI len);
// insert a node into the tail of the list
memory_list* enqueue_memory(memory_list* list, memory_t* new);
// remove a node from the tail of the list
memory_list* dequeue_memory(memory_list* list);

void print_list_pages(memory_list* list);
void print_node_pages(memory_t* node);
