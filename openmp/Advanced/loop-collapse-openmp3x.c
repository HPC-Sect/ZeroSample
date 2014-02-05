/****************************************************************************

                 OpenMP-3.0 Example Codes Beta-v1.0      
        
File          : loop-collapse-openmp3x.c  

Date          : Aug 2011 

Description   : Simple example program to demonstrates the use of openmp
		new feature "collapse" clause. 
		In this example the iteration space over the loop index i 
		and j is collapsed into the single large iteration space 
		which then executed by the team of threads.

OpenMP pragma/
Directive used : #pragma omp parallel for
		- collapse clause 

Input         : None 
Output        :  Value of count variable 
***************************************************************************/

/* Header file inclusion */
#include <stdio.h>
#include<omp.h>

/* Defined Parameters */
#define N 100 
#define M 100 
#define NUMTHREADS 4

/* Function declaration */
void loopParCollapse();

/* main function */
int main(int argc, char *argv[]) {
	
	/* Function calling */
	loopParCollapse();
	 

} /* End of main() */

/*
Description: Parallelize Nested loop using Collapse clause (openmp-3.0). Collapse clause
	     reduce the iterations over i & j in sigle iteration space which is executed 
	     by the threads in the team.  
@param : None
*/

void loopParCollapse()
{

        int i,j,count=0;
	double start_time, end_time;
	
	/* Set the number of threads */
	omp_set_num_threads(NUMTHREADS);	

	/* Create the parallel region and collapse clause reduce 
	   the iteration over i & j in single iteration space which 
	   is then executed by thread team */
        #pragma omp parallel for collapse(2)
        for ( i = 1 ; i <=N  ; i++ ) {
                for (j=1; j<=M ;j++ ) {
                #pragma omp atomic         /* Mutual exclusion point */
                                count++;
                }
        } /* end of the parallel region */

        printf(" Value of count( Collapse Clause)  : %d \n", count);
} /* End of the function */

