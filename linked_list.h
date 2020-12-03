#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include <ctype.h>

#define INUSED 1
#define NOT_INUSED 0
#define EMPTY -1

typedef long long int LLI;

// Process Scheduler =============================

typedef struct node node_t;
typedef struct memory memory_node;
typedef struct memory_lt memory_list;

struct node{
	LLI tm_arvd;
	LLI pro_id;
	LLI mem_size;
	LLI job_tm;
	LLI remain_tm;
	node_t* next;
	node_t* prev;
	memory_node* processInMemory;
};


typedef struct{
	node_t* head;
	node_t* tail;
	node_t* running;
	LLI amount;

}list_t;

// Memory Manager ================================


// This is a shared structure between processInMemory(layer2), pagePointers(layer3), pages(layer4).
struct memory{
		memory_node* next;
		memory_node* prev;
		// processInMemory: Layer 2 (from top to bottom, layer 1 is Processes)
		memory_list* pagePointers;
		node_t* process;
		// pagePointers: Layer 3
		memory_node* page;
		// Page: Layer 4
		int isUsed;
		LLI indexStart;
		LLI len;
		
};

struct memory_lt{
		memory_node* head;
		memory_node* tail;
		// processInMemory:
		LLI numProcessInMemory;
		node_t* runningProcess;
		// pagePointers:
		LLI numPages;
		// pages:
		LLI emptyPages;
		LLI totalPages;
};


typedef struct page page_t;

struct page{
	LLI indexStart;
	LLI len;
};


// Process scheduler======================================

list_t* make_empty_list();
void free_list(list_t* list);
node_t* make_new_node(LLI tm_arvd, LLI pro_id, LLI mem_size, LLI job_tm);
list_t* enqueue(list_t* list, node_t* new);
list_t* sorted_enqueue(list_t* list, node_t* new);
list_t* dequeue(list_t* list);
node_t* pop_dequeue(list_t* list, node_t* cur);
void print_list(list_t* list);
void print_node(node_t* node);



// Memory Manager=========================================

void free_list_memory(memory_list* list);

// create an empty processInMemory linked list
memory_list* make_new_processInMemory_list();
// initialise a processInMemory node
memory_node* make_new_processInMemory_node(memory_list* pagePointers, node_t* process);

// create an empty pagePointers linked list
memory_list* make_new_pagePointers_list();
// initialise a pagePointer node
memory_node* make_new_pagePointer_node(memory_node* page);

// create an empty pages linked list
memory_list* make_new_pages_list(LLI memorySize);
// initialise a page node
memory_node* make_new_page_node(int isUsed, LLI indexStart, LLI len);

// insert a node into the tail of the list
memory_list* enqueue_memory(memory_list* list, memory_node* new);
// remove a node from the tail of the list
memory_list* dequeue_memory(memory_list* list);
memory_node* pop_dequeue_memory(memory_list* list, memory_node* cur);
memory_list* push_enqueue_memory_CM(memory_list* list, memory_node* new);

void print_list_pages(memory_list* list);
void print_node_page(memory_node* cur);

void print_list_pagePointers(memory_list* list);
void print_node_pagePointer(memory_node* cur);

void print_list_processInMemory(memory_list* list);
void print_node_processInMemory(memory_node* cur);

memory_node* enqueue_pagePointer(memory_list* list, memory_node* new, memory_node* start);
//============================================
page_t* make_new_page_t(LLI indexStart, LLI len);
void print_page_t_node(page_t* page);
//============================================
// CS Shortest Job First
list_t* SJF_sorted_enqueue(list_t* list, node_t* new);