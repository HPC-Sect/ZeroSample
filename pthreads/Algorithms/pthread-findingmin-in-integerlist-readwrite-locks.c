/*************************************************************************
 *                      
 *
 *   
 * Example     : pthread-findingmin-in-integerlist-readwrite-locks.c
 *         
 * Objective   : Code to find minimum value in an integer list using
 *               read-write lock implementation
 *              
 * Input       :  <NumElements> <NumThreads>
 *           
 * Output      : Minimum value and Execution time.
 *                      
 * Description : Here minimum value is found in an integer list using synchronisation
 *               construct read-write locks.Number of elements and number of threads 
 *               are taken from user.  

 * Created     : Aug 2011
 * 
 *                                          
 * Email       : RarchK
 *                                 
 ****************************************************************************/

#include<pthread.h>
#include<stdio.h>
#include<sys/time.h>				// header file inclusion
#include<stdlib.h>



#define MIN_INT 0				// define necessary values
#define MAXTHREADS 8

void *find_min_rwlock(void *) ;
void checkResults(char *string, int rc);

pthread_rwlock_t       rwlock;                   // declaration of read-write lock variable

long int partial_list_size;
int minimum_value;
long int *list;                                     // declare global variables
long int NumElements;
int NumThreads;

int main (int argc,char * argv[]) 
{
 
        pthread_t       *threads;
        pthread_attr_t  pta;
        int             iteration,THREADS,ret_count;
	char 		* CLASS;

	double          time_start, time_end;
        struct          timeval tv;                                // variables required for timing
        struct          timezone tz;

        int counter;        
        printf("\n\t\t Objective : To compute the minimum of a list of numbers using read-write locks \n ");
       
         if( argc != 3 )                                                 // check command line arguments
	 {
	    printf("\t\t Very Few Arguments\n ");
            printf("\t\t Syntax : exec <NumElements> <Threads>\n");		
            exit(-1);
         }												// user input
         else {
            NumElements =atoi( argv[1]);
            THREADS = atoi(argv[2]);
         }

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


        partial_list_size = NumElements / NumThreads;                             // partial list size for each thread

        list = (long int *)malloc(sizeof(long int) * NumElements);
        
         
        for(counter = 0 ; counter < NumElements ; counter++)
        {
            srand48((unsigned int)NumElements);							// random number generation
            list[counter] = (rand()%1000)+1.0;
        }
         
        threads = (pthread_t *)malloc(sizeof(pthread_t)*NumThreads);       

        minimum_value = list[0];         

       
        ret_count = pthread_rwlock_init(&rwlock, NULL);						// initialise rwlock
	checkResults("pthread_rwlock_init()\n", ret_count);

	ret_count=pthread_attr_init(&pta);
        checkResults("pthread_attr_init()\n", ret_count);

        pthread_attr_setscope(&pta,PTHREAD_SCOPE_SYSTEM);

	gettimeofday(&tv, &tz);
        time_start = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;

	
	for(counter = 0 ; counter < NumThreads ; counter++)
	{
           ret_count=pthread_create(&threads[counter],&pta,(void *(*) (void *)) find_min_rwlock,(void *) (counter+1));     // call find_min_rwlock function
	   checkResults("pthread_create()\n", ret_count);
	}

	gettimeofday(&tv, &tz);
        time_end = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;


      	
        for(counter = 0 ; counter < NumThreads ; counter++)
	{
           ret_count=pthread_join(threads[counter],NULL);
	   checkResults("pthread_join()\n", ret_count);
        }
        ret_count=pthread_attr_destroy(&pta);
	checkResults("pthread_attr_destroy()\n", ret_count);

        printf("\n\t\t..........................................................................\n");
        printf("\n\t\t Minimum Value found in the Integer list     :  %d",minimum_value);
	printf("\n\t\t Time Taken in Seconds  (T)                  :  %lf Seconds",( time_end - time_start));
        printf("\n\t\t..........................................................................\n");
        
        free(list);
        free(threads);
        return 0;

 }
 void *find_min_rwlock(void * myid )                             // function to find min value
{

     int my_min;
     long int counter;

     int myId = (int)myid;

     my_min = list[(myId-1)*partial_list_size];

     for (counter = ((myId - 1) * partial_list_size); counter <= ((myId * partial_list_size) - 1); counter++){
        if (list[counter] < my_min)
            my_min = list[counter];
     }


    pthread_rwlock_rdlock(&rwlock) ;					// acquire read lock
    if (my_min < minimum_value)
    {
       pthread_rwlock_unlock(&rwlock);
       pthread_rwlock_wrlock(&rwlock);				// if condition is true then only acquire write lock and update variable
       minimum_value = my_min;
    }
    pthread_rwlock_unlock(&rwlock) ;		// unlock rwlock
   
    pthread_exit(NULL);
}

void checkResults(char *string, int rc)                            // check return values from pthread APIs
{
        if (rc)
  {
    printf("Error on : %s, rc=%d",string, rc);
    exit(0);
  }
  return;
}
