

/********************************************************************************************

	
  Source Code :  sharedMemoryStridedAccessPatterns.cu

  Objective   :	 This code demonstrates bank conflicts that can occur while accessing 
                 the shared memory

  Descritpion :  Example code to demonstrate that while reading the same word by all the 
                 threads, there will not be any serialization since all threads are accessing 
                 from the same bank the 32-bit word gets broadcasted to all the threads -- 
		 hence bandwidth can be comparable to the value got when there were no 
                 bank conflicts

  Description :  Example code to demonstrate the different strided access patterns in the 
                 shared memory the corresponding advantages in terms of the bandwidth of the 
                 shared memory that is achievable. 

                 The following strided accesses are demonstrated: 

		    stride of one 32-bit word      : resulting in no bank conflicts
	            stride of two 32-bit words     : causes 2-way bank conflicts 
	            stride of three 32-bit words   : causes no bank conflicts
		    stride of eight 32-bit words   : causes 8-way bank conflicts
	     	    stride of sixteen 32-bit words : causes 16-way bank conflicts

  Input       :  none

  output      :	 The different bandwidths that are achieved for the different strided accesses 
                  of the shared memory

 Modified     :  Aug 2011
                 

 Author	     :   RarchK		

****************************************************************************************/

#include <cuda.h>
#include <stdio.h>
#include <float.h>

#define BLOCK_SIZE 128   // 128 threads per block
#define TRANSFERED_DATA_SIZE 2000000   //2 MB

#define NO_OF_PATTERNS 5 
#define NTIMES 10

#define MIN(x,y) ((x)<(y)?(x):(y))
#define MAX(x,y) ((x)>(y)?(x):(y))

void printResults(void);


////////////////////////////////////////////////////////////////////////////////////////////////
//
// access pattern involving accessing the elements with a stride of one 32-bit word
// the access pattern causes no bank conflicts
//
/////////////////////////////////////////////////////////////////////////////////////////////////

__global__ void sharedMemAccessWithStride1(void)
{
	__shared__ __attribute__((unused)) float array[BLOCK_SIZE];
	int idx = threadIdx.x;
	float scalar = 5.0f;
	array[idx] = scalar;
}

////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
//
// access pattern involving accessing the elements with a stride of three 32-bit words 
// the access pattern causes no bank conflicts
//
/////////////////////////////////////////////////////////////////////////////////////////////////

__global__ void sharedMemAccessWithStride3(void)
{
	__shared__ __attribute__((unused)) float array[BLOCK_SIZE];
	int idx = (threadIdx.x * 3) % BLOCK_SIZE;
	float scalar = 5.0f;
	array[idx] = scalar;
}

////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
//
// access pattern involving accessing the elements with a stride of two 32-bit words 
// the access pattern causes 2-way bank conflicts. 
//
/////////////////////////////////////////////////////////////////////////////////////////////////

__global__ void sharedMemAccessWithStride2(void)
{
	__shared__ __attribute__((unused)) float array[BLOCK_SIZE];
	int idx = (threadIdx.x * 2) % BLOCK_SIZE;
	float scalar = 5.0f;
	array[idx] = scalar;
}

////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
//
// access pattern involving accessing the elements with a stride of eight 32-bit words 
// the access pattern causes 8-way bank conflicts. 
//
/////////////////////////////////////////////////////////////////////////////////////////////////

__global__ void sharedMemAccessWithStride8(void)
{
	__shared__ __attribute__((unused)) float array[BLOCK_SIZE];
	int idx = (threadIdx.x * 8) % BLOCK_SIZE;
	float scalar = 5.0f;
	array[idx] = scalar;
}

////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////
//
// access pattern involving accessing the elements with a stride of sixteen 32-bit words 
// the access pattern causes 16-way bank conflicts. 
//
/////////////////////////////////////////////////////////////////////////////////////////////////

__global__ void sharedMemAccessWithStride16(void)
{
	__shared__ __attribute__((unused)) float array[BLOCK_SIZE];
	int idx = (threadIdx.x * 16) % BLOCK_SIZE;
	float scalar = 5.0f;
	array[idx] = scalar;
}

///////////////////////////////////////////////////////////////////////////////////////////////





static double  avgtime[NO_OF_PATTERNS] = {0}, maxtime[NO_OF_PATTERNS] = {0}, mintime[NO_OF_PATTERNS];
static float bandWidths[NO_OF_PATTERNS] = {0}; 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// the main routene 
// for timing the different strided access patterns 
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
			sharedMemAccessWithStride1 <<< gridSize,BLOCK_SIZE >>> ();
			cudaEventRecord(stop,0);
			cudaEventSynchronize(stop);
			cudaEventElapsedTime(&elapsedTimes[0][k],start,stop);

			// PATTERN 2
			cudaEventRecord(start,0);
			sharedMemAccessWithStride3 <<< gridSize,BLOCK_SIZE >>> ();
			cudaEventRecord(stop,0);
			cudaEventSynchronize(stop);
			cudaEventElapsedTime(&elapsedTimes[1][k],start,stop);
		
			// PATTERN 3
			cudaEventRecord(start,0);
			sharedMemAccessWithStride2 <<< gridSize,BLOCK_SIZE >>> ();
			cudaEventRecord(stop,0);
			cudaEventSynchronize(stop);
			cudaEventElapsedTime(&elapsedTimes[2][k],start,stop);
			
			// PATTERN 4
			cudaEventRecord(start,0);
			sharedMemAccessWithStride8 <<< gridSize,BLOCK_SIZE >>> ();
			cudaEventRecord(stop,0);
			cudaEventSynchronize(stop);
			cudaEventElapsedTime(&elapsedTimes[3][k],start,stop);

			//PATTERN 5
			cudaEventRecord(start,0);
			sharedMemAccessWithStride16 <<< gridSize,BLOCK_SIZE >>> ();
			cudaEventRecord(stop,0);
			cudaEventSynchronize(stop);
			cudaEventElapsedTime(&elapsedTimes[4][k],start,stop);


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
// prints the results containig the minimum, maximum, average times that the different strided access pattern have taken
// the associated maximum bandwidth of the different patterns
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void printResults()
{

	printf("Demonstrating the different strided accesses of the shared memory\n");
		
   printf("The transfered data size (Bytes): %ld\n",TRANSFERED_DATA_SIZE*sizeof(float));

   printf("\n-------------------------------------------------------------------------------------------------------------------------------\n");
   printf("Pattern \t\t\t Bandwidth (GB/sec) \t Avg time (ms)  \t Min time (ms)  \t Max time (ms)\n");
   printf("-------------------------------------------------------------------------------------------------------------------------------\n");
	// printing the results for different access patterns
	for(int i=0; i < NO_OF_PATTERNS; i++)
	{
		
		switch(i)
		{
			case 0: printf("Stride of one 32-bit word     ");
					  break;

			case 1: printf("Stride of three 32-bit words  ");
					  break;
			
			case 2: printf("Stride of two 32-bit words    ");
					  break;

			case 3: printf("Stride of eight 32-bit words  ");
					  break;

			case 4: printf("Stride of sixteen 32-bit words");
					  break;

		}
		
		printf("\t %.6f \t\t %f \t\t  %f \t\t  %f\n",bandWidths[i]/1000000,avgtime[i],mintime[i],maxtime[i]);
  }
	
   printf("\n ------------------------------------------------------------------------------------------------------------------------------\n");

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


