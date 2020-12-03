#include "linked_list.h"
#include "helperFunctions.h"

//=====================================================================
LLI max_compare(LLI a, LLI b){
   if(a < b){
      return b;
   }
   return a;
}

//=====================================================================
LLI min_compare(LLI a, LLI b){
   if(a > b){
      return b;
   }
   return a;
}

//=====================================================================
int comparePages(const void * a, const void * b) {
   page_t *c = (page_t *) a;
   page_t *d = (page_t *) b;

   if (c->indexStart < d->indexStart) {
      return -1; // a is smaller
   }
   return 1;
}

//=====================================================================
void printPageNumberInOrder(page_t *array_pagePointers_list, LLI size){
   if (array_pagePointers_list == NULL) return;
   printf("[");
   for(LLI i = 0; i < size; i++ ){
      for(LLI j = 0; j < array_pagePointers_list[i].len; j++){
         printf("%lld",array_pagePointers_list[i].indexStart + j);
         if( i != size - 1 || j != array_pagePointers_list[i].len - 1){
            printf(",");
         }
      }
   }
   printf("]");
}

//=====================================================================
void sortPageNumberArray(page_t *array_pagePointers_list, LLI size){
   if (array_pagePointers_list == NULL) return;
   qsort((void *)array_pagePointers_list, size, sizeof(page_t), comparePages);
}

//=====================================================================
void storePagePointersIntoArray(page_t *array_pagePointers_list, LLI size, memory_list* pagePointers, LLI *indexOfArray, LLI numPages_req){
   if(array_pagePointers_list == NULL || pagePointers->numPages < numPages_req || numPages_req <= 0){
      return;
   }
   memory_node* cur = pagePointers->head;
   LLI count = 0;
   LLI page_len;
   while(cur != NULL){

      page_len = cur -> page -> len;
      array_pagePointers_list[*indexOfArray].indexStart = cur->page->indexStart;
      array_pagePointers_list[*indexOfArray].len = page_len; 

      if( count + page_len >= numPages_req){
         array_pagePointers_list[*indexOfArray].len = numPages_req - count;
         *indexOfArray = *indexOfArray + 1;
         return;
      }

      count = count + page_len;
      (*indexOfArray)++;
      cur = cur -> next;
   }
}

//=====================================================================
void printPageNumber(memory_list* pagePointers, LLI numPages_req){
   if(pagePointers->numPages < numPages_req || numPages_req <= 0){
      return;
   }
   printf("[");
   memory_node* cur = pagePointers->head;
   LLI count = 0;
   while(cur != NULL){
      // convertPageNumberToString(cur->page);
      for(LLI i = 0; i < cur->page->len; i++){
         printf("%lld",cur->page->indexStart + i);
         count = count + 1;
         
         if(count == numPages_req){
            printf("]");
            return;
         }
         printf(",");
      }
      cur = cur->next;
   }
   printf("]");
}

//=====================================================================
void convertPageNumberToString(memory_node* page){

   for(LLI i = 0; i < page->len; i++){
      printf("%lld, ",page->indexStart + i);
   }
}

//=====================================================================
void loadThisProcessIntoMemory(memory_list* processInMemory_list, memory_list* pages, list_t* queue, LLI* time, int cm_orNot){
   if(queue->running == NULL){
      return;
   }
   node_t* procsRunning = queue->running; // ready to run
   LLI pagesReq = procsRunning->mem_size/4;
  
   LLI pages_found = 0;
   LLI process_toEvict = 0;
   LLI blocks_toEvict = 0;
   LLI pages_toEvict = 0;
   int evictIsNeeded = FLAG_FALSE;
   LLI size;
   page_t *array_pagePointers_list;
   LLI indexOfArray = 0;

   if(checkifEnoughEmptyPages(pages, procsRunning) < 0){
      evictIsNeeded = FLAG_TRUE;
      pages_toEvict = -(checkifEnoughEmptyPages(pages, procsRunning));

      memory_node* cur = processInMemory_list->head;
      while(pages_found < pages_toEvict && cur != NULL){
         pages_found = pages_found +  cur->pagePointers-> numPages;
         process_toEvict = process_toEvict + 1;
         
         memory_node* temp = cur->pagePointers->head;
         LLI num_blocks = 0;
         while(temp != NULL){
            temp = temp->next;
            num_blocks ++;
         }
         cur = cur->next;
         blocks_toEvict = blocks_toEvict + num_blocks;
      }
   }

   if(evictIsNeeded){
      size = blocks_toEvict;
      array_pagePointers_list = (page_t *)malloc(size*sizeof(page_t));
      indexOfArray = 0;

      for(LLI i = 0; i < size; i++ ){
         array_pagePointers_list[i].indexStart = 0;
         array_pagePointers_list[i].len = 0;
      }
   }
   
   while(checkifEnoughEmptyPages(pages, procsRunning) < 0){
      evictLeastRecentProcess(processInMemory_list, pages, queue, time, NOT_COMPLETED, SWAPPING, array_pagePointers_list, size, &indexOfArray, (LLI)(processInMemory_list->head->process->mem_size/4), cm_orNot);
   }

   if(evictIsNeeded){
      sortPageNumberArray(array_pagePointers_list, size);
      printf("%lld, EVICTED, mem-addresses=", *time);

      printPageNumberInOrder(array_pagePointers_list, size);
      printf("\n");
      free(array_pagePointers_list);
      indexOfArray = 0;
   }
  
   
   memory_list* pagePointers = make_new_pagePointers_list();
   loadAllPagesIntoMemory(pagePointers, pages, pagesReq); 
   memory_node* processInMemory = make_new_processInMemory_node(pagePointers, procsRunning);
   processInMemory_list -> runningProcess = procsRunning;

   if(cm_orNot == CM_TRUE){
      processInMemory_list = push_enqueue_memory_CM(processInMemory_list, processInMemory);
   }else{
      processInMemory_list = enqueue_memory(processInMemory_list, processInMemory);
   }
   
   procsRunning -> processInMemory = processInMemory;
}

//=====================================================================
void loadThisProcessIntoMemory_virtualMemory(memory_list* processInMemory_list, memory_list* pages, list_t* queue, LLI* time, int virtualMemoryOrNot, int addNewProcessInMemoryOrNot, LLI pages_req_evict, LLI pages_req_load){
   if(queue->running == NULL){
      return;
   }
   node_t* procsRunning = queue->running; // ready to run
   LLI pagesReq = pages_req_load;
   LLI pages_found = 0;
   LLI process_toEvict = 0;
   LLI blocks_toEvict = 0;
   LLI pages_toEvict = 0;
   int evictIsNeeded = FLAG_FALSE;
   LLI stopPage = 0;
   LLI size;
   page_t *array_pagePointers_list;
   LLI indexOfArray = 0;
   int lastBlockOrNot = FLAG_FALSE;
   int lastProcessOrNot = FLAG_FALSE;

    if(pages_req_evict > 0){
      evictIsNeeded = FLAG_TRUE;
      pages_toEvict = pages_req_evict;

      memory_node* cur = processInMemory_list->head;
      while(lastProcessOrNot == FLAG_FALSE && pages_found < pages_toEvict && cur != NULL){

         if(pages_found + cur->pagePointers->numPages >= pages_toEvict){
            stopPage = pages_toEvict - pages_found;
            lastProcessOrNot = FLAG_TRUE;
            pages_found = pages_toEvict;
         }else{
            pages_found = pages_found +  cur->pagePointers-> numPages;
         }
         
         process_toEvict = process_toEvict + 1;
        
         memory_node* temp = cur->pagePointers->head;
         LLI num_blocks = 0;
         LLI num_pages_inBlock = 0;
         while(lastBlockOrNot == FLAG_FALSE && temp != NULL){
            if(lastProcessOrNot == FLAG_TRUE){
               num_pages_inBlock = num_pages_inBlock + temp->page->len;
               if(num_pages_inBlock >= stopPage){
                  lastBlockOrNot = FLAG_TRUE;
               }
            } 
            temp = temp->next;
            num_blocks ++;
         }
         cur = cur->next;
         blocks_toEvict = blocks_toEvict + num_blocks;
      }

   }
   if(evictIsNeeded){
      size = blocks_toEvict;
      array_pagePointers_list = (page_t *)malloc(size*sizeof(page_t));
      indexOfArray = 0;
      for(LLI i = 0; i < size; i++ ){
         array_pagePointers_list[i].indexStart = 0;
         array_pagePointers_list[i].len = 0;
      }
   }

   while(pages_toEvict > 0){
      evictLeastRecentProcess(processInMemory_list, pages, queue, time, NOT_COMPLETED, VIRTUAL_MEMORY, array_pagePointers_list, size, &indexOfArray, pages_toEvict, CM_FALSE);    
      pages_toEvict = pages_req_load - (pages -> emptyPages);
   }

   if(evictIsNeeded){
      sortPageNumberArray(array_pagePointers_list, size);
      printf("%lld, EVICTED, mem-addresses=", *time);
      printPageNumberInOrder(array_pagePointers_list, size);
      printf("\n");
      free(array_pagePointers_list);
      indexOfArray = 0;
   }
  
   if(addNewProcessInMemoryOrNot == ADD_NEWPROCESS_TRUE){
      memory_list* pagePointers = make_new_pagePointers_list();
      loadAllPagesIntoMemory(pagePointers, pages, pagesReq); 
      memory_node* processInMemory = make_new_processInMemory_node(pagePointers, procsRunning);
      processInMemory_list -> runningProcess = procsRunning;
      processInMemory_list = enqueue_memory(processInMemory_list, processInMemory);
      procsRunning -> processInMemory = processInMemory;
   }else{
      loadAllPagesIntoMemory(procsRunning->processInMemory->pagePointers, pages, pagesReq);
      processInMemory_list -> runningProcess = procsRunning;
   }
}

//=====================================================================
void evictLeastRecentProcess(memory_list* processInMemory, memory_list* pages, list_t* queue, LLI* time, int completedOrNot, int virtualMemoryOrNot, page_t *array_pagePointers_list, LLI size, LLI *indexOfArray, LLI pages_req_evict, int cm_orNot){
   if(processInMemory == NULL || processInMemory->head == NULL){
      return;
   }
   memory_node* processEvicted = processInMemory -> head;

   if(pages_req_evict > processEvicted->pagePointers->numPages){
      pages_req_evict = processEvicted->pagePointers->numPages;
   }

   if(completedOrNot == COMPLETED){
      if(cm_orNot == CM_TRUE){
         processEvicted = processInMemory->head;
      }else{
         processEvicted = processInMemory->tail;
      }

      pages_req_evict = processEvicted->pagePointers->numPages;
      printf("%lld, EVICTED, mem-addresses=", *time);
      printPageNumber(processEvicted->pagePointers, processEvicted->pagePointers->numPages);
      printf("\n");
   }
   else{
      if(virtualMemoryOrNot == VIRTUAL_MEMORY){
          storePagePointersIntoArray(array_pagePointers_list, size, processEvicted -> pagePointers, indexOfArray, pages_req_evict);
      }else if(virtualMemoryOrNot == SWAPPING){
          storePagePointersIntoArray(array_pagePointers_list, size, processEvicted -> pagePointers, indexOfArray, processEvicted->pagePointers->numPages);
      }
     
   }

   if(virtualMemoryOrNot == VIRTUAL_MEMORY){
      evictPagesOfAProcess(processEvicted->pagePointers, pages, pages_req_evict);
   }else if(virtualMemoryOrNot == SWAPPING){
      evictPagesOfAProcess(processEvicted->pagePointers, pages, processEvicted->process->mem_size/4);
   }
   
   if(virtualMemoryOrNot == SWAPPING){
      processEvicted -> process -> processInMemory = NULL;
   }

   if(processEvicted ->pagePointers -> numPages <= 0){
      if(virtualMemoryOrNot == VIRTUAL_MEMORY){
         processEvicted -> process -> processInMemory = NULL;
      }
      processEvicted = pop_dequeue_memory(processInMemory, processEvicted);
      
      free_list_memory(processEvicted->pagePointers);
      free(processEvicted);
   } 
}

//=====================================================================
void ifFinishEvictedPages(list_t* list, memory_list* processInMemory, memory_list* pages, LLI* time, int virtualMemoryOrNot, int cm_orNot){
   if(list -> running == NULL){
      return;
   }

   page_t *array_pagePointers_list = NULL;
   LLI indexOfArray = 0;

   if(list -> running -> remain_tm == 0){
      if(cm_orNot == CM_TRUE){
         evictLeastRecentProcess(processInMemory, pages, list, time, COMPLETED, virtualMemoryOrNot, array_pagePointers_list, 0, &indexOfArray, processInMemory->head->pagePointers->numPages, cm_orNot);
      }else{
         evictLeastRecentProcess(processInMemory, pages, list, time, COMPLETED, virtualMemoryOrNot, array_pagePointers_list, 0, &indexOfArray, processInMemory->tail->pagePointers->numPages, cm_orNot);
      }  
   }
}

//=====================================================================
void checkFinish(list_t* list, LLI *time, LLI *processes_total, LLI *throughput_min, LLI *throughput_max, LLI *cur_throughput, LLI *intervals, LLI *cur_interval, LLI *turnaround_total, double *timeoverhead_max, double *timeoverhead_total){
   if(list -> running == NULL){
      return;
   }

   node_t* completed = pop_dequeue(list, list->running);

   if(list -> running -> remain_tm == 0){
      // this process id ended, so free it
      printf("%lld, FINISHED, id=%lld, proc-remaining=%lld\n", *time, list->running->pro_id, list->amount);
      
      LLI turnaround = *time - list->running->tm_arvd;
      *turnaround_total = *turnaround_total + turnaround;
 
      double timeoverhead = (turnaround)/(double)list->running->job_tm;

      *timeoverhead_total = *timeoverhead_total + timeoverhead;

      if(timeoverhead > *timeoverhead_max){
         *timeoverhead_max =timeoverhead;

      }

      free(completed);  

      *intervals = (LLI)ceil((double)*time/(double)60);
      if(*intervals == 1 && *cur_throughput == 0){
         *throughput_min = 1;
      }
      // still in the same interval
      if(*intervals == *cur_interval){
         (*cur_throughput)++;
         if(*cur_throughput > *throughput_max){
            *throughput_max = *cur_throughput;
         }
      }
      // enters a new(next) interval
      else if(*intervals > *cur_interval){
         if(*intervals - *cur_interval > 1){
            *throughput_min = 0;
         }
         if(*cur_throughput > *throughput_max){
            *throughput_max = *cur_throughput;
         }
         if(*cur_throughput < *throughput_min){
            *throughput_min = *cur_throughput;
         }
         *cur_throughput = 1;
         *cur_interval = *intervals;
      }

      (*processes_total)++;

   }else{
      // Add this process back to the end of the queue, since it hasn't finished yet
      list = enqueue(list, completed);
   }
   list -> running = NULL; 
}

//=====================================================================
LLI startRunning(list_t* list, LLI *time, LLI quantum){
   if(list -> running == NULL){
      return 0;
   }
   if(quantum == -1){
      quantum = list->head->job_tm;
   }

   printf("%lld, RUNNING, id=%lld, remaining-time=%lld", *time, list->running->pro_id, list->running->remain_tm);
   if(list -> running -> remain_tm <= quantum){
     return list->running->remain_tm;
   }else{
      return quantum;
   }
}

//=====================================================================
void chooseWhichProcessToRun(list_t* list){
   if(list -> head == NULL){
      return;
   }
   // a process starts running
   list -> running = list -> head;
}

//=====================================================================
LLI checkifEnoughEmptyPages(memory_list* pages, node_t* process){
   // if it's greater or equal to zero, there is enough memory
   // if it's less than 0, not enough memory
    return (pages -> emptyPages) - (LLI)(process->mem_size/4);
}

//=====================================================================
memory_node* lookForEmptyPages(memory_list* pages){
   if(pages == NULL){
      return NULL;
   }
   memory_node* cur;
   cur = pages -> head;
   while(cur != NULL){
      if(cur -> isUsed == NOT_INUSED){
         return cur;
      }
      cur = cur -> next;
   }
   return NULL;
}

//=====================================================================
void loadAllPagesIntoMemory(memory_list* pagePointers, memory_list* pages, LLI pages_req){
   if(pages_req > pages->emptyPages){
      return;
   }
   memory_node* cur = lookForEmptyPages(pages);
   memory_node* pagePointer;
   memory_node* tmp;
   memory_node* start = pagePointers -> head;
   while(pages_req > 0 && cur != NULL){
      
      // when cur bloack is not big enough
      if(pages_req >= cur->len){
         tmp = loadPageIntoMemory(pages, cur, cur->len);

         pagePointer = make_new_pagePointer_node(tmp);
         start = enqueue_pagePointer(pagePointers, pagePointer, start);
         pagePointers->numPages = pagePointers->numPages + cur->len;

         pages_req = pages_req - cur->len;
      }
      else if(pages_req < cur->len){
         tmp = loadPageIntoMemory(pages, cur, pages_req);

         pagePointer = make_new_pagePointer_node(tmp);
         start = enqueue_pagePointer(pagePointers, pagePointer, start);
         pagePointers->numPages = pagePointers->numPages + pages_req;

         pages_req = 0;
      }

      cur = lookForEmptyPages(pages);
   }   
}

//=====================================================================
memory_node* loadPageIntoMemory(memory_list* pages, memory_node* cur, LLI len_req){
   if(cur == NULL || cur->isUsed == INUSED){
      return NULL;
   }
   if(len_req > cur->len){
      return NULL;
   }
   if(len_req == cur->len){
      cur->isUsed = INUSED;
      pages->emptyPages = pages->emptyPages - len_req;
      return cur;
   }
   if(len_req < cur->len){
      pages->emptyPages = pages->emptyPages - len_req;
      memory_node* new = make_new_page_node(INUSED, cur->indexStart, len_req);
      cur->indexStart = cur->indexStart + len_req;
      cur->len = cur->len - len_req;
      new->next = cur;
      new->prev = cur->prev;

      // cur is the head of the list
      if(cur->prev != NULL){      
         cur->prev->next = new;
      }else{
         pages->head = new;
      }
      cur->prev = new;
      return new;
   }
   return NULL;
}

//=====================================================================
void evictPagesOfAProcess(memory_list* pagePointers, memory_list* pages, LLI pagesToEvicted){
   if(pagePointers == NULL || pagePointers->head == NULL || pagesToEvicted > pagePointers->numPages){
      return;
   }

   memory_node *cur, *pre;
   cur = pagePointers->head;
   int flag_end = FLAG_FALSE;
   while(flag_end != FLAG_TRUE && cur!=NULL && pagesToEvicted > 0){
      pre = cur;
      cur = cur -> next;

      if(pagesToEvicted < pre->page->len){
         evictPartOfthePages(pages, pre->page, pagesToEvicted);

         pagePointers->numPages = pagePointers->numPages - pagesToEvicted;

         pagesToEvicted = 0;
         pagePointers->head = pre;
         flag_end = FLAG_TRUE;
      }else{
         pagePointers->numPages = pagePointers->numPages - pre->page->len;

         pagesToEvicted = pagesToEvicted - pre->page->len;

         evictPage(pages, pre->page);

         if(cur != NULL) {
            cur->prev = NULL;
         }
         pagePointers->head = cur;
         free(pre);
      }
      
   }

}

//=====================================================================
void evictPage(memory_list* pages, memory_node* cur){
   if(pages ==  NULL){
      return;
   }
   if(cur == NULL || cur->isUsed == NOT_INUSED){
      return;
   }
   cur->isUsed = NOT_INUSED;
   pages->emptyPages = pages->emptyPages + cur->len;
   // Merge with next
   if(cur->next != NULL && cur->next->isUsed == NOT_INUSED){
      memory_node* tmp = cur->next;

      cur->len = (cur->next->len) + (cur->len);
      cur->next = cur->next->next;
      if(cur->next != NULL) {
         cur->next->prev = cur;
      } else {
         pages->tail = cur;
      }

      free(tmp);
   }

   // Merge with prev
   if(cur->prev != NULL && cur->prev->isUsed == NOT_INUSED){
      memory_node* tmp = cur->prev;

      cur->len = (cur->prev->len) + (cur->len);
      cur->indexStart = cur->prev->indexStart;
      cur->prev = cur->prev->prev;

      if(cur->prev != NULL) {
         cur->prev->next = cur;
      } else {
         pages->head = cur;
      }

      free(tmp);
   }
}

//=====================================================================
// e.g. the pageblock size is 10, but I only want to evict 
void evictPartOfthePages(memory_list* pages, memory_node* cur, LLI len_req){
   if(len_req >= cur->len){
      return;
   }
   pages->emptyPages = pages->emptyPages + len_req;
   if(cur->prev == NULL || cur->prev->isUsed == INUSED){
      memory_node* new = make_new_page_node(NOT_INUSED, cur->indexStart, len_req);
      new->next = cur;
      if(cur->prev == NULL){
         pages->head = new;
      }
      else if(cur->prev->isUsed == INUSED){
         cur->prev->next = new;
         new->prev = cur->prev;
      }
      cur->prev = new;
   }else if(cur->prev->isUsed == NOT_INUSED){
      cur->prev->len = cur->prev->len + len_req;
   }
   cur->indexStart = cur->indexStart + len_req;
   cur->len = cur->len - len_req;
   cur->isUsed = INUSED;
}
//=====================================================================