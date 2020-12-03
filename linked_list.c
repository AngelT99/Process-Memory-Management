#include "linked_list.h"

//=====================================================================
page_t* make_new_page_t(LLI indexStart, LLI len){
	page_t *new;
	new = (page_t*)malloc(sizeof(*new));
	assert(new != NULL);
	new -> indexStart = indexStart;
	new -> len = len;
	return new;
}

void print_page_t_node(page_t* page){
	if(page != NULL){
		printf("indexStart: %lld, len: %lld\n", page->indexStart, page->len);
	}
	
}
//=====================================================================
// create an empty linked list
list_t* make_empty_list(){
	list_t *list;
	list = (list_t*)malloc(sizeof(list_t));
	assert(list != NULL);
	list -> head = NULL;
	list -> tail = NULL;
	list -> running = NULL;
	list -> amount = 0;
	return list;
};
//=====================================================================
// free the linked list
void free_list(list_t* list){
	node_t *cur, *pre;
	cur = list -> head;
	while(cur != NULL){
		pre = cur;
		cur = cur -> next;
		free(pre);
	}
	free(list);
};
//=====================================================================
// initialise a node
node_t* make_new_node(LLI tm_arvd, LLI pro_id, LLI mem_size, LLI job_tm){
	node_t *new;
	new = (node_t*)malloc(sizeof(*new));
	assert(new != NULL);
	new -> tm_arvd = tm_arvd;
	new -> pro_id = pro_id;
	new -> mem_size = mem_size;
	new -> job_tm = job_tm;
	new -> remain_tm = job_tm;
	new -> next = NULL;
	new -> prev = NULL;
	new -> processInMemory = NULL;
	return new;
};

//=====================================================================
// insert a node into the tail of the list
list_t* enqueue(list_t* list, node_t* new){
	if(list -> tail == NULL){
		list -> tail = list -> head = new;
	}else{
		new -> prev = list -> tail;
		new -> next = NULL;
		list -> tail -> next = new;
		list -> tail = new;
	}
	list -> amount ++;
	return list;
}

//=====================================================================
// insert a node into the queue in increasing order
list_t* sorted_enqueue(list_t* list, node_t* new){
	if(list -> tail == NULL){
		list -> tail = list -> head = new;
	}else{
		node_t* cur = list -> tail;
		// if there is only one node in the list
			if(list -> amount == 1){
				if(cur->tm_arvd == new->tm_arvd && cur->pro_id > new->pro_id){
					list -> head = new;
					new -> next = cur;
					cur -> prev = new;
					list -> amount++;
				}else{
					list = enqueue(list, new);
				}
				return list;
			}

		while(cur->tm_arvd == new->tm_arvd && cur->pro_id > new->pro_id){
			cur = cur->prev;
		}

		if(cur->next == NULL){
			list = enqueue(list, new);
			return list;
		}else{
			new -> next = cur -> next;
			cur -> next -> prev = new;
			new -> prev = cur;
			cur -> next = new;
		}
	}
	list -> amount ++;
	return list;
}

//=====================================================================
// insert a node into the sorted queue based on the job-time orders (accending, smallest job-time at the head, largest job-time at the tail)
list_t* SJF_sorted_enqueue(list_t* list, node_t* new){

	if(list -> head == NULL){
		list = enqueue(list, new);
		return list;
	}
	node_t* cur = list -> head;
	if(cur->job_tm > new->job_tm){
		cur->prev = new;
		new->next = cur;
		list->head = new;
		list -> amount++;
		return list;
	}
	while(cur != NULL){
		if(cur->next == NULL){
			list = enqueue(list, new);
			return list;
		}
		if(cur->next->job_tm > new->job_tm){
			cur->next->prev = new;
			new->next = cur->next;
			new->prev = cur;
			cur->next = new;
			list -> amount ++;
			return list;
		}
		cur = cur->next;
	}
	// list->amount++;
	return list;

}

//=====================================================================
// remove a node from the tail of the list
list_t* dequeue(list_t* list){
	if(list -> head == NULL){
		return list;
	}
	node_t *cur = list -> head;
	list -> amount --;
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

//=====================================================================
node_t* pop_dequeue(list_t* list, node_t* cur){
	if(cur == NULL){
		return cur;
	}
	if(cur -> next == NULL){
		if(cur -> prev == NULL){
			list -> head = NULL;
			list -> tail = NULL;
		}
		else{
			list -> tail = cur -> prev;
			cur -> prev -> next = NULL;
		}
	}
	else if(cur -> prev == NULL){
		list -> head = cur -> next;
		cur -> next -> prev = NULL;
	}else{
		cur -> next -> prev = cur -> prev;
		cur -> prev -> next = cur -> next;
	}
	cur -> prev = NULL;
	// cur -> next = NULL;
	(list -> amount)--;
	return cur;
}

//=====================================================================
void print_list(list_t* list){
	node_t* cur = list -> head;
	int i = 0;
	while(cur != NULL){
		printf("node%d: %lld %lld %lld %lld\n", i, cur->tm_arvd, cur->pro_id, cur->mem_size, cur->job_tm);
		cur = cur-> next;
		i++;
	}
}

//=====================================================================
void print_node(node_t* node){
	if(node != NULL){
		printf("node: %lld, %lld, %lld, %lld, %lld\n", node->tm_arvd, node->pro_id, node->mem_size, node->job_tm, node->remain_tm);
	}
}


// Memory Manager===================================================================
// =================================================================================
// free the linked list
void free_list_memory(memory_list* list){
	memory_node *cur, *pre;
	cur = list -> head;
	while(cur != NULL){
		pre = cur;
		cur = cur -> next;
		free(pre);
	}
	free(list);
};

//=====================================================================
// create an empty processInMemory linked list
memory_list* make_new_processInMemory_list(){
	memory_list *list;
	list = (memory_list*)malloc(sizeof(memory_list));
	assert(list != NULL);
	list -> head = NULL;
	list -> tail = NULL;
	list -> numProcessInMemory = 0;
	list -> runningProcess = NULL;

	list -> numPages = EMPTY;
	return list;
};

//=====================================================================
// initialise a processInMemory node
memory_node* make_new_processInMemory_node(memory_list* pagePointers, node_t* process){
	memory_node *new;
	new = (memory_node*)malloc(sizeof(*new));
	assert(new != NULL);
	new -> pagePointers = pagePointers;
	new -> process = process;
	new -> prev = NULL;
	new -> next = NULL;
	return new;
};

//=====================================================================
// create an empty pagePointers linked list
memory_list* make_new_pagePointers_list(){
	memory_list *list;
	list = (memory_list*)malloc(sizeof(memory_list));
	assert(list != NULL);
	list -> head = NULL;
	list -> tail = NULL;
	list -> numPages = 0;

	list -> numProcessInMemory = EMPTY;
	return list;
};

//=====================================================================
// initialise a pagePointer node
memory_node* make_new_pagePointer_node(memory_node* page){
	memory_node *new;
	new = (memory_node*)malloc(sizeof(*new));
	assert(new != NULL);
	new -> page = page;
	new -> prev = NULL;
	new -> next = NULL;
	return new;
};

//=====================================================================
// create an empty pages linked list
memory_list* make_new_pages_list(LLI memorySize){
	memory_list *list;
	list = (memory_list*)malloc(sizeof(memory_list));
	assert(list != NULL);
	list -> head = NULL;
	list -> tail = NULL;
	list -> emptyPages = (LLI)memorySize/4;
	list -> totalPages = (LLI)memorySize/4; // TODO
	list -> numPages = EMPTY;
	list -> numProcessInMemory = EMPTY;
	memory_node* first_empty_node = make_new_page_node(NOT_INUSED, 0, (LLI)memorySize/4);
	list = enqueue_memory(list, first_empty_node);
	return list;
};

//=====================================================================
// initialise a page node
memory_node* make_new_page_node(int isUsed, LLI indexStart, LLI len){
	memory_node *new;
	new = (memory_node*)malloc(sizeof(*new));
	assert(new != NULL);
	new -> isUsed = isUsed;
	new -> indexStart = indexStart;
	new -> len = len;
	new -> prev = NULL;
	new -> next = NULL;
	return new;
};

//=====================================================================
// insert a node into the tail of the list
memory_list* enqueue_memory(memory_list* list, memory_node* new){
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

	return list;
}

//=====================================================================
// insert a node into the head of the list
memory_list* push_enqueue_memory_CM(memory_list* list, memory_node* new){
	if(list -> head == NULL){
		list -> head = list -> tail = new;
	}else{
		new -> next = list -> head;
		new -> prev = NULL;
		list -> head -> prev = new;
		list -> head = new;
	}
	if(list -> numProcessInMemory != EMPTY){
		(list -> numProcessInMemory)++;
	}
	return list;
}

//=====================================================================
memory_node* enqueue_pagePointer(memory_list* list, memory_node* new, memory_node* start){
	if(list->head == NULL){
		list = enqueue_memory(list, new);
		return new;
	}
	memory_node* cur = start;

	if(cur->page->indexStart > new->page->indexStart){
		cur->prev = new;
		new->next = cur;
		list->head = new;
		return new;
	}

	while(cur != NULL){
		if(cur->next == NULL){
			list = enqueue_memory(list, new);
			return new;
		}
		if(cur->next->page->indexStart > new->page->indexStart){
			cur->next->prev = new;
			new -> next = cur->next;
			new -> prev = cur;
			cur -> next = new;
			return new;
		}
		cur = cur -> next;
	}
	return new;
}

//=====================================================================
// remove a node from the tail of the list
memory_list* dequeue_memory(memory_list* list){
	if(list -> head == NULL){
		return list;
	}
	memory_node *cur = list -> head;

	// keep track of num of processInMemory
	if(list -> numProcessInMemory != EMPTY){
		(list -> numProcessInMemory)--;
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

//=====================================================================
memory_node* pop_dequeue_memory(memory_list* list, memory_node* cur){
	if(cur == NULL){
		return cur;
	}
	if(cur -> next == NULL){
		if(cur -> prev == NULL){
			list -> head = NULL;
			list -> tail = NULL;
		}
		else{
			list -> tail = cur -> prev;
			cur -> prev -> next = NULL;
		}
	}
	else if(cur -> prev == NULL){
		list -> head = cur -> next;
		cur -> next -> prev = NULL;
	}
	else{
		cur -> next -> prev = cur -> prev;
		cur -> prev -> next = cur -> next;
	}
	cur -> next = NULL;
	cur -> prev = NULL;

	(list -> numProcessInMemory)--;
	return cur;
}

//=====================================================================
void print_list_pages(memory_list* list){
	memory_node* cur = list -> head;
	printf("emptyPages: %lld, totalPages: %lld \n", list->emptyPages, list->totalPages);
	int i = 0;
	while(cur != NULL){
		printf("node%d: %d %lld %lld\n", i, cur->isUsed, cur->indexStart, cur->len);
		cur = cur-> next;
		i++;
	}
}

//=====================================================================
void print_node_page(memory_node* cur){
	if(cur != NULL){
		printf("node: %d %lld %lld\n", cur->isUsed, cur->indexStart, cur->len);
	}
}

//=====================================================================
void print_list_pagePointers(memory_list* list){
	memory_node* cur = list -> head;
	printf("numPages: %lld \n", list->numPages);
	int i = 0;
	while(cur != NULL){
		printf("node%i: %d %lld %lld\n", i, cur->page->isUsed, cur->page->indexStart, cur->page->len);
	
		cur = cur-> next;
		i++;
	}
}

//=====================================================================
void print_node_pagePointer(memory_node* cur){
	if(cur != NULL){
		printf("node: %d %lld %lld\n", cur->page->isUsed, cur->page->indexStart, cur->page->len);
	}
}

//=====================================================================
void print_list_processInMemory(memory_list* list){
	memory_node* cur = list -> head;
	printf("numProcessInMemory: %lld \n", list->numProcessInMemory);
	int i = 0;
	while(cur != NULL){
		printf("node%i:cur->process->pro_id: %lld, cur->pagePointers->numPages: %lld\n", i, cur->process->pro_id, cur->pagePointers->numPages);
	
		cur = cur-> next;
		i++;
	}
}

//=====================================================================
void print_node_processInMemory(memory_node* cur){
	if(cur != NULL){
		printf("node:cur->process->pro_id: %lld, cur->pagePointers->numPages: %lld\n", cur->process->pro_id, cur->pagePointers->numPages);
	}
}
//=====================================================================
