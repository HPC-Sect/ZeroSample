

/*****************************************************************************



 Example 1.4           : omp-sumof-elements.c


 Objective             : Write an OpenMP program to find Sum Of Elements
                         of One-Dimensional real array.
                         This example demonstrates the use of OpenMP
                         Parallel For Directive And Critical Section.
 
 Input                 : Size of an array
                   
                         Number of threads 

 Output                : Sum of array elements	                                            
                                                                        
 Created               :  Aug 2011  
       
 
 Author                : RarchK                                          


*********************************************************************************/



#include<stdio.h>
#include<omp.h>
#include<stdlib.h>

/* Main Program */
main(int argc , char **argv)
{
	double          *Array, *Array1, *Check, serial_sum, sum;
	int             array_size, i,threadid,tval,Noofthreads;


        printf("\n\t\t---------------------------------------------------------------------------");
        printf("\n\t\t Email : RarchK");
        printf("\n\t\t---------------------------------------------------------------------------");
        printf("\n\t\t Objective :Find the Sum of elements of one-dimensional real array.  ");
        printf("\n\t\t OpenMP Parallel for directive and Critical Section are used ");
        printf("\n\t\t..........................................................................\n");

	 /* Checking for command line arguments */
        if( argc != 3 ){

           printf("\t\t Very Few Arguments\n ");
           printf("\t\t Syntax : exec <Threads> <array-size>\n");
           exit(-1);
        }

        Noofthreads=atoi(argv[1]);
        if ((Noofthreads!=1) && (Noofthreads!=2) && (Noofthreads!=4) && (Noofthreads!=8) && (Noofthreads!= 16) ) {
               printf("\n Number of threads should be 1,2,4,8 or 16 for the execution of program. \n\n");
               exit(-1);
         }

          
        array_size=atoi(argv[2]);
	/*printf("\n\t\t Enter the size of the array\n");
	scanf("%d", &array_size); */

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
 
	/* OpenMP Parallel For Directive And Critical Section */
	#pragma omp parallel for
	for (i = 0; i < array_size; i++) 
          {
              /* printf("the thread num and its iteration is %d %d \n",omp_get_thread_num(),i); */
		#pragma omp critical
		sum = sum + Array[i];
           } /* End of parallel region */

	serial_sum = 0.0;
	/* Serail Calculation */
	for (i = 0; i < array_size; i++)
		serial_sum = serial_sum + Check[i];


	if (serial_sum == sum)
		printf("\n\n\t\t The Serial And Parallel Sums Are Equal\n");
	else {
		printf("\n\\nt\t The Serial And Parallel Sums Are UnEqual\n");
		exit(1);
	}

	/* Freeing Memory */
	free(Check);
	free(Array);

	printf("\n\t\t The SumOfElements Of The Array Using OpenMP Directives Is %lf\n", sum);
	printf("\t\t The SumOfElements Of The Array By Serial Calculation Is %lf\n\n", serial_sum);
        printf("\n\t\t..........................................................................\n");
}
