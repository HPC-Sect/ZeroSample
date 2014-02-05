



/*****************************************************************************


 Example               : omp-hello-world.c


 Objective             : OpenMP program to print "Hello World"
                  	 This example demonstrates the use of
                  	 omp_get_thread_num()
                         omp_get_num_threads() calls
 
 Input                 : Set the number of threads to use by means of the OMP_NUM_THREADS environment variable.

                   		For C shell use command :

                        		setenv OMP_NUM_THREADS 4

                   		For bash shell use command :

                       			 export OMP_NUM_THREADS=4.


 Output                : Each thread prints a message "Hello World" and its
                  	 identifier.	                                            
                                                                        
 Created               :  Aug 2011

 
 
 Author                : RarchK                                          


*********************************************************************************/



#include<stdio.h>
#include<stdlib.h>
#include<omp.h>

/* Main Program */
int main(int argc , char **argv)
{
	int             Threadid, Noofthreads;


        printf("\n\t\t---------------------------------------------------------------------------");
        printf("\n\t\t Email : RarchK");
        printf("\n\t\t---------------------------------------------------------------------------");
        printf("\n\t\t Objective : OpenMP program to print \"Hello World\" using OpenMP PARALLEL directives\n ");
        printf("\n\t\t..........................................................................\n");

 
 	/* Set the number of threads */
	/* omp_set_num_threads(4); */ 
 	/* OpenMP Parallel Construct : Fork a team of threads */
#pragma omp parallel private(Threadid)
{
 		/* Obtain the thread id */
		Threadid = omp_get_thread_num();
		printf("\n\t\t Hello World is being printed by the thread : %d\n", Threadid);

		/* Master Thread Has Its Threadid 0 */
		if (Threadid == 0) {
			Noofthreads = omp_get_num_threads();
			printf("\n\t\t Master thread printing total number of threads for this execution are : %d\n", Noofthreads);
		}
}/* All thread join Master thread */

	return 0;
}
