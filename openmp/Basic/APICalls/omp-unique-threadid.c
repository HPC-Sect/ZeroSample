

/*****************************************************************************************



 Example 1.1           : omp-unique-threadid.c


 Objective             :  Write a simple OpenMP program to print unique number for
                  	  each thread started by the #pragma parallel.
                  	  This example demonstrates the use of OpenMP PARALLEL
                  	  Directive and omp_get_thread_num() call
 
 Input                 : Set the number of threads to use by means of the OMP_NUM_THREADS environment variable.
		   
		   		For C shell use command :
		
					setenv OMP_NUM_THREADS 4 
	
		   		For bash shell use command :
		
					export OMP_NUM_THREADS=4 
 

 Output                : Each thread prints its thread id.	                                            
                                                                        
 Created               : Aug 2011 
       
 
 Author                : RarchK                                          


******************************************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

/* Main Program */
main(int argc,char **argv)
{
	int             threadid,Noofthreads;


 	printf("\n\t\t---------------------------------------------------------------------------");
        printf("\n\t\t Email : RarchK");
        printf("\n\t\t---------------------------------------------------------------------------");
        printf("\n\t\t Objective : OpenMP program to print unique thread identifier for ");
	printf("\n\t\t each thread using OpenMP PARALLEL directive.");
        printf("\n\t\t..........................................................................\n");

	printf("\n\n\t\t Master thread  prints this before forking the team of thread \n");

        /* Set the number of threads */
	/*omp_set_num_threads(4);*/

	/* OpenMP Parallel Construct : Fork a team of threads */ 
	#pragma omp parallel private(threadid)
	{
                /* Obtain the thread id */
		threadid = omp_get_thread_num();

		/* Each Thread Prints Its Threadid */
		printf("\n\t\t My thread id is : %d\n", threadid);
	
	} /* All thread join Master thread */

	printf("\n\t\t Master thread  prints this after the end parallel region \n \n");
}
