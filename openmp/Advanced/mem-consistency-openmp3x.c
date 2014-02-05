/************************************************************************
                        

             OpenMP-3.0 Example Codes Beta-v1.0	
	
File          : mem-consistency-openmp3x.c 

Date 	      : Aug 2011 

Description   : Simple example program to demonstrates the importance to 
		maintain the memory consistency.

 		In the example program , at Print 1, the value of x could be 
		either 2 or 5,depending on the timing of the threads, and the 
		implementation of the assignment to x. There are two reasons 
      		that the value at Print 1 might not be 5. First,Print 1 might 
		be executed before the assignment to x is executed. Second, even 
		if Print 1 is executed after the assignment,the value 5 is not 
		guaranteed to be seen by thread 1 because a flush may not have 
		been executed by thread 0 since the assignment.

		The barrier after Print 1 contains implicit flushes on all 
		thread as well as a thread synchronization, so the programmer
 		is guaranteed that the value 5 will be printed by both Print 2
 		and Print 3.

Input         : None 

Output        : Value of the variable x 
 

*****************************************************************************************/

/* Header file inclusion  */
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

/* Main Program */
 int main(int argc,char **argv){
   	
	int x,numThreads;

	
	 /* Checking for command line arguments */
        if( argc != 2 ){

           printf("\t\t Very Few Arguments\n ");
           printf("\t\t Syntax : exec  <No. of Threads>\n");
           exit(-1);
        }

        /* Initalizing Number of Nodes in the List and 
           Number of threads */
        numThreads =atoi(argv[1]);

	 x = 2;
	
	/* Setting the number of threads */
	omp_set_num_threads(numThreads);

   	/* Create the parallel region and set the no. of threads to 2 */
   	#pragma omp parallel shared(x)
   	{
   
		if (omp_get_thread_num() == 0) {
         		x = 5;
			//sleep(10);
      		} else {
      			/* Print 1: the following read of x has a race */
        		printf("\n\t\t 1: Thread# %d: x = %d\n", omp_get_thread_num(),x );
      		}
     
	 	#pragma omp barrier /* Perform the synchronization */

      		if (omp_get_thread_num() == 0) {
      			/* Print 2 */
      			printf("\n\t\t 2: Thread# %d: x = %d\n", omp_get_thread_num(),x );
      		} else {
      			/* Print 3 */
      			printf("\n\t\t 3: Thread# %d: x = %d\n", omp_get_thread_num(),x ); 
      		}
   	}

   
	return 0;
}
