


/************************************************************************************
				PEMG-2010 
			    June 21-24, 2010
	
  Source Code :  sharedMemoryRestructuringDataTypes.cu

  Objective   :  This code demonstrates achievable shared memory bandwidth for different 
                 inbuilt data types

  Description :  Example code to demonstrate the different shared memory bandwidths 
                 achieved when 
			 1) accessing a 3d array of floats
			 2) accessing a float3 array
			 3) accessing a 4d array of floats
			 4) accessing a float4 array

 Input        :  None 

 output       :	 The different bandwidths of the shared memory that are achieved in the above 
                 mentioned accesses

 Modified     :  May 2010
                 National PARAM Supercomputing Facility, C-DAC,Pune.

 Author	      :  RarchK		

****************************************************************************************/

#include <stdio.h>
#include <cuda.h>
#include <float.h>

#define NO_OF_PATTERNS 4
#define NTIMES 10

#define BLOCK_SIZE 128
#define TRANSFERED_DATA_SIZE 2000000   //2 MB

#define MIN(x,y) ((x)<(y)?(x):(y))
#define MAX(x,y) ((x)>(y)?(x):(y))

void printResults(void);



////////////////////////////////////////////////////////////////////////////////////////////////
//
// intialising an unused 3D float array in shared memory
// access pattern involving accessing the i-th element from all the rows by the i-th thread 
//
/////////////////////////////////////////////////////////////////////////////////////////////////

__global__ void set3DFloatArray( float value )
{
	__shared__ __attribute__((unused)) float array[3][BLOCK_SIZE];
	int idx = threadIdx.x;	
	array[0][idx] = value;
	array[1][idx] = value;
	array[2][idx] = value;
}

/////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// intialising an unused float3 array in shared memory
// access pattern involving accessing all the fields of the i-th element of the array by the i-th thread 
//
////////////////////////////////////////////////////////////////////////////////////////////////////////

__global__ void setFloat3Array( float value )
{
	__shared__ __attribute__((unused)) float3 array[BLOCK_SIZE];
	int idx = threadIdx.x;	
	array[idx].x = value;
	array[idx].y = value;
	array[idx].z = value;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////
//
// intialising an unused 4D float array in shared memory
// access pattern involving accessing the i-th element from all the rows by the i-th thread 
//
///////////////////////////////////////////////////////////////////////////////////////////////

__global__ void set4DFloatArray( float value )
{
	__shared__ __attribute__((unused)) float array[4][BLOCK_SIZE];
	int idx = threadIdx.x;	
	array[0][idx] = value;
	array[1][idx] = value;
	array[2][idx] = value;
	array[3][idx] = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// intialising an unused float4 array in shared memory
// access pattern involving accessing all the fields of the i-th element of the array by the i-th thread 
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

__global__ void setFloat4Array( float value )
{
	__shared__ __attribute__((unused)) float4 array[BLOCK_SIZE];
	int idx = threadIdx.x;	
	array[idx].x = value;
	array[idx].y = value;
	array[idx].z = value;
	array[idx].w = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////




static double  avgtime[NO_OF_PATTERNS] = {0}, maxtime[NO_OF_PATTERNS] = {0}, mintime[NO_OF_PATTERNS];
static float bandWidths[NO_OF_PATTERNS] = {0}; 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// the main routene 
// for timing the different initialising kernels 
// finding the band widths
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
	float elapsedTimes[NO_OF_PATTERNS][NTIMES];
	cudaEvent_t start,stop;
	
	double bytes = sizeof(float) * TRANSFERED_DATA_SIZE;
	
	// event creation, which will be used for timing the code
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	
	//finding the 1D grid size 
	int gridSize = TRANSFERED_DATA_SIZE/BLOCK_SIZE;
	
	if( TRANSFERED_DATA_SIZE % BLOCK_SIZE != 0 ) 
			gridSize += 1;
	
	// running each pattern NTIMES
	for(int k=0; k < NTIMES; k++)
	{
		// timing the kernels corresponding to different access patterns
			
			// PATTERN 1
			cudaEventRecord(start,0);
			set3DFloatArray <<< gridSize,BLOCK_SIZE >>> (2.0f);
			cudaEventRecord(stop,0);
			cudaEventSynchronize(stop);
			cudaEventElapsedTime(&elapsedTimes[0][k],start,stop);

			// PATTERN 2
			cudaEventRecord(start,0);
			setFloat3Array <<< gridSize,BLOCK_SIZE >>> (2.0f);
			cudaEventRecord(stop,0);
			cudaEventSynchronize(stop);
			cudaEventElapsedTime(&elapsedTimes[1][k],start,stop);

			//PATTERN 3
			cudaEventRecord(start,0);
			set4DFloatArray <<< gridSize,BLOCK_SIZE >>> (2.0f);
			cudaEventRecord(stop,0);
			cudaEventSynchronize(stop);
			cudaEventElapsedTime(&elapsedTimes[2][k],start,stop);
		
			//PATTERN 3
			cudaEventRecord(start,0);
			setFloat4Array <<< gridSize,BLOCK_SIZE >>> (2.0f);
			cudaEventRecord(stop,0);
			cudaEventSynchronize(stop);
			cudaEventElapsedTime(&elapsedTimes[3][k],start,stop);


	} // end of the for loop involving NTIMES


	// intializing the mintime array
	for(int i=0; i < NO_OF_PATTERNS;i++)
		mintime[i] = FLT_MAX;


   for (int k=1; k < NTIMES; k++) // skiping the first iteration
	{
	   for (int i=0; i < NO_OF_PATTERNS; i++)
	   {
	       avgtime[i] = avgtime[i] + elapsedTimes[i][k];
	       mintime[i] = MIN(mintime[i],elapsedTimes[i][k]);
	       maxtime[i] = MAX(maxtime[i], elapsedTimes[i][k]);
	   }
	}


	// calculation of the different band widths that are achieved by different access patterns  
	for(int i=0; i < NO_OF_PATTERNS; i++)
	{
		avgtime[i] = avgtime[i]/(double)(NTIMES-1); // finding the average time
		bandWidths[i] = bytes/mintime[i];
	}

   printResults();

	printf("\n\n**** successful termination of the program  ****\n\n");

	cudaEventDestroy(start);
	cudaEventDestroy(stop);

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// prints the results containig the minimum, maximum, average times taken by different kernels
// the associated maximum bandwidth of the sharede memory achieved by the different kernals
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void printResults()
{

	printf("Demonstrating differences among the different data types and their accesses in the shared memory\n");
		
   printf("The transfered data size (Bytes): %ld\n",TRANSFERED_DATA_SIZE*sizeof(float));

   printf("\n-------------------------------------------------------------------------------------------------------------------------------\n");
   printf("Pattern \t\t\t Bandwidth (GB/sec) \t Avg time (ms)  \t Min time (ms)  \t Max time (ms)\n");
   printf("-------------------------------------------------------------------------------------------------------------------------------\n");

	// printing the results for different access patterns
	for(int i=0; i < NO_OF_PATTERNS; i++)
	{
		
		switch(i)
		{
			case 0: printf("accessing 3-d float array ");
					  break;

			case 1: printf("accessing 1-d float3 array");
					  break;

			case 2: printf("accessing 4-d float array ");
					  break;
			
			case 3: printf("accessing 1-d float4 array");
					  break;
		}

		printf("\t %.6f \t\t %f \t\t  %f \t\t  %f\n",bandWidths[i]/1000000,avgtime[i],mintime[i],maxtime[i]);
  }
	
   printf("\n ------------------------------------------------------------------------------------------------------------------------------\n");

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

