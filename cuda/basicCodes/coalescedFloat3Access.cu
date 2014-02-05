
#include <cuda.h>
#include <stdio.h>
#include <float.h>


#define ARRAY_SIZE 2000000  // 2 MB
#define BLOCK_SIZE 256 // with 512 block size the shared memory requirement will overshoot the available limit

#define NTIMES 10

#define MIN(x,y) ((x)<(y)?(x):(y))
#define MAX(x,y) ((x)>(y)?(x):(y))



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// kernel for copying the input array of  float3 datatype into another array of float3 
// the access pattern is not coalesced
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

__global__ void nonCoalescedFloat3Access(float3 *dest, float3* src,long size)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;

	if(idx < size)
	{
		dest[idx].x = src[idx].x;
		dest[idx].y = src[idx].y;
		dest[idx].z = src[idx].z;
	}

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// kernel for copying the input array of  float3 datatype into another array of float3 
// the access pattern is made coalesced using Shared memory
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

__global__ void coalescedFloat3Access(float* dest, float* src,long size)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;

	 __shared__ float sInArray[3 * BLOCK_SIZE * sizeof(float)];

	if(idx < size)
	{
	   // loading into the shared memory from the device global memory
		sInArray[threadIdx.x] = src[idx];
		sInArray[threadIdx.x + BLOCK_SIZE] = src[idx + BLOCK_SIZE];
		sInArray[threadIdx.x + (2*BLOCK_SIZE)] = src[idx + (2*BLOCK_SIZE)];

		__syncthreads(); //syncronizing before writing into the dest array in global memory

		dest[idx] = sInArray[threadIdx.x];
		dest[idx + BLOCK_SIZE] = sInArray[threadIdx.x + BLOCK_SIZE];
		dest[idx + (2*BLOCK_SIZE)] = sInArray[threadIdx.x + (2*BLOCK_SIZE)];
	}

}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// kernel for setting the array of float3 with given element
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

__global__ void setArray(float3 *array,  float3 value, int size)
{
	int idx = threadIdx.x + blockIdx.x * blockDim.x;
	if (idx < size)
	{
		array[idx].x = value.x;
		array[idx].y = value.y;
		array[idx].z = value.z;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////






////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// the main routene 
// for timing the two access patterns -- non-coalesced float3 access and coalesced float3 access using the shared memory 
// finding the band width for the two access patterns
// printing the results achieved
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc,char* argv[])
{  
  	double  avgtime[2] = {0}, maxtime[2] = {0}, mintime[2]={FLT_MAX,FLT_MAX};
   float bandWidths[2] = {0}; 
	float3 *srcArray , *destArray;
	float elapsedTimes[2][NTIMES];
	cudaEvent_t start,stop;
	cudaError_t err = cudaSuccess;
	
	double bytes = 2 * sizeof(float3) * ARRAY_SIZE;

	// allocating memory on hte device
	err = cudaMalloc((void **)&srcArray,ARRAY_SIZE*sizeof(float3));
	if(err == cudaErrorMemoryAllocation)
	{
		printf("error in device memory allocation for - srcArray\n exiting out of the program.....\n");
		exit(-1);
	}
	
	err = cudaMalloc((void **)&destArray,ARRAY_SIZE*sizeof(float3));
	if(err == cudaErrorMemoryAllocation)
	{
		printf("error in device memory allocation for - destArray\n exiting out of the program.....\n");
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
	setArray <<< gridSize,BLOCK_SIZE >>> (srcArray,make_float3 (1,2,3),ARRAY_SIZE);
	setArray <<< gridSize,BLOCK_SIZE >>> (destArray,make_float3 (0,0,0),ARRAY_SIZE);

	cudaThreadSynchronize();

	// timing the different kernels
	for(int i=0; i < NTIMES; i++)
	{
	   //timing the kernel with non-coalesced float3 access
		cudaEventRecord(start,0);
		nonCoalescedFloat3Access <<< gridSize,BLOCK_SIZE >>> (destArray,srcArray,ARRAY_SIZE); 
		cudaEventRecord(stop,0);
		cudaEventSynchronize(stop);
		cudaEventElapsedTime(&elapsedTimes[0][i],start,stop);

		//timing the kernel with coalesced float3 access
		cudaEventRecord(start,0);
		coalescedFloat3Access <<< gridSize,BLOCK_SIZE >>> ((float *)destArray,(float *)srcArray,ARRAY_SIZE);
		cudaEventRecord(stop,0);
		cudaEventSynchronize(stop);
		cudaEventElapsedTime(&elapsedTimes[1][i],start,stop);

	}

	// calculating max, min, average timings taken by the two kernels
   for (int i=1; i < NTIMES; i++) // skiping the first iteration
	{
	   for (int k=0; k < 2; k++)
	   {
	       avgtime[k] = avgtime[k] + elapsedTimes[k][i];
	       mintime[k] = MIN(mintime[k],elapsedTimes[k][i]);
	       maxtime[k] = MAX(maxtime[k], elapsedTimes[k][i]);
	   }

	}

	for(int i=0; i < 2; i++)
	{
		avgtime[i] = avgtime[i]/(double)(NTIMES-1); // finding the average time
		bandWidths[i] = bytes/mintime[i];
	}


   printf("\nDemonstrating the usage of shared memory for coalesced access of float3 array from the global memory of the devices with compute capability <= 1.1\n");
   printf("The array size (single precision): %d\n",ARRAY_SIZE);

   printf("\n-------------------------------------------------------------------------------------------------------------------------------\n");
   printf("Pattern \t\t\t\t Bandwidth (GB/sec) \t Avg time (ms)  \t Min time (ms)  \t Max time (ms)\n");
   printf("-------------------------------------------------------------------------------------------------------------------------------\n");

   //printing the results -- deifferent bandwidths achieved by the two kernels
   for(int i=0; i < 2; i++)
   {
		
		switch(i)
		{
			case 0: printf("Non-coalesced access                ");
					  break;

			case 1: printf("coalesced access using shared memory");
					  break;
		}

		printf("\t %.6f \t\t %f \t\t  %f \t\t  %f\n",bandWidths[i]/1000000,avgtime[i],mintime[i],maxtime[i]);
	}
  
   printf("\n ------------------------------------------------------------------------------------------------------------------------------\n");
	
	printf("\n\n**** successful termination of the program  ****\n\n");

	cudaEventDestroy(start);
	cudaEventDestroy(stop);

	cudaFree(srcArray);
	cudaFree(destArray);

	return 0;
} // end of main routene

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



