/***************************************************************************

            OpenMP-3.0 Example Codes Beta-v1.0      
        
File          : task-construct-alg-openmp3x.c 
Date          : Aug 2011   
Description   : Simple example program to demonstrates the use of openmp
                new feature "task" and "taskwait" construct for the
		parllelization of recursive algorithum ( Fibnacci series)

OpenMP pragma/
Directive used : #pragma omp parallel  
		 #pragma omp single
		 #pragma omp task

Input         : - Number of threads to use , 
		- Number to specify the upper limit to find the sum 
		  fibnacci numbers in the range 1-Number. 

Output        : Sum of the Fibnacci numbers in the specified range 
**************************************************************************/

/* Header file inclusion */
#include <stdio.h>
#include <omp.h>
#include<stdlib.h>


/*
Description : Function to generate the fibnacci series for given range
	      in parallel using the openmp new feature "task" and
	       perform the synchronization using "taskwait" construct.
@param [n] : Number
*/
int fib(int n)
{
  int x, y;
  if (n<2)
    return n;
  else
    {
	/* creating the two tasks per recursion level */ 
       #pragma omp task shared(x) firstprivate(n)
		//printf("\n My Thread ID	%d",omp_get_thread_num());
       		x=fib(n-1);
       	#pragma omp task shared(y) firstprivate(n)
		//printf("\n My Thread ID	%d",omp_get_thread_num());
       		y=fib(n-2);

	/*The taskwait directive ensures that the two tasks generated in an 
	invocation of fib() are completed (that is. the tasks compute x and y 
	before that invocation of fib() returns.*/
       #pragma omp taskwait
       return x+y;
    }
}/* End of the Function */

/* main function */
int main(int argc , char **argv)
{
 	
	int 	number,numThreads;

 	/* Checking for command line arguments */
        if( argc != 3 ){

           printf("\t\t Very Few Arguments\n ");
           printf("\t\t Syntax : exec <Number> <No. of Threads>\n");
           exit(-1);
        }
	
     	/* Initalizing number of threads & upper limit
	   of fibnacci series */
        number=atoi(argv[1]);
        numThreads=atoi(argv[2]);

  	/* Setting the execution environment */
	omp_set_dynamic(0);
  	omp_set_num_threads(numThreads);

	/* Parallel Region will create the team of threads that will 
	   eventually execute all the Tasks */
  	#pragma omp parallel shared(number)
  	{
    		/* Restricting single thread to do the work
		   i.e Create the tasks */ 		
		#pragma omp single
		{
    		 	printf ("\n Total Threads %d ",omp_get_num_threads());	
			printf ("\n Fibnacci Number fib(%d) = %d\n\n", number, fib(number));
		}
  	}

}/* End of main */
