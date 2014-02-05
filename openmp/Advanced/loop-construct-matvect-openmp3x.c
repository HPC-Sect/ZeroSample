/****************************************************************************
  

               OpenMP-3.0 Example Codes Beta-v1.0      
        
File          : loop-construct-matvect-openmp3x.c 

Date          :Aug 2011    


Description   : The program perform the matrix vector multiplication in 
		parallel using the openmp-3.0 feature collapse clause and
                nested parallel directive openMP-2.5 approach and display 
		the time taken in both the approches.

                a) loopParNested(OpenMP-2.5) : In this approach the nested
		loop is parallelised using nested parallel directive.
		Which may incure the high overheads of creating nested 
		parallel region.
		   
                b) loopParCollapse(OpenMP-3.0): In this approach the openmp-3.0
		feature "collapse" clause has been used to parallelize the nested
		loop.The iteration space over the loop index i and j is collapsed 
		into the single large iteration space which then executed by the 
		team of threads.


OpenMP pragma/
Directive used : #pragma omp parallel
		 - collapse clause 

Input         : - Number of threads
		- Number of Rows
		- Number of Columns
		- Vector Size  
Output        :  Time Taken in both approach

***************************************************************************/

/* Header file inclusion */
#include <stdio.h>
#include<omp.h>
#include<stdlib.h>
#include<assert.h>

/* Function Prototype */
int loopParCollapse(int threads,double *matrix[],double *vector,long int rows,long int cols,long int vectorSize);
int loopParNested(int threads,double *matrix[],double *vector,long int rows,long int cols,long int vectorSize);
int checkResult(double *matrix[],double *vector,double *resultVector,int rows,int cols);

/* Main function */
int main(int argc, char *argv[]) {

	long int numThreads,matRows,matCols,vectorSize,i,j;
        double	 **matrix,*vector;

	 /* Checking for command line arguments */
        if( argc != 5 ){

           printf("\t\t Very Few Arguments\n ");
           printf("\t\t Syntax : exec <Threads> <NoOfRows> <NoofColumns> <vector-size>\n");
           exit(-1);
        }

	/* Initializing nuber of threads */
        numThreads=atoi(argv[1]);
        
	/* Checking for the condition Number of threads should be 1/2/4/8/16 */
	if ((numThreads!=1) && (numThreads!=2) && (numThreads!=4) && (numThreads!=8) && (numThreads!= 16) ) {
               printf("\n Number of threads should be 1,2,4,8 or 16 for the execution of program. \n\n");
               exit(-1);
         }

   	/* 	Initializing number of Rows & 
		Columns in the matrix and the vector size */
        matRows=atol(argv[2]);
        matCols=atol(argv[3]);
        vectorSize =atol(argv[4]);

	/* Checking Matrix and Vector size should be positive */
        if (matRows <= 0 || matCols <= 0 || vectorSize <= 0) {
                printf("\n\t\t The Matrix and Vectorsize should be of positive sign\n");
                exit(-1);
        }

        /* Checking For Matrix Vector Computation Necessary Condition */
        if (matCols != vectorSize) {
                printf("\n\t\t Matrix Vector computation cannot be possible \n");
                exit(-1);
        }

	 /* Dynamic Memory Allocation  And Initialization Of Matrix Elements */
      	assert((matrix = (double **) malloc(sizeof(double) * matRows))!=NULL);
        for (i = 0; i < matRows; i++) {
               assert((matrix[i] = (double *) malloc(sizeof(double) * matCols))!=NULL);
                for (j = 0; j < matCols; j++)
                        matrix[i][j] = i + j;
        }

        /* Dynamic Memory Allocation  for Vector*/
      assert((vector = (double *) malloc(sizeof(double) * vectorSize))!=NULL);

        /* vector Initialization */
        for (i = 0; i < vectorSize; i++)
                vector[i] = i;

	printf("\n\t\t Matrix Size       : %ld * %ld ", matRows,matCols);	
	printf("\n\t\t Vector Size       : %ld      ", vectorSize);	
	printf("\n\t\t Number of threads : %ld      ", numThreads);	

	/* Function calling to perform Matrix Vector Multiplication 
	   using OpenMP-3.0 Collapse clause  */
	if((loopParCollapse(numThreads,matrix,vector,matRows,matCols,vectorSize))==1)
	{
		printf("\n\t Matrix Vector Multiplication Collapse clause is failed \n");
		exit(-1);
	 }
	/* Function calling to perform Matrix Vector Multiplication 
	   using OpenMP-2.5 nested parallel regions  */
	if((loopParNested(numThreads,matrix,vector,matRows,matCols,vectorSize))==1)
	{
		printf("\n\t Matrix Vector Multiplication Netsed Approach is failed \n");
		exit(-1);
	}
        


}/* End of main */

/*
Description: Parallelize Nested loop using Collapse clause (openmp-3.0). Collapse clause
	     reduce the iterations in sigle iteration space which is executed by the
	     threads in the team.  
@param [threads] 	:  Number of threads
@param [matrix]	 	:  Starting address of Input Matrix
@param [vector]	 	:  Starting address of Input Vector
@param [rows  ]	 	:  Number of Rows in the matrix
@param [cols ]	 	:  Number of Columns in the matrix
@param [vectorSize]	 : Vector Size 

@return 		: Return 0 if sucessful else 1 if failed 
*/

int loopParCollapse(int threads,double *matrix[],double *vector,long int rows,long int cols,long int vectorSize)
{

	int i,j;
	double start_time, end_time;
        double *result;

	/* Dynamic Memory Allocation for output vector */
        assert((result = (double *) malloc(sizeof(double) * rows))!=NULL);
        /* Initializing output vector */
	for (i = 0; i < rows; i = i + 1)
        	result[i]=0.0;

	/* Setting the number of threads */
	omp_set_num_threads(threads);

	start_time = omp_get_wtime();

	/* Create the parallel region & reduce the iteration space 
	   over i and j to single iteration space which is 
	   then executed by team of threads */
        #pragma omp parallel for collapse(2)
        for ( i = 0 ; i <rows  ; i++ ) {
                for (j=0; j<cols ;j++ ) {
		result[i]=result[i]+matrix[i][j]*vector[j]; 
                }
        } /* End of parllel region */
	end_time = omp_get_wtime();

        /* Verifing the ouput by parallel computation*/
	if(checkResult(matrix,vector,result,rows,cols)!=0){
		printf("\n\t\t There is a difference from Serial and Parallel Computation \n");
		return 1;
	}


	printf("\n\t\t Time Taken (Collapse Clause : OpenMP-3.0)   : %lf sec ",(end_time-start_time));

	return 0;
} /* End of the function */

/*
Description: Parallelize Nested loop using "Nested Parallel Directive" (openmp-2.5).  
In this approach the nested loop is parallelised using nested parallel directive.
Which may incure the high overheads of creating nested parallel region.
  
@param [threads]        :  Number of threads
@param [matrix]         :  Starting address of Input Matrix
@param [vector]         :  Starting address of Input Vector
@param [rows  ]         :  Number of Rows in the matrix
@param [cols ]          :  Number of Columns in the matrix
@param [vectorSize]      : Vector Size 

@return                 : Return 0 if sucessful else 1 if failed 
*/

int loopParNested(int threads,double *matrix[],double *vector,long int rows,long int cols,long int vectorSize)
{

        int i,j;
        double start_time, end_time;
        double *result;

        /* Dynamic Memory Allocation for output vector*/
        assert((result = (double *) malloc(sizeof(double) * rows))!=NULL);
        for (i = 0; i < rows; i = i + 1)
                result[i]=0.0;

	/* Enabling the nested parallel region */
	omp_set_nested(1);
	
	/* Setting the number of threads */
        omp_set_num_threads(threads);

        start_time = omp_get_wtime();

	/* Outer : Creating the parllel region and divide the 
		   between the thread team*/
        #pragma omp parallel for private(j) 
        for ( i = 0 ; i <rows  ; i++ ) {
		/* Inner : Creating the parllel region inside the outer 
		   parallel region and divide the work between 
		   the thread team */
        	#pragma omp parallel for 
                for (j=0; j<cols ;j++ ) {
                	result[i]=result[i]+matrix[i][j]*vector[j];
                }
        }
        end_time = omp_get_wtime();

        printf("\n\t\t Time Taken (Nested Parallelism : OpenMP-2.5) : %lf sec  \n\n ",(end_time-start_time));

        return 0;
}/* End of the Function */
/*
Description : Function to check the output .

@param [matrix] 	:  Input matrix
@param [vector]	      	: Input vector
@param [resultVector] 	: Output vector
@param [rows]	 	: Number of Rows 
@param [cols]		: Number of columns

@return 		: Return 0 if sucessful else 1 if failed 
*/

int checkResult(double *matrix[],double *vector,double *resultVector,int rows,int cols)
{

        double 	*checkOutVector;
	int 	i,j;


        /* Dynamic Memory Allocation for vector*/
        assert((checkOutVector = (double *) malloc(sizeof(double) * rows))!=NULL);
	 for (i = 0; i < rows; i = i + 1)
               checkOutVector[i]=0.0;


	/* Serial Computation */
        for (i = 0; i < rows; i = i + 1)
                for (j = 0; j < cols; j = j + 1)
                      checkOutVector[i] = checkOutVector[i] + matrix[i][j] * vector[j];

        /* Checking Parallel computation result with the serial computation */
        for (i = 0; i < rows; i = i + 1){
                if (checkOutVector[i] == resultVector[i])
                        continue;
                else 
			return 1;
	}

	return 0;

}/* End of the function */

