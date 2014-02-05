

/*****************************************************************************




 Example               : omp-matvect-mult.c

 Objective             : Write an OpenMP program for Matrix vector multiplication
                  	 This example demonstrates the use of PARALLEL FOR Directive
                  	 and Private clause .It uses loop work-sharing
                  	 construct i.e. distribution of columns of matrix

 
 Input                 : a) Number of threads  
		  	 b) Size of matrix (rows and columns) 
                  	 c) Vector size

 Output                : Each thread computes the assigned row vector multiplication
                  	 and master thread prints the final output and time taken .
	                                            
                                                                        
 Created               :Aug 2011  
       
 
 Author                : RarchK                                          


*********************************************************************************/


#include <stdio.h>
#include<sys/time.h>
#include <omp.h>
#include <stdlib.h>


/* Main Program */
main(int argc,char **argv)
{
	int             NoofRows, NoofCols, Vectorsize, i, j,Noofthreads;
	/*float         **Matrix, *Vector, *Result, *Checkoutput;*/
	double **Matrix, *Vector, *Result, *Checkoutput;
	struct timeval  TimeValue_Start;
        struct timezone TimeZone_Start;

        struct timeval  TimeValue_Final;
        struct timezone TimeZone_Final;
        long            time_start, time_end;
        double          time_overhead;

        printf("\n\t\t---------------------------------------------------------------------------");
        printf("\n\t\t Email : RarchK");
        printf("\n\t\t---------------------------------------------------------------------------");
        printf("\n\t\t Objective : Matrix-Vector Computations (Floating Point Operations)\n ");
        printf("\n\t\t Matrix into Vector Multiplication using ");
        printf("\n\t\t OpenMP one PARALLEL for directive and Private Clause;");
        printf("\n\t\t..........................................................................\n");

         /* Checking for command line arguments */
        if( argc != 5 ){

           printf("\t\t Very Few Arguments\n ");
           printf("\t\t Syntax : exec <Threads> <NoOfRows> <NoofColumns> <vector-size>\n");
           exit(-1);
        }

        Noofthreads=atoi(argv[1]);
        if ((Noofthreads!=1) && (Noofthreads!=2) && (Noofthreads!=4) && (Noofthreads!=8) && (Noofthreads!= 16) ) {
               printf("\n Number of threads should be 1,2,4,8 or 16 for the execution of program. \n\n");
               exit(-1);
         }

        NoofRows=atoi(argv[2]);
        NoofCols=atoi(argv[3]);
        Vectorsize=atoi(argv[4]);
              
/*	printf("\n\t\t Read the matrix size no. of rows and columns and vectorsize\n");
	scanf("%d%d%d", &NoofRows, &NoofCols, &Vectorsize);*/

	if (NoofRows <= 0 || NoofCols <= 0 || Vectorsize <= 0) {
		printf("\n\t\t The Matrix and Vectorsize should be of positive sign\n");
		exit(1);
	}

	/* Checking For Matrix Vector Computation Necessary Condition */
	if (NoofCols != Vectorsize) {
		printf("\n\t\t Matrix Vector computation cannot be possible \n");
		exit(1);
	}

	/* Dynamic Memory Allocation  And Initialization Of Matrix Elements */
	Matrix = (double **) malloc(sizeof(double) * NoofRows); 

	for (i = 0; i < NoofRows; i++) {
	 	Matrix[i] = (double *) malloc(sizeof(double) * NoofCols); 
		for (j = 0; j < NoofCols; j++)
			Matrix[i][j] = i + j;
	}

	/* Dynamic Memory Allocation */
	Vector = (double *) malloc(sizeof(double) * Vectorsize);

	/* vector Initialization */
	for (i = 0; i < Vectorsize; i++)
		Vector[i] = i;

	printf("\n");

        printf("\n\t\t Threads      : %d ",Noofthreads);
        printf("\n\t\t Matrix Size  : %d X %d ",NoofRows,NoofCols);
        printf("\n\t\t Vector Size  : %d\n",Vectorsize);

	/* Dynamic Memory Allocation */
	Result = (double *) malloc(sizeof(double) * NoofRows);
	Checkoutput = (double *) malloc(sizeof(double) * NoofRows);

	for (i = 0; i < NoofRows; i = i + 1)
	{
	Result[i]=0.0;
	Checkoutput[i]=0.0;
	}

	gettimeofday(&TimeValue_Start, &TimeZone_Start);

	omp_set_num_threads(Noofthreads);

	/* OpenMP Parallel for Directive :  Fork a team of threads giving them their own copies of variables */
	#pragma omp parallel for private(j)
	for (i = 0; i < NoofRows; i = i + 1) {
		for (j = 0; j < NoofCols; j = j + 1) {
         			Result[i] = Result[i] + Matrix[i][j] * Vector[j];
                 }
         }/* All thread join Master thread */  
	gettimeofday(&TimeValue_Final, &TimeZone_Final);



	/* Serial Computation */
	for (i = 0; i < NoofRows; i = i + 1)
		for (j = 0; j < NoofCols; j = j + 1)
			Checkoutput[i] = Checkoutput[i] + Matrix[i][j] * Vector[j];

	/* Checking with the serial calculation */
	for (i = 0; i < NoofRows; i = i + 1)
		if (Checkoutput[i] == Result[i])
			continue;
		else {
			printf("\n\t\t There is a difference from Serial and Parallel Computation \n");
			exit(1);
		}

	time_end = TimeValue_Final.tv_sec * 1000000 + TimeValue_Final.tv_usec;
        time_start = TimeValue_Start.tv_sec * 1000000 + TimeValue_Start.tv_usec;

        time_overhead = (time_end - time_start)/1000000.0;

 	 printf("\n\t\t Matrix into Vector Multiplication using OpenMP Parallel for directive ......Done \n");

        /*printf("\n\t\t Calculated PI :  \t%1.15lf  \n\t\t Error : \t%1.16lf\n", totalsum, fabs(totalsum - PI));*/
        printf("\n\t\t Time in Seconds (T)         : %lf",time_overhead);
        printf("\n\n\t\t   ( T represents the Time taken for computation )");
        printf("\n\t\t..........................................................................\n");


	/* Freeing The Memory Allocations */
	free(Vector);
	free(Result);
	free(Matrix);
	free(Checkoutput);

}
