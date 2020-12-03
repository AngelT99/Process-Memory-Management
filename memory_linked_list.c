#include "memory_linked_list.c"


// free the linked list
void free_list_memory(memory_list* list){
	memory_t *cur, *pre;
	cur = list -> head;
	while(cur != NULL){
		pre = cur;
		cur = cur -> next;
		free(pre);
	}
	free(list);
};


// create an empty processInMemory linked list
memory_list* make_new_processInMemory_list(){
	memory_list *list;
	list = (memory_list*)malloc(sizeof(memory_list));
	assert(list != NULL);
	list -> head = NULL;
	list -> tail = NULL;
	list -> numProcessInMemory = 0;
	list -> runningProcess = NULL;
	return list;
};

// initialise a processInMemory node
memory_t* make_new_processInMemory_node(memory_list* pagePointers, node_t* process){
	memory_t *new;
	new = (memory_t*)malloc(sizeof(*new));
	assert(new != NULL);
	new -> pagePointers = pagePointers;
	new -> process = process;
	return new;
};



// create an empty pagePointers linked list
memory_list* make_new_pagePointers_list(){
	memory_list *list;
	list = (memory_list*)malloc(sizeof(memory_list));
	assert(list != NULL);
	list -> head = NULL;
	list -> tail = NULL;
	list -> numPages = 0;
	return list;
};

// initialise a pagePointer node
memory_t* make_new_pagePointer_node(memory_t* page){
	memory_t *new;
	new = (memory_t*)malloc(sizeof(*new));
	assert(new != NULL);
	new -> page = page;
	return new;
};



// create an empty pages linked list
memory_list* make_new_pages_list(LLI memorySize){
	memory_list *list;
	list = (memory_list*)malloc(sizeof(memory_list));
	assert(list != NULL);
	list -> head = NULL;
	list -> tail = NULL;
	list -> emptyPages = (LLI)memorySize/4;
	list -> totalPages = 0; // TODO
	return list;
};

// initialise a page node
memory_t* make_new_page_node(int inUsed, LLI indexStart, LLI len){
	memory_t *new;
	new = (memory_t*)malloc(sizeof(*new));
	assert(new != NULL);
	new -> inUsed = inUsed;
	new -> indexStart = indexStart;
	new -> len = len;
	return new;
};





// insert a node into the tail of the list
memory_list* enqueue_memory(memory_list* list, memory_t* new){
	if(list -> tail == NULL){
		list -> tail = list -> head = new;
	}else{
		new -> prev = list -> tail;
		new -> next = NULL;
		list -> tail -> next = new;
		list -> tail = new;
	}
	// keep track of num of processInMemory
	if(list -> numProcessInMemory != EMPTY){
		(list -> numProcessInMemory)++;
	}
	// keep track of num of pagePointers
	if(list -> numPages != EMPTY){
		(list -> numPages)++;
	}
	return list;
}


// remove a node from the tail of the list
memory_list* dequeue_memory(memory_list* list){
	if(list -> head == NULL){
		return list;
	}
	memory_t *cur = list -> head;

	// keep track of num of processInMemory
	if(list -> numProcessInMemory != EMPTY){
		(list -> numProcessInMemory)--;
	}
	// keep track of num of pagePointers
	if(list -> numPages != EMPTY){
		(list -> numPages)--;
	}

	if(list -> head == list -> tail){
		free(cur);
		list -> head = NULL;
		list -> tail = NULL;
		return list;
	}
	list -> head -> next -> prev = NULL;
	list -> head = list -> head -> next;
	free(cur);
	return list;
}


void print_list_pages(memory_list* list){
	memory_t* cur = list -> head;
	printf("emptyPages: %lld, totalPages: %lld \n", list->emptyPages, list->totalPages);
	int i = 0;
	while(cur != NULL){
		printf("node%d: %lld %lld %lld\n", i, cur->inUsed, cur->indexStart, cur->len);
		cur = cur-> next;
		i++;
	}
}

void print_node_pages(memory_t* node){
	if(node != NULL){
		printf("node: %lld %lld %lld\n", i, cur->inUsed, cur->indexStart, cur->len);
	}
}


