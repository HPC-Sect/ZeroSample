
/*****************************************************************************




 Example               : omp-matmat-mult.c


 Objective             : Write an OpenMP Program of Matrix Matrix Multiplication
                  	 and measure the performance.
                  	 This example demonstrates the use of
                  	 PARALLEL for Directive and Private clause

 
 Input                 : a) Number of threads 
		  	 b) Size of matrices (numofrows and noofcols of A and noofrows
                  	 and Noofcols of B )

 Output                : Each thread computes the matrix matrix multiplication and
                  	 master thread prints the final result matrix and also time taken for
		  	 the computation.	                                            
                                                                        

 Author                : RarchK                                          


*********************************************************************************/




#include <stdio.h>
#include <sys/time.h>
#include <omp.h>
#include <stdlib.h>



/* Main Program */
main(int argc,char **argv)
{
	int             NoofRows_A, NoofCols_A, NoofRows_B, NoofCols_B, i,j, k,Noofthreads;
	float         **Matrix_A, **Matrix_B, **Result, **Checkoutput, flops;

	struct timeval  TimeValue_Start;
	struct timezone TimeZone_Start;

	struct timeval  TimeValue_Final;
	struct timezone TimeZone_Final;
	long            time_start, time_end;
        double          time_overhead;

 	printf("\n\t\t---------------------------------------------------------------------------");
        printf("\n\t\t Email : RarchK");
        printf("\n\t\t---------------------------------------------------------------------------");
        printf("\n\t\t Objective : Dense Matrix Computations (Floating Point Operations)\n ");
        printf("\n\t\t Matrix into Matrix Multiplication using ");
        printf("\n\t\t OpenMP one PARALLEL for directive and Private Clause;");
        printf("\n\t\t..........................................................................\n");

         /* Checking for command line arguments */
        if( argc != 6 ){

           printf("\t\t Very Few Arguments\n ");
           printf("\t\t Syntax : exec <Threads> <NoOfRows A> <NoOfCols A>  <NoOfRows B> <NoOfCols B>\n");
           exit(-1);
        }

        Noofthreads=atoi(argv[1]);
        if ((Noofthreads!=1) && (Noofthreads!=2) && (Noofthreads!=4) && (Noofthreads!=8) && (Noofthreads!= 16) ) {
               printf("\n Number of threads should be 1,2,4,8 or 16 for the execution of program. \n\n");
               exit(-1);
         }

	NoofRows_A=atoi(argv[2]);
	NoofCols_A=atoi(argv[3]);
	NoofRows_B=atoi(argv[4]);
	NoofCols_B=atoi(argv[5]);

/*	printf("\n\t\t Read The Matrix Size Noofrows and Colums of Matrix A and B  \n");
	scanf("%d%d%d%d", &NoofRows_A, &NoofCols_A, &NoofRows_B, &NoofCols_B);*/


        printf("\n\t\t Threads        : %d ",Noofthreads);
        printf("\n\t\t Matrix A Size  : %d X %d ", NoofRows_A,NoofCols_A);
        printf("\n\t\t Matrix B Size  : %d X %d ", NoofRows_B,NoofCols_B);

	/* The NoofRows And NoofCols Should Be Of Positive integer */ 
	if (NoofRows_A <= 0 || NoofCols_A <= 0 || NoofRows_B <= 0 || NoofCols_B <= 0) {
		printf("\n\t\t The NoofRows And NoofCols Should Be Of Positive Sign\n");
		exit(1);
	}
	/* Checking For Necessary Condition */
	if (NoofCols_A != NoofRows_B) {
		printf("\n\t\t Matrix Matrix Computation Is Not Possible \n");
		exit(1);
	}
	
	/* Dynamic memory allocation and initialization of Matrix_A Elements */
	Matrix_A = (float **) malloc(sizeof(float *) * NoofRows_A);
	for (i = 0; i < NoofRows_A; i++) {
		Matrix_A[i] = (float *) malloc(sizeof(float) * NoofCols_A);
		for (j = 0; j < NoofCols_A; j++)
			Matrix_A[i][j] = i + j;
	}

	/* Matrix_B Elements */
	Matrix_B = (float **) malloc(sizeof(float *) * NoofRows_B);
	for (i = 0; i < NoofRows_B; i++) {
		Matrix_B[i] = (float *) malloc(sizeof(float) * NoofCols_B);
		for (j = 0; j < NoofCols_B; j++)
			Matrix_B[i][j] = i + j;
	}

	/* Dynamic Memory Allocation */
	Result = (float **) malloc(sizeof(float *) * NoofRows_A);
	Checkoutput = (float **) malloc(sizeof(float *) * NoofRows_A);

	for (i = 0; i < NoofRows_A; i++) {
		Result[i] = (float *) malloc(sizeof(float) * NoofCols_B);
		Checkoutput[i] = (float *) malloc(sizeof(float) * NoofCols_B);
		for (j = 0; j < NoofCols_B; j++) {
			Result[i][j] = 0.0;
			Checkoutput[i][j] = 0.0;
		}
	}

	gettimeofday(&TimeValue_Start, &TimeZone_Start);

	omp_set_num_threads(Noofthreads);

	/* OpenMP Parallel For Directive :  Fork a team of threads giving them their own copies of variables */
	#pragma omp parallel for private(j,k)
	for (i = 0; i < NoofRows_A; i = i + 1)
		for (j = 0; j < NoofCols_B; j = j + 1)
			for (k = 0; k < NoofCols_A; k = k + 1)
				Result[i][j] = Result[i][j] + Matrix_A[i][k] * Matrix_B[k][j];
        /* All threads join master thread and disband */


	 gettimeofday(&TimeValue_Final, &TimeZone_Final);

        /* Calculate the time taken for the computation */
	time_start = TimeValue_Start.tv_sec * 1000000 + TimeValue_Start.tv_usec;
	time_end = TimeValue_Final.tv_sec * 1000000 + TimeValue_Final.tv_usec;
	time_overhead = (time_end - time_start)/1000000.0;


	/* Serial Computation */
	for (i = 0; i < NoofRows_A; i = i + 1)
		for (j = 0; j < NoofCols_B; j = j + 1)
			for (k = 0; k < NoofCols_A; k = k + 1)
				Checkoutput[i][j] = Checkoutput[i][j] + Matrix_A[i][k] * Matrix_B[k][j];

	for (i = 0; i < NoofRows_A; i = i + 1)
		for (j = 0; j < NoofCols_B; j = j + 1)
			if (Checkoutput[i][j] == Result[i][j])
				continue;
			else {
				printf("\n\t\t There Is A Difference In Parallel Calculation \n");
				exit(1);
			}


 	printf("\n\n\t\t Matrix into Matrix Multiplication using Parallel for directive......Done \n");
  	printf("\n\t\t Time in Seconds (T)        : %lf Seconds \n",time_overhead);
       	printf("\n\t\t   ( T represents the Time taken for computation )");
       	printf("\n\t\t..........................................................................\n");

	/* Freeing Allocated Memory */
	free(Matrix_A);
	free(Matrix_B);
	free(Result);
	free(Checkoutput);

}
