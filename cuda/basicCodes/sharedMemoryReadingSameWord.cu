
/**********************************************************************************************

	
  Source Code :  sharedMemoryReadingSameWord.cu

  Objective   :  Example code to demonstrate the different access patterns of float3 array 
                 in the global memory the corresponding advantages in terms of the bandwidth 
                 that is achievable

  Descritpion :  Example code to demonstrate that while reading the same word by all the 
                 threads, there will not be any serialization since all threads are accessing 
                 from the same bank the 32-bit word gets broadcasted to all the threads -- 
		 hence bandwidth can be comparable to the value got when there were no 
                 bank conflicts

  output:	 The different bandwidths of the shared memory that are achieved while 
                 accessing the same word and the access pattern without any bank conflicts

 Modified     :  Aug 2011
                 

 Author	      :  RarchK		

****************************************************************************************/

#include <stdio.h>
#include <cuda.h>
#include <float.h>

#define BLOCK_SIZE 128   // 128 threads per block
#define TRANSFERED_DATA_SIZE 2000000   //2 MB

#define NO_OF_PATTERNS 2 
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
// access pattern involving reading the same word by all the threads  
// the access pattern is conflict-free 
// since the word gets broadcasted to all the threads
//
/////////////////////////////////////////////////////////////////////////////////////////////////

__global__ void sharedMemReadSameWord(void)
{
	__shared__  float array[BLOCK_SIZE];
	int idx = 3;
	float __attribute__((unused)) scalar ;
	scalar = array[idx];
}

///////////////////////////////////////////////////////////////////////////////////////////////////






static double  avgtime[NO_OF_PATTERNS] = {0}, maxtime[NO_OF_PATTERNS] = {0}, mintime[NO_OF_PATTERNS];
static float bandWidths[NO_OF_PATTERNS] = {0}; 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// the main routene 
// for timing the two kernals 
// finding the corresponding band widths
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
			sharedMemReadSameWord <<< gridSize,BLOCK_SIZE >>> ();
			cudaEventRecord(stop,0);
			cudaEventSynchronize(stop);
			cudaEventElapsedTime(&elapsedTimes[1][k],start,stop);

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
// prints the results containig the minimum, maximum, average times taken by the two kernels
// the associated maximum bandwidth achieved by the different kernels
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void printResults()
{
	printf("Demonstrating that reading same 32-bit word by all threads will not cause bank conflicts\n");
		
   printf("The transfered data size (Bytes): %ld\n",TRANSFERED_DATA_SIZE*sizeof(float));

   printf("\n-------------------------------------------------------------------------------------------------------------------------------\n");
   printf("Pattern \t\t\t Bandwidth (GB/sec) \t Avg time (ms)  \t Min time (ms)  \t Max time (ms)\n");
   printf("-------------------------------------------------------------------------------------------------------------------------------\n");


	// printing the results for different access patterns
	for(int i=0; i < NO_OF_PATTERNS; i++)
	{
		switch(i)
		{
			case 0: printf("Stride of one 32-bit word   ");
					  break;

			case 1: printf("Reading the same 32-bit word");
					  break;

		}
		
		printf("\t %.6f \t\t %f \t\t  %f \t\t  %f\n",bandWidths[i]/1000000,avgtime[i],mintime[i],maxtime[i]);
  }
	
   printf("\n ------------------------------------------------------------------------------------------------------------------------------\n");

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


