#include "linked_list.h"
#include "helperFunctions.h"


int main(int argc, char *argv[])
{
   // Read Command Line inputs ============================================================================
   int option;

   char file[MAX_FILENAME_LEN];
   memset(file, '\0', sizeof(file));

   char sch_algorithm[MAX_STRING_LEN];
   memset(sch_algorithm, '\0', sizeof(sch_algorithm));

   char mem_allocate[MAX_STRING_LEN];
   memset(mem_allocate, '\0', sizeof(mem_allocate));

   LLI mem_size = 0;

   LLI quantum = 10;

   while((option = getopt(argc, argv, ":f:a:m:s:q:")) != -1){ 
      switch(option){

         case 'f': // file name
         if(strlen(optarg) >= MAX_FILENAME_LEN){

         }else{
            strcpy(file, optarg);
         }
         break;

         case 'a': // scheduling-algorithm
         strcpy(sch_algorithm, optarg);
         break;

         case 'm': // memory-allocation
         strcpy(mem_allocate, optarg);
         break;

         case 's': // memory-size
         mem_size = atoll(optarg);
         break;

         case 'q': // quantum
         quantum = atoll(optarg);
         break;
      }
  }
   // Set up which mode to use ==========================================================================
   int useMemory = NOT_USEMEMORY;
   int virtualMemoryOrNot = SWAPPING;
   int cs_SFJ = SJF_FALSE;
   int cm_orNot = CM_FALSE; 

   if(strcmp(mem_allocate, "u") != 0) {
      useMemory = 1;
   }
   if(strcmp(mem_allocate, "v") == 0) {
      virtualMemoryOrNot = VIRTUAL_MEMORY;
   }
   if(strcmp(sch_algorithm, "cs") == 0){
      cs_SFJ = SJF_TRUE;
   }
   if(strcmp(mem_allocate, "cm") == 0){
      cm_orNot = CM_TRUE;
   }

    // Read-file's preparation =========================================================================
   FILE *fp;
   fp = fopen(file, "r"); // read mode

   if (fp == NULL){
      perror("Error while opening the file.\n");
      exit(EXIT_FAILURE);
   }

   // queue of processes
   list_t *queue = make_empty_list();
   LLI time_arrived, process_id, mem_size_req, job_time;
   LLI time = 0;

   // statistics =====================================
   LLI throughput_max = 0;
   LLI throughput_min = 0;
   LLI processes_total = 0;
   LLI cur_throughput = 0;
   LLI intervals = 1;
   LLI cur_interval = 1;
   LLI turnaround_total = 0;
   double timeoverhead_total = 0;
   double timeoverhead_max = 0;

   // signal flags to identify condition/mode ========
   int flag_moreLine = FLAG_TRUE;
   int flag_NotArrived = FLAG_FALSE;
   int flag_saved = FLAG_TRUE;
   int process_stillRunning = FLAG_TRUE;
   // store the input temporarily
   node_t* stored = NULL;
   // time that takes this turn to run
   LLI time_ofThisTurn = 0;
   // queue of processes which are in memory (double linked list)
   memory_list* processInMemory_list = make_new_processInMemory_list();
   // queue of memory pages (double linked list)
   memory_list* pages = make_new_pages_list(mem_size);

   // Read the file inputs line by line when they arrive ================================================
   while(flag_moreLine || process_stillRunning){

      if(flag_saved && fscanf(fp, "%lld %lld %lld %lld\n", &time_arrived, &process_id, &mem_size_req, &job_time) == 4){
         flag_moreLine = FLAG_TRUE;
         stored = make_new_node(time_arrived, process_id, mem_size_req, job_time);
         flag_saved = FLAG_FALSE;

         if(time >= time_arrived){

            if(cs_SFJ == SJF_TRUE){
               queue = SJF_sorted_enqueue(queue, stored);
            }else{
               queue = sorted_enqueue(queue, stored);
            }

            flag_saved = FLAG_TRUE;
            stored = NULL;
            flag_NotArrived = FLAG_FALSE;
         }

         // look for and check if more processes arrive at the same time
         while(!flag_NotArrived && flag_saved){
            if(flag_saved && fscanf(fp, "%lld %lld %lld %lld\n", &time_arrived, &process_id, &mem_size_req, &job_time) == 4){
               flag_moreLine = 1;
               stored = make_new_node(time_arrived, process_id, mem_size_req, job_time);
               flag_saved = 0;

               if(time >= time_arrived){

                  if(cs_SFJ == SJF_TRUE){
                     queue = SJF_sorted_enqueue(queue, stored);
                  }else{
                     queue = sorted_enqueue(queue, stored);
                  }

                  flag_saved = FLAG_TRUE;
                  stored = NULL;
               }else{
                  flag_NotArrived = FLAG_TRUE;
               }
            }else{
               flag_NotArrived = FLAG_TRUE;
            }
         }
      }

      // there are no more input lines
      else if(flag_saved == FLAG_TRUE){
          flag_moreLine = FLAG_FALSE;
      }

      // Processes scheduling part + Memory Management part ==================================================
      if(useMemory){
          // if process finishes, evict the pages first
         ifFinishEvictedPages(queue, processInMemory_list, pages, &time, virtualMemoryOrNot, cm_orNot);
      }  
      // check the process finishes, then end it
      checkFinish(queue, &time, &processes_total, &throughput_min, &throughput_max, &cur_throughput,&intervals, &cur_interval, &turnaround_total, &timeoverhead_max, &timeoverhead_total);
      // choose next process to run
      chooseWhichProcessToRun(queue);

      if(queue -> running != NULL){
         LLI loading_time = 0;
         LLI pageFault_penalty = 0;

         // Memory Management ================================================
         if(useMemory){

            // Swapping or CM ====================
            if(!strcmp(mem_allocate, "p") || !strcmp(mem_allocate, "cm")){

               if(queue->running->processInMemory != NULL){
                  memory_node* cur = pop_dequeue_memory(processInMemory_list, queue->running->processInMemory);
                  if(cm_orNot == CM_TRUE){
                     processInMemory_list = push_enqueue_memory_CM(processInMemory_list, cur);
                  }else{
                     processInMemory_list = enqueue_memory(processInMemory_list, cur);
                  }
               }else{
                  if(cm_orNot == CM_TRUE){
                     loadThisProcessIntoMemory(processInMemory_list, pages, queue, &time, CM_TRUE);
                  }else{
                     loadThisProcessIntoMemory(processInMemory_list, pages, queue, &time, CM_FALSE);
                  }
                  loading_time = (LLI)(queue->running->mem_size/4)*2;
               }
            }

            // Virtual Memory =====================
            else if(!strcmp(mem_allocate, "v")){

               LLI job_size = 0;
               LLI min_pages;
               LLI evict_pages;
               LLI inMemory_pages = 0;
               LLI empty_pages = pages->emptyPages;
               LLI load_pages = 0;
               LLI pageFaults = 0;
               
               if(queue->running != NULL){
                     job_size = (LLI)queue->running->mem_size/4;
                     if(queue -> running -> processInMemory != NULL){
                        inMemory_pages = queue->running->processInMemory->pagePointers->numPages;
                     }
               }
               // calculate how many pages need to evict and load
               min_pages = min_compare(job_size, 4);
               evict_pages = max_compare(0, min_pages - inMemory_pages - empty_pages);
               load_pages = min_compare(job_size - inMemory_pages, empty_pages + evict_pages);

             
               if(job_size > load_pages + inMemory_pages){
                  pageFaults = job_size - (load_pages + inMemory_pages);
               }
               

               if(queue -> running -> processInMemory != NULL){
                  // process in memory already
                  memory_node* cur = pop_dequeue_memory(processInMemory_list, queue->running->processInMemory);
                  processInMemory_list = enqueue_memory(processInMemory_list, cur);
                  loadThisProcessIntoMemory_virtualMemory(processInMemory_list, pages, queue, &time, VIRTUAL_MEMORY, ADD_NEWPROCESS_FALSE, evict_pages, load_pages);

               }else{
                  // process not in memory
                  loadThisProcessIntoMemory_virtualMemory(processInMemory_list, pages, queue, &time, VIRTUAL_MEMORY, ADD_NEWPROCESS_TRUE, evict_pages, load_pages);
               
               }
               loading_time = load_pages*2;
               pageFault_penalty = pageFaults;
            }

         }
         // Add page fault penalty to the remain time
         queue->running->remain_tm += pageFault_penalty;

         // First In First Serve
         if(!strcmp(sch_algorithm, "ff")){
            time_ofThisTurn = startRunning(queue, &time, QUANTUM_NOT_NEEDED);
         // Round Robin
         }else if(!strcmp(sch_algorithm, "rr")){
            time_ofThisTurn = startRunning(queue, &time, quantum);
         // Shortest Job First
         }else if(!(strcmp(sch_algorithm, "cs"))){
            time_ofThisTurn = startRunning(queue, &time, QUANTUM_NOT_NEEDED);
         }


         if (useMemory) {        
            LLI numbPages_used = pages->totalPages - pages->emptyPages;
            LLI numPages_total = pages->totalPages;
            
            printf(", load-time=%lld, mem-usage=%d%%", loading_time, (int)ceil(100*(double)numbPages_used/(double)numPages_total));
            printf(", mem-addresses=");
            printPageNumber(processInMemory_list->tail->pagePointers, processInMemory_list->tail->pagePointers->numPages);
            printf("\n");
         }
         else{
            printf("\n");
         }

         // Update Time 
         time = time + time_ofThisTurn + loading_time;
         queue->running->remain_tm = queue->running->remain_tm - time_ofThisTurn;
      }

      else{
            if(stored != NULL){ 
               time = stored -> tm_arvd;
            }
      }


      // Keep reading file inputs (update the process queue) ====================================================

      if(flag_saved == FLAG_FALSE && time >= stored->tm_arvd){

         if(cs_SFJ == SJF_TRUE){
            queue = SJF_sorted_enqueue(queue, stored);
         }else{
            queue = sorted_enqueue(queue, stored);
         }
         flag_saved = FLAG_TRUE;
         stored = NULL;
      }

      if(queue -> amount <= 0 ){
         process_stillRunning = FLAG_FALSE;
      }

   }

   // Read files END ===========================================================================================================

   fclose(fp);

   // print out STATISTICS 

   LLI throughput_avg = (LLI)ceil((double)processes_total/(double)intervals);
   LLI turnaround_avg = (LLI)ceil((double)turnaround_total/(double)processes_total);
   double timeoverhead_avg = (double)timeoverhead_total/(double)processes_total;
   printf("Throughput %lld, %lld, %lld\n", throughput_avg, throughput_min, throughput_max);
   printf("Turnaround time %lld\n", turnaround_avg);
   printf("Time overhead %.2lf %.2lf\n", timeoverhead_max, timeoverhead_avg);
   printf("Makespan %lld\n", (LLI)time);

   free_list(queue);
   free_list_memory(processInMemory_list);
   free_list_memory(pages);

   return 0;
}
