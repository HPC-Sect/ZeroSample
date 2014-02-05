/*****************************************************************************

			
  Example     : pthread-findminimumvalue.c
 
  Objective   : Finds the Minimum Value in the Integer List.

  Input       : Number of Threads,
                Integer List Size.

  Output      : Minimum value in the Integer List
	                                                                                                                    
  Created     : Aug 2011  

  E-mail      : RarchK         
                                 
****************************************************************************/


#include<pthread.h>
#include<stdio.h>
#include<sys/time.h>
#include<stdlib.h>

#define MIN_INT 0
#define MAXTHREADS 8

void *find_min(void *) ;

pthread_mutex_t minimum_value_lock;

long int partial_list_size;
int minimum_value;
long int *list;
long int NumElements, CLASS_SIZE;
int NumThreads;

int main (int argc,char * argv[]) 
{
 
        pthread_t       *threads;
        pthread_attr_t  pta;
        int             iteration,THREADS,ret_count;
        double          time_start, time_end;
        struct          timeval tv;
        struct          timezone tz;
        double          MemoryUsed = 0.0;
	char 		* CLASS;

        int counter;        
        printf("\n\t\t--------------------------------------------------------------------------");
        printf("\n\t\t Centre for Development of Advanced Computing (C-DAC)");
        printf("\n\t\t C-DAC Multi Core Benchmark Suite 1.0");
        printf("\n\t\t Email : RarchK");
        printf("\n\t\t---------------------------------------------------------------------------");
        printf("\n\t\t Objective : Sorting Single Dimension Array (Integer Operations)\n ");
        printf("\n\t\t Performance of Sorting a Minimum value in a large Single Dimension Array ");
        printf("\n\t\t on Multi Socket Multi Core Processor using 1/2/4/8 threads \n");
        printf("\n\t\t..........................................................................\n");
       
         if( argc != 3 ){

            printf("\t\t Very Few Arguments\n ");
            printf("\t\t Syntax : exec <Class-Size> <Threads>\n");
            exit(-1);
         }
         else {
            CLASS = argv[1];
            THREADS = atoi(argv[2]);
         }
         if( strcmp(CLASS, "A" )==0){
            CLASS_SIZE = 10000000;
         }
         if( strcmp(CLASS, "B" )==0){
            CLASS_SIZE = 100000000;
         }
        if( strcmp(CLASS, "C" )==0){
            CLASS_SIZE = 1000000000;
        }

        NumElements = CLASS_SIZE; 
        NumThreads = THREADS;
        printf("\n\t\t Array Size  :  %ld",NumElements);
        printf("\n\t\t Threads     :  %d",NumThreads);
        printf("\n");

        if (NumThreads < 1 )
        {
           printf("\n Number of threads must be greater than zero. Aborting ...\n");
           return 0;
        }

        if ((NumThreads != 1) && (NumThreads != 2) && (NumThreads != 4) && (NumThreads != 8))
        {
           printf("\n Number of Threads must be 1 or 2 or 4 or 8. Aborting ...\n");
           return 0;
        }

        if ( ( NumElements % NumThreads ) != 0 )
        {
           printf("\n Number of threads not a factor of Integer List size. Aborting.\n");
 	   return 0 ;
        }


        partial_list_size = NumElements / NumThreads;

        list = (long int *)malloc(sizeof(long int) * NumElements);
        MemoryUsed += ( NumElements * sizeof(long int));
          
        gettimeofday(&tv, &tz);
        time_start = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
         
        for(counter = 0 ; counter < NumElements ; counter++){
            srand48((unsigned int)NumElements);
            list[counter] = (rand()%1000)+1.0;
        }
         
        threads = (pthread_t *)malloc(sizeof(pthread_t)*NumThreads);       

        minimum_value = list[0];         

        ret_count=pthread_mutex_init(&minimum_value_lock, NULL);
    	if(ret_count)
    	{
		printf("\n ERROR : Return code from pthread_mutex_init() is %d ",ret_count);
		exit(-1);
    	}
         
        ret_count=pthread_attr_init(&pta);
    	if(ret_count)
    	{
		printf("\n ERROR : Return code from pthread_attr_init() is %d ",ret_count);
		exit(-1);
    	}

        pthread_attr_setscope(&pta,PTHREAD_SCOPE_SYSTEM);

        for(counter = 0 ; counter < NumThreads ; counter++)
	{
           ret_count=pthread_create(&threads[counter],&pta,(void *(*) (void *)) find_min,(void *) (counter+1));
    	   if(ret_count)
    	   {
		printf("\n ERROR : Return code from pthread_create() is %d ",ret_count);
		exit(-1);
    	   }
	}
      	
        for(counter = 0 ; counter < NumThreads ; counter++)
	{
           ret_count=pthread_join(threads[counter],NULL);
    	   if(ret_count)
    	   {
		printf("\n ERROR : Return code from pthread_join() is %d ",ret_count);
		exit(-1);
    	   }
        }
        ret_count=pthread_attr_destroy(&pta);
    	if(ret_count)
    	{
		printf("\n ERROR : Return code from pthread_attr_destroy() is %d ",ret_count);
		exit(-1);
    	}

        gettimeofday(&tv, &tz);
        time_end = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;

        printf("\n\t\t Minimum Value found in the Integer list     :  %d",minimum_value);
        printf("\n\t\t Memory Utilised                             :  %lf MB",(MemoryUsed / (1024*1024)));
        printf("\n\t\t Time Taken in Seconds  (T)                  :  %lf Seconds",( time_end - time_start));   
        printf("\n\t\t..........................................................................\n");

        
        free(list);
        free(threads);
        return 0;

 }
 void *find_min(void * myid ) {

     int my_min;
     long int counter;

     int myId = (int)myid;

     my_min = list[(myId-1)*partial_list_size];

     for (counter = ((myId - 1) * partial_list_size); counter <= ((myId * partial_list_size) - 1); counter++){
        if (list[counter] < my_min)
            my_min = list[counter];
     }

     /* lock the mutex associated with minimum_value and update the variable as      
     required */

    pthread_mutex_lock(&minimum_value_lock) ;
    if (my_min < minimum_value)
       minimum_value = my_min;
 
    /* and unlock the mutex */
    pthread_mutex_unlock(&minimum_value_lock) ;
   
    pthread_exit(NULL);
}

