
/**********************************************************************************

 Example 	  : pthread-vectorvector-multi.c

 Objective        : To compute the vector-vector multiplication with pthreads using 
                    block-striped partitioning for uniform data distribution. Assume 
                    that the vectors are of size n and p is number of processors used 
                    and n is a multiple of p.

 Input            : Vector Size, Number of Threads. Threads must be a factor of vector size.

 Output           : Dot product of the vectors.

 Created	  : Aug 2011

 E-mail      	  : RarchK     	

*************************************************************************************/ 

/* Uses Uniform Data Distribution. */ 

#include <pthread.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>

#define MAXTHREADS 8

pthread_mutex_t mutex_sum = PTHREAD_MUTEX_INITIALIZER;

int   *VecA, *VecB, sum = 0, dist;

/* Thread callback function */
void * doMyWork(int myId)
{
	
	int counter, mySum = 0;

	printf("\n %d: I am taking the interval: %d - %d.", myId, ((myId - 1) * dist), ((myId * dist) - 1));
		/*calculating local sum by each thread */	
		for (counter = ((myId - 1) * dist); counter <= ((myId * dist) - 1); counter++)
			mySum += VecA[counter] * VecB[counter];
	
		
		printf("\n %d: My Local Sum: %d.", myId, mySum);
	
		/*updating global sum using mutex lock */
		pthread_mutex_lock(&mutex_sum);
	
		sum += mySum;
	
		pthread_mutex_unlock(&mutex_sum);
	
		
		return;
	
} 
/*Main function start */
main(int argc, char *argv[])
{
	
		/*variable declaration */
		int ret_count;
		pthread_t * threads;

		pthread_attr_t pta;
	
	        double time_start, time_end, diff;
                struct timeval tv;
                struct timezone tz;
	
		int   counter, NumThreads, VecSize;
	
		printf("\n\t\t---------------------------------------------------------------------------");
                printf("\n\t\t Centre for Development of Advanced Computing (C-DAC)");
                printf("\n\t\t Email : RarchK");
                printf("\n\t\t---------------------------------------------------------------------------");
                printf("\n\t\t Objective : Vector-Vector Multiplication");
                printf("\n\t\t Compute the vector-vector multiplication using block-striped partitioning ");
		printf("\n\t\t for uniform data distribution");
                printf("\n\t\t..........................................................................\n");

	
		if (argc != 3)
                {
		printf(" Missing Arguments: exe <VectorSize> <NumThreads> \n");
		return;
	        } 
		
		NumThreads = abs(atoi(argv[2]));
	
		VecSize = abs(atoi(argv[1]));
	
		
		if (VecSize == 0)
                {
                  printf("\nVector Size is assumed as 100");
	       	  VecSize = 100;
                }
	
		if (NumThreads == 0)
                {
                  printf("\nNumber of Threads are assumed as 4");
		  NumThreads =4;
                }
	
		printf("\n Size of Vector: %d.", VecSize);
		printf("\n Number of Threads: %d.", NumThreads);
	
		
		if (NumThreads > MAXTHREADS)
                {
			printf("\n Number of threads should be less than or equal to 8. Aborting.\n");
			return ;
	        } 
		if (VecSize % NumThreads != 0)
                {
		printf("\n Number of threads not a factor of vector size. Aborting.\n");
			return ;
	        } 
		/*Memory allocation for vectors */
		VecA = (int *) malloc(sizeof(int) * VecSize);
	
		VecB = (int *) malloc(sizeof(int) * VecSize);

		pthread_attr_init(&pta);
	
		
		threads = (pthread_t *) malloc(sizeof(pthread_t) * NumThreads);
	
		
		dist = VecSize / NumThreads;
	
		/*Vector A and Vector B intialization */
		for (counter = 0; counter < VecSize; counter++) 
                {
			VecA[counter] = 2;
			VecB[counter] = 3;
	        }
		
               /*calculating start time */
                gettimeofday(&tv, &tz);
                time_start = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;

		/*Thread Creation */
		for (counter = 0; counter < NumThreads; counter++)
		{
			ret_count=pthread_create(&threads[counter], &pta, (void *(*) (void *)) doMyWork, (void *) (counter + 1));
			if(ret_count)
			{
				printf("\n ERROR: Return code from pthread_create() function is %d",ret_count);
				exit(-1);
			}

		}
		/*joining thread */
		for (counter = 0; counter < NumThreads; counter++)
		{
			ret_count=pthread_join(threads[counter], NULL);
			if(ret_count)
    			{
				printf("\n ERROR : Return code from pthread_join() is %d ",ret_count);
				exit(-1);
    			}
		}
		/*calculating end time*/
                gettimeofday(&tv, &tz);
                time_end = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
	
		
		printf("\n The Sum is: %d.", sum);
                printf("\n Time in Seconds (T)  :  %lf\n", time_end - time_start);

		ret_count=pthread_attr_destroy(&pta);
		if(ret_count)
    		{
			printf("\n ERROR : Return code from pthread_attr_destroy() is %d ",ret_count);
			exit(-1);
    		}
	
		return;
	
} 


