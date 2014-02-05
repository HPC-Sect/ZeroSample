/*****************************************************************

             OpenMP-3.0 Example Codes Beta-v1.0      
        
File          :  parallel-construct-openmp3x.c 

Description   : Simple example program to demonstrate the use of
		OpenMP parallel	construct for coarse-grain parallelism 
		i.e each thread in the parallel	region decide which part 
		of the global vector to work on based on the thread ID.
		
OpenMP Pragma /
Function Used : - #pragma omp parallel  
                

Input         : None 

Output        : Output Vector after scaling                          
************************************************************************/

/* Header file inclusion */ 
#include<stdio.h>
#include<stdlib.h>

/* Macro checking */
#ifdef _OPENMP
	#include<omp.h>
#endif

/* global variable declaration */
int vectorSize;

/*
Description: Function perform the vector scaling by using the #pragma omp parllel 
directive. where each thread in the parallel region decide which part of the global
vector to work on based on the thread.

@param [tempVector]   :  Starting address of the vector
@param [totalThreads] :  Total number of the threads in the parallel region

@return               : None
*/
void parVectScale(double *tempVector,int totalThreads)
{

	int myId, istart, iend, partition, index;

	/* Set the no. of threads */
	omp_set_num_threads(totalThreads);

	printf("\n\t\t  Vector Scaling using OpenMP(parallel) Coarse Grain approach \n");

	/* Coarse grain parallism using openmp parallel construct */	
	#pragma omp parallel private(myId, istart, iend, index)
	{
  		myId = omp_get_thread_num();
		/* Division of the work according to the thread ID */
		partition = vectorSize  / omp_get_num_threads(); 
		istart = myId * partition;
		iend = istart + partition-1;
		
		printf("\n\t\t My Thread Id : %d , Start Index : %d , End Index : %d ",myId,istart,iend);	
		for ( index = istart ; index <= iend ; index++ )
			tempVector[index] = 2 * tempVector[index];
	}

} /* End of parVectScale function */

/*
Description : The function perform the vector scaling serially.
	      If the _OPENMP macro has not been defined the 
	      computation will be performed serially.

@param [tempVector] : Starting address of the vector.

@return               : None
*/

void  serialVectScale(double *tempVector)
{
	int index;
	printf("\n\n\n\t Vector Scaling using serial version");
        for ( index = 0 ; index < vectorSize  ; index++ )
                        tempVector[index] = 2 * tempVector[index];

}/* End of function */

/* the main function*/
int main(int argc , char **argv)
{
	int    	numThreads,index;
	double 	*vector;

	/* Checking for command line arguments */
        if( argc != 3 ){

           printf("\t\t Very Few Arguments\n ");
           printf("\t\t Syntax : exec <vector-size>  <No. of Threads>\n");
           exit(-1);
        }

        vectorSize =atoi(argv[1]);
        numThreads =atoi(argv[2]);
		
	/* Allocating the Memory to the vector */
	if((vector = (double *)malloc (sizeof(double) * vectorSize )) == NULL) {
		perror( " Memory allocation for vector  "); 
		exit(-1);
	}

	/* Initializing the vector */
	for ( index = 0 ; index < vectorSize  ; index++){
		vector[index] = rand()+0.005  ;
       // 	printf(" %lf ",vector[index]);
	}
	printf("\n\t\t Initializing Vector..........Done \n\t");

        /* Function to perform vector scaling using openmp parallel construct */
	#ifdef _OPENMP
		parVectScale(vector,numThreads);
	#else
        /* Function to perform vector scaling serially */
		serialVectScale(vector);
	#endif

	/* Uncomment to print the output vector */
	/*printf("\n\t Vector after scaling \n\t");
	for ( index = 0 ; index <vectorSize  ; index++)
        	printf(" %lf ",vector[index]);*/ 

	printf("\n\n");


	return 0;
} /* End of main function */
