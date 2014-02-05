


/*****************************************************************************


 Example 1.8           : omp-shared-private-data.c


 Objective             : Write an OpenMP program ( Managing Private and Shared 
                         Data) to find Sum Of Elements
                         of One-Dimensional real array.
                         This example demonstrates the use of OpenMP
                         Parallel For Directive, Private & Shared Clause
                         Critical Section.

 Input                 : Size of an array
                   
                         Number of threads 


 Output                : Sum of array elements using the following three different methods.
			 1) Using Shared Data
			 2) Using Private Data
			 3) Performing Serial computation.	                                            
                                                                        
 Created               :Aug 2011. 
       
  Author                : RarchK                                          


*********************************************************************************/



#include<stdio.h>
#include<omp.h>
#include<stdlib.h>

/* Main Program */
main(int argc , char **argv)
{
	double          *Array, *Check, serial_sum,sum_private, sum,my_sum;
	int             array_size, i,threadid,tval,Noofthreads;


        printf("\n\t\t---------------------------------------------------------------------------");
        printf("\n\t\t Email : RarchK");
        printf("\n\t\t---------------------------------------------------------------------------");
        printf("\n\t\t Objective : Managing Private and Shared Data");
        printf("\n\t\t Find the Sum of elements of one-dimensional real array.  ");
        printf("\n\t\t OpenMP Parallel for directive , Private and Shared Clause and Critical section is used ");
        printf("\n\t\t..........................................................................\n");

	 /* Checking for command line arguments */
        if( argc != 3 ){

           printf("\t\t Very Few Arguments\n ");
           printf("\t\t Syntax : exec <Threads> <array-size> \n");
           exit(-1);
        }

        Noofthreads=atoi(argv[1]);
        if ((Noofthreads!=1) && (Noofthreads!=2) && (Noofthreads!=4) && (Noofthreads!=8) && (Noofthreads!= 16) ) {
               printf("\n Number of threads should be 1,2,4,8 or 16 for the execution of program. \n\n");
               exit(-1);
         }

        array_size=atoi(argv[2]);
          
	/*printf("\n\t\t Enter the size of the array\n");
	scanf("%d", &array_size);*/

       /* Array Size should be positive integer */
	if (array_size <= 0) {
		printf("\n\t\t Array Size Should Be Of Positive Value ");
		exit(1);
	}

        printf("\n\t\t Threads     : %d ",Noofthreads); 
        printf("\n\t\t Array Size  : %d ",array_size);

	/* Dynamic Memory Allocation */
	Array = (double *) malloc(sizeof(double) * array_size);
	Check = (double *) malloc(sizeof(double) * array_size);

	/* Array Elements Initialization */
	for (i = 0; i < array_size; i++) {
		Array[i] = i * 5;
		Check[i] = Array[i];
	}

	sum=0.0;
 
        /* set the number of threads */
	omp_set_num_threads(Noofthreads);

   	/*  ....................................................................................
       	In this section each for loop is parallelized , each thread compute partial sum of
       	its block . The variable my_sum  is shared by all the threads based on OpenMP default 
       	shared rule, so there is a data race condition on the  my_sum, more than one thread
       	updating the same memory location so the result produce by this may be incorrect.
    	......................................................................................... */
 
	/* OpenMP Parallel For Directive And Critical Section */
         #pragma omp parallel shared(sum) 
        {
        	my_sum=0.0;        
          	#pragma omp for 
  		for (i = 0; i < array_size; i++) 
                      my_sum = my_sum + Array[i];   /* Data races occure */
		#pragma omp critical
		sum = sum + my_sum;
           } /* End of parallel region */



	 /*  ....................................................................................
       	In this section each for loop is parallelized , each thread compute partial sum of
       	its block . The variable my_sum  is private. The each thread will update their own
       	copy of my_sum variable.
    	 ......................................................................................... */

           sum_private = 0;
	/* OpenMP Parallel For Directive And Private Clause Critical Section */
         #pragma omp parallel private(my_sum) shared(sum)
        {
                my_sum=0.0;
                #pragma omp for
                for (i = 0; i < array_size; i++)
                      my_sum = my_sum + Array[i];
                #pragma omp critical
                sum_private = sum_private + my_sum;
           } /* End of parallel region */

      
        
	 /*  ....................................................................................
      	The serial computation of the sum of the ealements of the array for output verification.
	 ......................................................................................... */
	serial_sum = 0.0;
	/* Serail Calculation */
	for (i = 0; i < array_size; i++)
		serial_sum = serial_sum + Check[i];


          printf("\n\n\t\t The compuation of the sum of the elements of the Array ........... Done "); 

	/* Freeing Memory */
	free(Check);
	free(Array);

	printf("\n\n\t\t The SumOfElements Of The Array Using OpenMP Directives without Private Clause : %lf", sum);
	printf("\n\t\t The SumOfElements Of The Array Using OpenMP Directives and Private Clause Is    : %lf", sum_private);
	printf("\n\t\t The SumOfElements Of The Array By Serial Calculation Is                         : %lf\n\n", serial_sum);
        printf("\n\t\t..........................................................................\n");
}
