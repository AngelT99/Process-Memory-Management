#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>


#define MAX_FILENAME_LEN 1000
#define MAX_STRING_LEN 10
#define COMPLETED 1
#define NOT_COMPLETED 0
#define VIRTUAL_MEMORY 1
#define SWAPPING 0
#define ADD_NEWPROCESS_TRUE 1
#define ADD_NEWPROCESS_FALSE 0
#define SJF_TRUE 1
#define SJF_FALSE 0
#define CM_TRUE 1
#define CM_FALSE 0
#define USEMEMORY 1	
#define NOT_USEMEMORY 0
#define FLAG_TRUE 1
#define FLAG_FALSE 0
#define QUANTUM_NOT_NEEDED -1



typedef long long int LLI;

void checkFinish(list_t* list, LLI *time, LLI *processes_total, LLI *throughput_min, LLI *throughput_max, LLI *cur_throughput, LLI *intervals, LLI *cur_interval, LLI *turnaround_total, double *timeoverhead_max, double *timeoverhead_total);
LLI startRunning(list_t* list, LLI *time, LLI quantum);
void chooseWhichProcessToRun(list_t* list);
memory_node* lookForEmptyPages(memory_list* pages);
LLI checkifEnoughEmptyPages(memory_list* pages, node_t* process);
void evictPage(memory_list* pages, memory_node* cur);
memory_node* loadPageIntoMemory(memory_list* pages, memory_node* cur, LLI len_req);
void loadAllPagesIntoMemory(memory_list* pagePointers, memory_list* pages, LLI pages_req);
void evictPagesOfAProcess(memory_list* pagePointers, memory_list* pages, LLI pagesToEvicted);
void evictPartOfthePages(memory_list* pages, memory_node* cur, LLI len_req);
void loadThisProcessIntoMemory(memory_list* processInMemory_list, memory_list* pages, list_t* queue, LLI* time, int cm_orNot);
void evictLeastRecentProcess(memory_list* processInMemory, memory_list* pages, list_t* queue, LLI* time, int completedOrNot, int virtualMemoryOrNot, page_t *array_pagePointers_list, LLI size, LLI *indexOfArray, LLI pages_req_evict, int cm_orNot);
void printPageNumber(memory_list* pagePointers, LLI numPage_req);
void convertPageNumberToString(memory_node* page);
void ifFinishEvictedPages(list_t* list, memory_list* processInMemory, memory_list* pages, LLI* time, int virtualMemoryOrNot, int cm_orNot);
void storePagePointersIntoArray(page_t *array_pagePointers_list, LLI size, memory_list* pagePointers, LLI *indexOfArray, LLI numPages_req);
int comparePages(const void *a, const void *b);
void sortPageNumberArray(page_t *array_pagePointers_list, LLI size);
void printPageNumberInOrder(page_t *array_pagePointers_list, LLI size);
void loadThisProcessIntoMemory_virtualMemory(memory_list* processInMemory_list, memory_list* pages, list_t* queue, LLI* time, int virtualMemoryOrNot, int addNewProcessInMemoryOrNot, LLI pages_req_evict, LLI pages_req_load);
LLI max_compare(LLI a, LLI b);
LLI min_compare(LLI a, LLI b);
