/*****************************************************************************



 Example               : omp-matmat-schedule.c



 Objective             : OpenMP program to demonstrate the use of OpenMP schedule
                  	 clause ( LOOP SCHEDULING & PARTITIONING ). Threads share
                  	 the iteration of the loop according to the chunk size.
                  	 This example demonstrates the use of
                  	 PARALLEL for Directive and Schedule clauses
 
 Input                 : a) Number of threads 
		  	 b) Chunk Size 
		  	 c) size of Matrices(i.e Size of Matrix A and Matrix B) ie in terms of
 	  	     	   CLASS where CLASS A :1024; CLASS B: 2048 and CLASS C: 4096



 Output                : Each thread computes the matrix matrix multiplication and
                  	 master prints the  time taken for the computation while
                  	 using different method and different scheduling.	                                            
                                                                        
 Created               : Aug 2011. 
 
 Author                : RarchK                                          


*********************************************************************************/


#include <stdio.h>
#include <sys/time.h>
#include <omp.h>
#include <stdlib.h>


/* Main Program */
main(int argc,char **argv)
{
	int             i,j, k,Noofthreads;
	float         **Matrix_A, **Matrix_B, **Result, **Checkoutput;

	struct timeval  TimeValue_Start;
	struct timezone TimeZone_Start;

	struct timeval  TimeValue_Final;
	struct timezone TimeZone_Final;
	long            time_start, time_end;
        double          time_overhead1,time_overhead2,time_overhead3,time_overhead4;
        int             chunksize;
	int             CLASS_SIZE,MATRIX_SIZE;
	char            *CLASS;

 	printf("\n\t\t---------------------------------------------------------------------------");
        printf("\n\t\t Email : RarchK");
        printf("\n\t\t---------------------------------------------------------------------------");
        printf("\n\t\t Objective : Loop Scheduling and Partitioning (Dense Matrix Computation) \n ");
        printf("\n\t\t Matrix into Matrix Multiplication using ");
        printf("\n\t\t OpenMP  PARALLEL for directive and Schedule clause");
        printf("\n\t\t..........................................................................\n");


         /* Checking for command line arguments */
        if( argc != 4 ){

           printf("\t\t Very Few Arguments\n ");
           printf("\t\t Syntax : exec <Threads> <chunksize> <CLASS>\n");
           printf("\t\t Where  : CLASS = A or B or C\n");
           exit(-1);
        }

        Noofthreads=atoi(argv[1]);
        if ((Noofthreads!=1) && (Noofthreads!=2) && (Noofthreads!=4) && (Noofthreads!=8) && (Noofthreads!= 16) ) {
               printf("\n Number of threads should be 1,2,4,8 or 16 for the execution of program. \n\n");
               exit(-1);
         }

       CLASS = argv[3];
       if( strcmp(CLASS, "A" )==0){
            CLASS_SIZE = 1024;
       }
       else if( strcmp(CLASS, "B" )==0){
            CLASS_SIZE = 2048;
       }
       else if( strcmp(CLASS, "C" )==0){
            CLASS_SIZE = 4096;
       }
       else {
            printf("\n\t\t Error : value of CLASS must be A or B or C \n");
            exit(-1);  
        }
       

        chunksize=atoi(argv[2]);

	MATRIX_SIZE = CLASS_SIZE ;
        printf("\n\t\t Threads           : %d ",Noofthreads);
        printf("\n\t\t Chunk Size        : %d ",chunksize);
        printf("\n\t\t Matrix A Size     : %d ",MATRIX_SIZE );
        printf("\n\t\t Matrix B Size     : %d ",MATRIX_SIZE);

	
	/* Dynamic memory allocation and initialization of Matrix_A Elements */
	Matrix_A = (float **) malloc(sizeof(float *) * MATRIX_SIZE);
	for (i = 0; i < MATRIX_SIZE ; i++) {
		Matrix_A[i] = (float *) malloc(sizeof(float) * MATRIX_SIZE);
		for (j = 0; j < MATRIX_SIZE; j++)
			Matrix_A[i][j] = i + j;
	}

	/* Matrix_B Elements */
	Matrix_B = (float **) malloc(sizeof(float *) * MATRIX_SIZE);
	for (i = 0; i < MATRIX_SIZE; i++) {
		Matrix_B[i] = (float *) malloc(sizeof(float) * MATRIX_SIZE);
		for (j = 0; j < MATRIX_SIZE; j++)
			Matrix_B[i][j] = i + j;
	}

	/* Dynamic Memory Allocation */
	Result = (float **) malloc(sizeof(float *) * MATRIX_SIZE);
	Checkoutput = (float **) malloc(sizeof(float *) * MATRIX_SIZE);

	for (i = 0; i < MATRIX_SIZE; i++) {
		Result[i] = (float *) malloc(sizeof(float) * MATRIX_SIZE);
		Checkoutput[i] = (float *) malloc(sizeof(float) * MATRIX_SIZE);
		for (j = 0; j < MATRIX_SIZE; j++) {
			Result[i][j] = 0.0;
			Checkoutput[i][j] = 0.0;
		}
	}

	
	/* .................................................................................
         This section of Parallelized the for loop without shedule clause.
	....................................................................................*/

	gettimeofday(&TimeValue_Start, &TimeZone_Start);

	omp_set_num_threads(Noofthreads);

	/* OpenMP Parallel For Directive :  Fork a team of threads giving them their own copies of variables */
	#pragma omp parallel for private(j,k) 
	for (i = 0; i < MATRIX_SIZE; i = i + 1)
		for (j = 0; j < MATRIX_SIZE; j = j + 1)
			for (k = 0; k < MATRIX_SIZE; k = k + 1)
				Result[i][j] = Result[i][j] + Matrix_A[i][k] * Matrix_B[k][j];
        /* All threads join master thread and disband */


	 gettimeofday(&TimeValue_Final, &TimeZone_Final);

        /* Calculate the time taken for the computation */
	time_start = TimeValue_Start.tv_sec * 1000000 + TimeValue_Start.tv_usec;
	time_end = TimeValue_Final.tv_sec * 1000000 + TimeValue_Final.tv_usec;
	time_overhead1 = (time_end - time_start)/1000000.0;

 	printf("\n\n\t\t Matrix into Matrix Multiplication using Parallel for directive without schedule clause ...Done ");

	/*........................................................................................

        This section of Parallelized the for loop with schedule (static,chunksize) clause.
	Threads share the iteration of the loop according to the chunk size.
	Loop iterations are divided into pieces of size chunk and then statically assigned to threads.
	........................................................................................*/

	 gettimeofday(&TimeValue_Start, &TimeZone_Start);


        /* OpenMP Parallel For Directive :  Fork a team of threads giving them their own copies of variables */
        #pragma omp parallel for private(j,k) schedule(static,chunksize)
        for (i = 0; i < MATRIX_SIZE; i = i + 1)
                for (j = 0; j < MATRIX_SIZE; j = j + 1)
                        for (k = 0; k < MATRIX_SIZE; k = k + 1)
                                Result[i][j] = Result[i][j] + Matrix_A[i][k] * Matrix_B[k][j];
        /* All threads join master thread and disband */


         gettimeofday(&TimeValue_Final, &TimeZone_Final);

        /* Calculate the time taken for the computation */
        time_start = TimeValue_Start.tv_sec * 1000000 + TimeValue_Start.tv_usec;
        time_end = TimeValue_Final.tv_sec * 1000000 + TimeValue_Final.tv_usec;
        time_overhead2 = (time_end - time_start)/1000000.0;

 	printf("\n\t\t Matrix into Matrix Multiplication using Parallel for directive with schedule(static,chunksize) clause ......Done");

	/*........................................................................................

         This section of Parallelized the for loop with schedule (dynamic,chunksize) clause.
     	 Loop iterations are divided into pieces of size chunk, and dynamically scheduled among
 	the threads; when a thread finishes one chunk, it is dynamically assigned another. 
	 ........................................................................................*/

         gettimeofday(&TimeValue_Start, &TimeZone_Start);

        /* OpenMP Parallel For Directive :  Fork a team of threads giving them their own copies of variables */
        #pragma omp parallel for private(j,k) schedule(dynamic,chunksize)
        for (i = 0; i < MATRIX_SIZE; i = i + 1)
                for (j = 0; j < MATRIX_SIZE; j = j + 1)
                        for (k = 0; k < MATRIX_SIZE; k = k + 1)
                                Result[i][j] = Result[i][j] + Matrix_A[i][k] * Matrix_B[k][j];
        /* All threads join master thread and disband */


         gettimeofday(&TimeValue_Final, &TimeZone_Final);

        /* Calculate the time taken for the computation */
        time_start = TimeValue_Start.tv_sec * 1000000 + TimeValue_Start.tv_usec;
        time_end = TimeValue_Final.tv_sec * 1000000 + TimeValue_Final.tv_usec;
        time_overhead3 = (time_end - time_start)/1000000.0;


 	printf("\n\t\t Matrix into Matrix Multiplication using Parallel for directive with schedule(dynamic,chunksize) clause ......Done ");

	/*........................................................................................

         This section serially do the Matrix Matrix computation .
        ........................................................................................*/

	gettimeofday(&TimeValue_Start, &TimeZone_Start);

	/* Serial Computation */
	for (i = 0; i < MATRIX_SIZE; i = i + 1)
		for (j = 0; j < MATRIX_SIZE; j = j + 1)
			for (k = 0; k < MATRIX_SIZE; k = k + 1)
				Checkoutput[i][j] = Checkoutput[i][j] + Matrix_A[i][k] * Matrix_B[k][j];


	 gettimeofday(&TimeValue_Final, &TimeZone_Final);

        /* Calculate the time taken for the computation */
        time_start = TimeValue_Start.tv_sec * 1000000 + TimeValue_Start.tv_usec;
        time_end = TimeValue_Final.tv_sec * 1000000 + TimeValue_Final.tv_usec;
        time_overhead4 = (time_end - time_start)/1000000.0;



 	printf("\n\t\t Matrix into Matrix Multiplication by serial .....................................Done \n");
  	printf("\n\n\t\t Time in Seconds ( without schedule clause )            : %lf Seconds ",time_overhead1);
  	printf("\n\t\t Time in Seconds ( with schedule (static,chunksize)       : %lf Seconds ",time_overhead2);
  	printf("\n\t\t Time in Seconds ( with schedule (dynamic,chunksize)      : %lf Seconds ",time_overhead3);
  	printf("\n\t\t Time in Seconds ( using serial computation)              : %lf Seconds \n",time_overhead4);
       	printf("\n\t\t   ( T represents the Time taken for computation )");
       	printf("\n\t\t..........................................................................\n");

	/* Freeing Allocated Memory */
	free(Matrix_A);
	free(Matrix_B);
	free(Result);
	free(Checkoutput);

}
