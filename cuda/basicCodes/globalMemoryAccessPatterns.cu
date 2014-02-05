

/********************************************************************************************

	
  source Code :  globalMemoryAccessPatterns.cu

  Objective   :  Example code to demonstrate the different access patterns of the global 
                 memory and the corresponding advantages in terms of the bandwidth that 
                 is achievable.

  Description : The six patterns that are discussed in the programming guide which the 
                program is implementing

		1) coalesced float memory access, resulting in a single memory transaction
		2) coalesced float memory access (divergent warp), resulting in a single 
                   memory transaction
		3) non-sequential float memory access, resulting in 16 memory transactions
		4) access with a misaligned starting address, resulting in 16 memory transactions
		5) non-contiguous float memory access, resulting in 16 memory transactions
		6) non-coalesced float3 memory access, resulting in 16 memory transactions

                 the six patterns along with the cudaMemcpy, the bandwidth is being calculated 

input       :   none

output      :	The different bandwidths that are achieved for different access patterns

Modified    :   Aug 2011
                

Author 	    :   RarchK		

*****************************************************************************************/

#include <cuda.h>
#include <stdio.h>
#include <float.h>

// there are six patterns that have been quoted in programming guide 2.3.1 for devices with compute capability 1.1
#define NO_OF_PATTERNS 6 

#define ARRAY_SIZE 20000000 // 20 MB
#define BLOCK_SIZE 512

#define NTIMES 10 

#define MIN(x,y) ((x)<(y)?(x):(y))
#define MAX(x,y) ((x)>(y)?(x):(y))

void printResults(void); // function declaration

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// the access pattern where successive threads are acceessing the successive memory locations in the Global memory
// and the array is aligned at (128)
// the first scenario from the performance guidelines of the programming guide
// coalesced float memory access, resulting in a single memory transaction
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

__global__ void coalescedGMAccess(float* dest,float* src,long size)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
	if(idx < size)
		dest[idx] = src[idx];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// the access pattern where successive threads are accessing the successive memory locations in the Global memory
// and array is aligned but some of the threads are not acessing any memory location
// the second scenario from the performance guide lines of the programming guide
// coalesced float memory access (divergent warp), resulting in a single memory transaction
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

__global__ void coalescedGMAccessDivergent(float* dest,float* src,long size)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
	if(idx < size)
	//every warp there will be 4 threads that will not be accessing any memory location
		if(idx % 8 != 0) 
			dest[idx] = src[idx];
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// kernel for setting the array with given element
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

__global__ void setArray(float *array,  float value, int size)
{
	int idx = threadIdx.x + blockIdx.x * blockDim.x;
	if (idx < size)
		array[idx] = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// the access pattern where successive threads are accessing the successive memory locations in the Global memory 
// but the starting address is misaligned
// the fourth scenario from the performance guidelines of the programming guide
// access with a misaligned starting address, resulting in 16 memory transactions.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

__global__ void nonCoalescedGMAccessMisalign(float* dest,float* src,long size)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
	if(idx < size-1) // the last thread is skipped, it will not access any location
		dest[idx+1] = src[idx+1];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// the access pattern where successive threads are not accessing the successive memory locations in the Global memory
// the starting address is aligned
// the third scenario from the performance guidelines of the programming guide
// non-sequential float memory access, resulting in 16 memory transactions
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

__global__ void nonCoalescedGMAccessNonSeq(float* dest,float* src,long size)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
	if(idx < size)
	{

	//in each half warp the third thread accesses the location which would have been accessed by the fourth thread in coalesced access
	//and fourth thread accesses the location of the third thread in the coalesced access

		if(idx % 16 == 3)
			idx = 4;
		else if(idx % 16 == 4)
			idx = 3;
					
		dest[idx] = src[idx];

	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// the access pattern where the successive threads are not accessing the successive memory locations
// the starting address is aligned
// the fifth scenario from the performance guidelines of the programming guide
// non-contiguous float memory access, resulting in 16 memory transactions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

__global__ void nonCoalescedGMAccessNonContiguous(float* dest,float* src,long size)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;

	if(idx < size)
	{
		if(idx > 3) // threads with their id more than 3 will access the location id+1
			idx++;

		dest[idx] = src[idx];
	}


}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// access pattern involves accessing contiguous float3 elements' first component by the contiguous threads
// the starting address is aligned
// the sixth scenario from the performance guidelines of the programming guide
// non-coalesced float3 memory access, resulting in 16 memory transactions
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

__global__ void nonCoalescedGMAccessFloat3(float3 *src, float* dest,long size)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;

	if(idx < size)
	{
		dest[idx] = src[idx].x;
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




// one extra location to hold results for cudaMemcpy also other than the 6 patterns 

static double  avgtime[NO_OF_PATTERNS+1] = {0}, maxtime[NO_OF_PATTERNS+1] = {0}, mintime[NO_OF_PATTERNS+1];
static float bandWidths[NO_OF_PATTERNS+1] = {0}; 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// the main routene 
// for timing the different access patterns 
// finding the band widths
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc,char* argv[])
{
	float *srcArray , *destArray;
	float3 *srcArray3; // the array of float3 datatype
	float elapsedTimes[NO_OF_PATTERNS+1][NTIMES];
	cudaEvent_t start,stop;
	cudaError_t err = cudaSuccess;
	
	double bytes = 2 * sizeof(float) * ARRAY_SIZE;

	// allocating the memory for the two arrays on the device
	// the memory allocated using cudaMalloc will give the memory address that is aligned (128)
	err = cudaMalloc((void **)&srcArray,ARRAY_SIZE*sizeof(float));
	if(err == cudaErrorMemoryAllocation)
	{
		printf("error in device memory allocation for - srcArray\n exiting out of the program.....\n");
		exit(-1);
	}
	
	err = cudaMalloc((void **)&destArray,ARRAY_SIZE*sizeof(float));
	if(err == cudaErrorMemoryAllocation)
	{
		printf("error in device memory allocation for -  destArray\n exiting out of the program.....\n");
		exit(-1);
	}
	
	err = cudaMalloc((void **)&srcArray3,ARRAY_SIZE*sizeof(float3));
	if(err == cudaErrorMemoryAllocation)
	{
		printf("error in device memory allocation for - srcArray3\n exiting out of the program.....\n");
		exit(-1);
	}
	
	// event creation, which will be used for timing the code
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	
	//finding the 1D grid size 
	int gridSize = ARRAY_SIZE/BLOCK_SIZE;
	
	if( ARRAY_SIZE % BLOCK_SIZE != 0 ) 
			gridSize += 1;
	
	// intializing the arrays on the device
	setArray <<< gridSize,BLOCK_SIZE >>> (srcArray,1.0f,ARRAY_SIZE);
	setArray <<< gridSize,BLOCK_SIZE >>> (destArray,0.0f,ARRAY_SIZE);

	cudaThreadSynchronize();

	// running each pattern NTIMES
	for(int k=0; k < NTIMES; k++)
	{
		// timing the kernels corresponding to different access patterns
			
			// PATTERN 1
   		// timing the code with coalesced float memory access, resulting in a single memory transaction from the Global memory
			cudaEventRecord(start,0);
			coalescedGMAccess <<< gridSize,BLOCK_SIZE >>> (destArray,srcArray,ARRAY_SIZE);
			cudaEventRecord(stop,0);
			cudaEventSynchronize(stop);
			cudaEventElapsedTime(&elapsedTimes[0][k],start,stop);


			// PATTERN 2
			// timing the code with coalesced float memory access (divergent warp), resulting in a single memory transaction from the Global memory
			cudaEventRecord(start,0);
			coalescedGMAccessDivergent <<< gridSize,BLOCK_SIZE >>> (destArray,srcArray,ARRAY_SIZE);
			cudaEventRecord(stop,0);
			cudaEventSynchronize(stop);
			cudaEventElapsedTime(&elapsedTimes[1][k],start,stop);


			// PATTERN 3
			//timing the code with non-sequential float memory access, resulting in 16 memory transactions from the Global memory
			cudaEventRecord(start,0);
			nonCoalescedGMAccessNonSeq <<< gridSize,BLOCK_SIZE >>> (destArray,srcArray,ARRAY_SIZE);
			cudaEventRecord(stop,0);
			cudaEventSynchronize(stop);
			cudaEventElapsedTime(&elapsedTimes[2][k],start,stop);


			// PATTERN 4
			//timing the code of access pattern with a misaligned starting address, resulting in 16 memory transactions from the Global memory
			cudaEventRecord(start,0);
			nonCoalescedGMAccessMisalign <<< gridSize,BLOCK_SIZE >>> (destArray,srcArray,ARRAY_SIZE);
			cudaEventRecord(stop,0);
			cudaEventSynchronize(stop);
			cudaEventElapsedTime(&elapsedTimes[3][k],start,stop);


			//PATTERN 5
			//timing the code with non-contiguous float memory access, resulting in 16 memory transactions
			cudaEventRecord(start,0);
			nonCoalescedGMAccessNonContiguous <<< gridSize,BLOCK_SIZE >>> (destArray,srcArray,ARRAY_SIZE);
			cudaEventRecord(stop,0);
			cudaEventSynchronize(stop);
			cudaEventElapsedTime(&elapsedTimes[4][k],start,stop);


			//PATTERN 6
			//timing the code for no-coalesced access of float3 array
			cudaEventRecord(start,0);
			nonCoalescedGMAccessFloat3 <<< gridSize,BLOCK_SIZE >>> (srcArray3,destArray,ARRAY_SIZE);
			cudaEventRecord(stop,0);
			cudaEventSynchronize(stop);
			cudaEventElapsedTime(&elapsedTimes[5][k],start,stop);
			
			//PATTERN 7
			//timing the code that uses the cudaMemcpy API call
			cudaEventRecord(start,0);
			cudaMemcpy(destArray,srcArray,ARRAY_SIZE * sizeof(float),cudaMemcpyDeviceToDevice);
			cudaEventRecord(stop,0);
			cudaEventSynchronize(stop);
			cudaEventElapsedTime(&elapsedTimes[6][k],start,stop);

	} // end of the for loop involving NTIMES


	// intializing the mintime array
	for(int i=0; i < NO_OF_PATTERNS+1;i++)
		mintime[i] = FLT_MAX;


   for (int k=1; k < NTIMES; k++) // skiping the first iteration
	{
	   for (int i=0; i < NO_OF_PATTERNS+1; i++)
	   {
	       avgtime[i] = avgtime[i] + elapsedTimes[i][k];
	       mintime[i] = MIN(mintime[i],elapsedTimes[i][k]);
	       maxtime[i] = MAX(maxtime[i], elapsedTimes[i][k]);
	   }
	}


	// calculation of the different band widths that are achieved by different access patterns that are given in the performane guide lines 
	
	for(int i=0; i < NO_OF_PATTERNS+1; i++)
	{
		avgtime[i] = avgtime[i]/(double)(NTIMES-1); // finding the average time
		bandWidths[i] = bytes/mintime[i];
	}

	printResults();

	printf("\n\n**** successful termination of the program  ****\n\n");
		

	cudaEventDestroy(start);
	cudaEventDestroy(stop);

	cudaFree(srcArray);
	cudaFree(destArray);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// prints the results containig the minimum, maximum, average times that the different access patterns have taken
// the associated maximum bandwidth of the different patterns
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void printResults()
{

  printf("\nComparing different access patterns of the global memory for the devices with compute capability <= 1.1\n");
  printf("The array size (single precision): %d\n",ARRAY_SIZE);

  printf("\n-------------------------------------------------------------------------------------------------------------------------------\n");
  printf("Pattern \t\t\t\t Bandwidth (GB/sec) \t Avg time (ms)  \t Min time (ms)  \t Max time (ms)\n");
  printf("-------------------------------------------------------------------------------------------------------------------------------\n");

	// printing the results for different access patterns
	for(int i=0; i < NO_OF_PATTERNS+1; i++)
	{
		
		switch(i)
		{
			case 0: printf("coalesced access                  ");
					  break;

			case 1: printf("coalesced access (divergent warp) ");
					  break;
			
			case 2: printf("non-sequential access             ");
					  break;

			case 3: printf("accessing  misaligned address     ");
					  break;

			case 4: printf("non-contiguous access             ");
					  break;

			case 5: printf("non-coalesced float3 access       ");
					  break;

			case 6: printf("using cudaMemcpy                  ");
					  break;

		}
		
		printf("\t %.6f \t\t %f \t\t  %f \t\t  %f\n",bandWidths[i]/1000000,avgtime[i],mintime[i],maxtime[i]);

  }
	
  
  printf("\n ------------------------------------------------------------------------------------------------------------------------------\n");

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

