


/**************************************************************************         			


 Example 1.5           : omp-sumof-elements-reduction.c


 Objective             : Write an OpenMP program to print Sum of Elements of Array
                  	 This example demonstrates the use of
                  	 PARALLEL FOR DIRECTIVE and Reduction clause
		  	 In a Reduction we repeatedly apply a binary operator to a variable and some
             	  	 other value and store the result back in the variable .
 
 Input                 : Size of an Array

                  	 Number of threads


 Output                : The Master thread prints Sum of Elements of the array.	                                            
                                                                        
 Created               : Aug 2011  

 Author                : RarchK                                          


*********************************************************************************/


#include<stdio.h>
#include<omp.h>
#include<stdlib.h>

/* Main Program */
main(int argc , char **argv)
{
	float          *array_A, sum, *checkarray, serialsum;
	int             arraysize, i, k, Noofthreads;

 	printf("\n\t\t---------------------------------------------------------------------------");
        printf("\n\t\t Email : RarchK");
        printf("\n\t\t---------------------------------------------------------------------------");
        printf("\n\t\t Objective : Find the Sum of elements of one-dimensional real array  ");
        printf("\n\t\t using OpenMP Parallel for directive and Reduction Clause  ");
        printf("\n\t\t..........................................................................\n");


	 /* Checking for command line arguments */
        if( argc != 3 ){

           printf("\t\t Very Few Arguments\n ");
           printf("\t\t Syntax : exec <Threads> <array-size> \n");
           exit(-1);
        }

        Noofthreads=atoi(argv[1]);
        if ((Noofthreads!=1) && (Noofthreads!=2) && (Noofthreads!=4) && (Noofthreads!=8) && (Noofthreads!= 16) ) {
               printf("\n\t\t Number of threads should be 1,2,4,8 or 16 for the execution of program. \n\n");
               exit(-1);
         }

        arraysize=atoi(argv[2]);

	/*printf("\n\t\t Enter the size of the array \n");
 	scanf("%d", &arraysize);*/

	if (arraysize <= 0) {
		printf("\n\t\t Positive Number Required\n");
		exit(1);
	}

        printf("\n\t\t Threads     : %d ",Noofthreads);
        printf("\n\t\t Array Size  : %d",arraysize);

	/* Dynamic Memory Allocation */
	array_A = (float *) malloc(sizeof(float) * arraysize);
	checkarray = (float *) malloc(sizeof(float) * arraysize);

	/* Array Elements Initialization */
	for (i = 0; i < arraysize; i++) {
		array_A[i] = i + 5;
		checkarray[i] = array_A[i];
	}

	sum = 0.0;

        /* Set the number of threads */
	omp_set_num_threads(Noofthreads);

	/* OpenMP Parallel For With Reduction Clause :
              In a Reduction we repeatedly apply a binary operator to a variable and some 
              other value and store the result back in the variable .
              Here the Reduction will apply on to the sum variable.Each thread calculate
              its partial sum of the array elements and the final sum will store in sum variable.
	 */
	#pragma omp parallel for reduction(+ : sum)
	for (i = 0; i < arraysize; i++)
          {
		sum = sum + array_A[i];
          }
	/* Serial Calculation */
	serialsum = 0.0;
	for (i = 0; i < arraysize; i++)
		serialsum = serialsum +checkarray[i];


	/* Output Checking */
	if (serialsum != sum) {
		printf("\n\n\t\t The parallel calculation of array sum is different from serial calculation \n");
		exit(-1);
	} else
		printf("\n\n\t\t The parallel calculation of array sum is same with serial calculation \n");

	/* Freeing Memory Which Was Allocated */
	free(checkarray);
	free(array_A);

	printf("\n\t\t The SumOfElements Of The Array Using OpenMP Directives Is %f\n", sum);
	printf("\t\t The SumOfElements Of The Array By Serial Calculation Is  %f\n\n", serialsum);
}
