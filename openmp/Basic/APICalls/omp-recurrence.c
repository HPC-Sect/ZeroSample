
/*****************************************************************************



 Example 1.3           : omp-recurrence.c


 Objective	       : Write an OpenMP program for parallelization of a loop nest
                  	 containing a recurrence relation.
                   	 This example demonstrates the use of PARALLEL FOR DIRECTIVE
 
 Input                 : Size of the Matrix

                   	 Number of Threads 


 Output                : Each thread does the row wise recurrence calculation
                  	 and the final Matrix and status of execution i.e. The status
 		  	 of comparison of parallel and serial result of a loop nest 
                  	 containing recurrence is printed by Master Thread	                                            
                                                                        
 Created               :Aug 2011. 
       
 
 Author                : RarchK                                          


*********************************************************************************/


#include <stdio.h>
#include<stdlib.h>
#include <omp.h>

/* Main Program */
main(int argc,char **argv)
{
	double        **InputMatrix, **CheckMatrix;
	int             j, i, N,Noofthreads,total_threads;

	printf("\n\t\t---------------------------------------------------------------------------");
        printf("\n\t\t Email : RarchK");
        printf("\n\t\t---------------------------------------------------------------------------");
        printf("\n\t\t Objective : Parallization of a loop nest contating a recurrence relation.\n ");
        printf("\n\t\t Demonstrates the use of OpenMP Parallel for directive  ");
        printf("\n\t\t..........................................................................\n");


	 /* Checking for command line arguments */
        if( argc !=3 ){

           printf("\t\t Very Few Arguments\n ");
           printf("\t\t Syntax : exec <Threads> <matrix-size>\n");
           exit(-1);
        }

              

        Noofthreads=atoi(argv[1]);
        N=atoi(argv[2]);

        if ((Noofthreads!=1) && (Noofthreads!=2) && (Noofthreads!=4) && (Noofthreads!=8) && (Noofthreads!= 16) ) {
               printf("\n Number of threads should be 1,2,4,8 or 16 for the execution of program. \n\n");
               exit(-1);
         }

/*	printf("\n\t\t Enter the size of the Matrix\n");
	scanf("%d", &N);*/

	/* Input Checking */
	if (N <= 0) {
		printf("\n\t\t Array Size Should Be Of Positive Sign \n");
		exit(1);
	}

	/* Dynamic Memory Allocation */
	InputMatrix = (double **) malloc(sizeof(double *) * N);
	CheckMatrix = (double **) malloc(sizeof(double *) * N);

	/* Initializing The Matrix Elements */
	for (i = 0; i < N; i++) {
		InputMatrix[i] = (double *) malloc(sizeof(double) * N);
		for (j = 0 ; j < N; j++)
			InputMatrix[i][j] = i + j;
	}

	/* CheckMatrix Is Also Same As Input Matrix */
	for (i =0; i < N; i++) {
		CheckMatrix[i] = (double *) malloc(sizeof(double) * N);
		for (j = 0; j < N; j++)
			CheckMatrix[i][j] = InputMatrix[i][j];
	}

        /* set the number of threads */
	omp_set_num_threads(Noofthreads);

	/* OpenMP Parallel For Directive : Fork the team of threads 
           As In following code segment the j loop contain the recurrence i.e. the loop contain the data dependence
           each iteration write an element of InputMatrix that is read by the next iteration. that is difficult 
           to remove,so the  i loop is parallelised instead.  
  
             */
	for (j = 1; j < N; j++)
	#pragma omp parallel for
	for (i = 1; i < N; i++)
               	{
                       if ( (omp_get_thread_num()) == 0)
                        {
                            total_threads=omp_get_num_threads();
                         } 
			InputMatrix[i][j] = InputMatrix[i][j] + InputMatrix[i][j - 1];
              }/* End of the parallel region */
         

	/* For Validity Of Output */
	/* Serial Calculation */
	for (j = 1; j < N; j++)
		for (i = 1; i < N; i++)
			CheckMatrix[i][j] = CheckMatrix[i][j] + CheckMatrix[i][j - 1];



	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
			if (CheckMatrix[i][j] == InputMatrix[i][j]) {
				continue;
			} else {
				printf("\n\t\t The result of the serial and parallel calculation are not Equal \n");
				exit(1);
			}



/*	printf("\n The Output Matrix After Loop Nest Containing a Recurrence \n");
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++)
			printf("%lf\t", InputMatrix[i][j]);
		printf("\n");
	}*/

        printf("\n\n\t\t Threads     : %d",total_threads);	
        printf("\n\t\t Matrix Size : %d ",N); 
	printf("\n\n\t\t Serial And Parallel Calculation Are Same. \n");

        printf("\n\t\t..........................................................................\n");
	printf("\n");

	/* Freeing Of Allocated Memory */
	free(InputMatrix);
	free(CheckMatrix);

}
