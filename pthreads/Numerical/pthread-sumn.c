
/***********************************************************************

 Example              : pthread-sumn.c

 Objective 	      : To calculate the sum of first n integers.
                        Demonstrates the usage of:

                             pthread_create()
                             pthread_join()
                             pthread_mutex_t
                             pthread_mutex_lock()
                             pthread_mutex_unlock()
			     pthread_attr_init()
			     pthread_attr_destroy()
 
 Input                 : Number of threads.

 Output                : Sum of the first Input number of threads.	                                            
                                                                        
 Created               : Aug 2011
 
 E-mail                : RarchK                                          


*******************************************************************************/
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>

#define MAXTHREADS 8

pthread_mutex_t mutex_sum = PTHREAD_MUTEX_INITIALIZER;

int  sum = 0;



void  * Addup(int Id)
{
	
	pthread_mutex_lock(&mutex_sum);
	
	{
		
			sum += Id;
		
	} 
		
		pthread_mutex_unlock(&mutex_sum);
	
		return;
	
} 


main(int argc, char *argv[])
{
	
		int ret_count;		
		pthread_t * threads;

		pthread_attr_t pta;
	
		
		int i, NumThreads;

		printf("\n\t\t---------------------------------------------------------------------------");
  		printf("\n\t\t Centre for Development of Advanced Computing (C-DAC)");
  		printf("\n\t\t Email : RarchK");
  		printf("\n\t\t---------------------------------------------------------------------------");
  		printf("\n\t\t Objective : \n To calculate the sum of first n integers.");
  		printf("\n\t\t..........................................................................\n");

	
		
		if (argc != 2) 
                {
		
			printf(" Missing Argument: Number of Threads.\n");
			return;
	         } 
		
		NumThreads = abs(atoi(argv[1]));
	
		if (NumThreads == 0)
                {
                        printf("\nNumber of Threads are Assumed as 4");
		
			NumThreads = 4;
                }

                if (NumThreads > MAXTHREADS )
		{

			printf("\n\t Error : Number of threads should be less than 8\n");
			exit(-1);
		}
	
		printf("\n The Number of threads is %d.\n", NumThreads);
	
		
		threads = (pthread_t *) malloc(sizeof(pthread_t) * NumThreads);

		ret_count=pthread_attr_init(&pta);
      		if (ret_count)
      		{
         		printf("ERROR; return code from pthread_attr_init() is %d\n", ret_count);
         		exit(-1);
      		}		

	
		
		for (i = 0; i < NumThreads; i++)
		{

			ret_count=pthread_create(&threads[i], &pta, (void *(*) (void *)) Addup, (void *) (i + 1));
      			if (ret_count)
      			{
         			printf("ERROR; return code from pthread_create() is %d\n", ret_count);
         			exit(-1);
      			}
		}
		
		for (i = 0; i < NumThreads; i++)
		{

			ret_count=pthread_join(threads[i], NULL);
			if (ret_count)
      			{
         			printf("ERROR; return code from pthread_join() is %d\n", ret_count);
         			exit(-1);
      			}
		}

	
		ret_count=pthread_attr_destroy(&pta);
      		if (ret_count)
      		{
         		printf("ERROR; return code from pthread_attr_destroy() is %d\n", ret_count);
        		exit(-1);
      		}
		
		printf("\n Number of threads: %d. Sum: %d.\n", NumThreads, sum);
	
		return;
	
} 
