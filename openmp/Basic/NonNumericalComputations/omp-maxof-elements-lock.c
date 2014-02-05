/***********************************************************************************



  Example 3.2 : omp-maxof-elements-lock.c
 
  Objective   : Write an  OpenMP program to print Largest of an element
                in an array
                This example demonstrates the use of
                omp_init_lock(),omp_set_lock(),omp_unset_lock(),
                omp_destroy_lock() which are known as LOCK functions.


  Input       : Number of threads 
                Number of elements of the array Input
               

  Output      : Each thread checks with its available iterations and finally
                Master thread prints the maximum value in the array,Time taken
		to find the Max Element and also the threads .	
	                                                                                                                    
  Created     :Aug 2011.        

  
  Author      : RarchK                                          
****************************************************************************/

#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <sys/time.h>
#define MINUS_INFINITY -9999
#define MAXIMUM_VALUE 65535

/* Main Program */

main(int argc,char **argv)
{
	int            *array, i, Noofelements, cur_max, current_value,Noofthreads;
	struct timeval  TimeValue_Start;
        struct timezone TimeZone_Start;

        struct timeval  TimeValue_Final;
        struct timezone TimeZone_Final;
        long            time_start, time_end;
        double          time_overhead;

	omp_lock_t     MAXLOCK;

	printf("\n\t\t---------------------------------------------------------------------------");
        printf("\n\t\t Email : RarchK");
        printf("\n\t\t---------------------------------------------------------------------------");
        printf("\n\t\t Objective : Finding Maximum element of an Array ");
        printf("\n\t\t This example demonstrates the use of OpenMP Lock routines such as : omp_init_lock()");
        printf("\n\t\t omp_set_lock(),omp_unset_lock(), omp_destroy_lock(). ");
        printf("\n\t\t..........................................................................\n");

         /* Checking for command line arguments */
        if( argc != 3 ){

           printf("\t\t Very Few Arguments\n ");
           printf("\t\t Syntax : exec <Threads>  <No. of elements> \n");
           exit(-1);
        }

        Noofthreads=atoi(argv[1]);
        if ((Noofthreads!=1) && (Noofthreads!=2) && (Noofthreads!=4) && (Noofthreads!=8) && (Noofthreads!= 16) ) {
               printf("\n Number of threads should be 1,2,4,8 or 16 for the execution of program. \n\n");
               exit(-1);
         }

        Noofelements=atoi(argv[2]);
/*	printf("\n\t\t Enter the number of elements\n");
	scanf("%d", &Noofelements);*/
 
        printf("\n\t\t Threads : %d",Noofthreads);
        printf("\n\t\t Number of elements in Array : %d \n ",Noofelements);

	if (Noofelements <= 0) {
		printf("\n\t\t The array elements cannot be stored\n");
		exit(1);
	}
	/* Dynamic Memory Allocation */

	array = (int *) malloc(sizeof(int) * Noofelements);

	/* Allocating Random Number To Array Elements */

	srand(MAXIMUM_VALUE);
	for (i = 0; i < Noofelements; i++)
		array[i] = rand();

	if (Noofelements == 1) {
		printf("\n\t\t The Largest Element In The Array Is %d", array[0]);
		exit(1);
	}

	gettimeofday(&TimeValue_Start, &TimeZone_Start);
        /* set the no. of threads */
	omp_set_num_threads(Noofthreads);
 
        /* Initialize the lock variable */
	omp_init_lock(&MAXLOCK);
	cur_max = MINUS_INFINITY;
	
	/* OpenMP Parallel For Directive And Lock Functions : Fork the team of threads */
	#pragma omp parallel for
	for (i = 0; i < Noofelements; i = i + 1) {
		if (array[i] > cur_max) {
			omp_set_lock(&MAXLOCK);
			if (array[i] > cur_max) 
				cur_max = array[i];
			omp_unset_lock(&MAXLOCK);
		}
	} /* End of the parallel section */

	/* Destroying The Lock */
	omp_destroy_lock(&MAXLOCK);
        gettimeofday(&TimeValue_Final, &TimeZone_Final);

        /* calculate the timing for the computation */
        time_start = TimeValue_Start.tv_sec * 1000000 + TimeValue_Start.tv_usec;
        time_end = TimeValue_Final.tv_sec * 1000000 + TimeValue_Final.tv_usec;
        time_overhead = (time_end - time_start)/1000000.0;



	/* Serial Calculation */
	current_value = array[0];
	for (i = 1; i < Noofelements; i++)
		if (array[i] > current_value)
			current_value = array[i];

	/* Checking For Output Validity */

	if (current_value == cur_max)
		printf("\n\n\t\t The Max Value Is Same For Serial And Using Parallel OpenMP Directive\n");
	else {
		printf("\n\n\t\t The Max Value Is Not Same In Serial And Using Parallel OpenMP Directive\n");
		exit(-1);
	}

	/* Freeing Allocated Memory */

	free(array);

	printf("\n\t\t The Largest Number Of The Array Is %d\n", cur_max);
        printf("\n\t\t Time in Seconds (T)        : %lf Seconds \n",time_overhead);
        printf("\n\t\t..........................................................................\n");
}
