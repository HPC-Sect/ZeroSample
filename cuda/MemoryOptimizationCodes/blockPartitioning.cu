
#include <stdio.h>
#include <cuda.h>

#define ARRAY_SIZE 2097120
#define N 5
#define HLINE "----------------------------------------------------\n"
#define NTIMES 10

void printResults();
void printDeviceDetails();
void cudaSafeMalloc(void ** , size_t );
void CudaGetDeviceProperties(cudaDeviceProp *, int);
void CudaGetDevice(int *);
void checkCudaErrors();


float avgTime[N] = {0};

static int blockSize[] = {32,64,128,256,512};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Simple Copy Kernel
// It copies one array on device to other
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

__global__ void simpleCopyKernel(float* dest,float* src,long size)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
	if(idx < size)
		dest[idx] = src[idx];
}




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Kernel for initializing the array on device  with given element
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

__global__ void setArray(float *array,  float value, int size)
{
	int idx = threadIdx.x + blockIdx.x * blockDim.x;
	if (idx < size)
		array[idx] = value;
}






////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main Routine 
// for timing the copy kernel printing the results
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc,char* argv[])
{
	int i,j;
	float *d_srcArray , *d_destArray;
	//float bandWidths[N];
	float elapsedTimes[N][NTIMES];
	cudaEvent_t start,stop;
	

	int gridSize[N];
	
	// allocating the memory for the two arrays on the device
	cudaSafeMalloc((void **)&d_srcArray,ARRAY_SIZE*sizeof(float));
	cudaSafeMalloc((void **)&d_destArray,ARRAY_SIZE*sizeof(float));
	
	// event creation, which will be used for timing the code
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	
	for(j=0; j< NTIMES; j++)
	{
		for(i=0; i<N; i++)
		{
			//finding the 1D grid size 
			gridSize[i] = ARRAY_SIZE/blockSize[i];
	
			if( ARRAY_SIZE % blockSize[i] != 0)
			gridSize[i] += 1;
	
			// intializing the arrays on the device
			setArray <<< gridSize[i],blockSize[i] >>> (d_srcArray,1.0f,ARRAY_SIZE);
			setArray <<< gridSize[i],blockSize[i] >>> (d_destArray,0.0f,ARRAY_SIZE);

			cudaThreadSynchronize();

   		// timing the copy routine for different Block Sizes 
			cudaEventRecord(start,0);
			simpleCopyKernel <<< gridSize[i],blockSize[i] >>> (d_destArray,d_srcArray,ARRAY_SIZE);
			cudaEventRecord(stop,0);
			cudaEventSynchronize(stop);
			cudaEventElapsedTime(&elapsedTimes[i][j],start,stop);

		}
	
	}
	
	//Computing average time
	for(i=0; i<N; i++)
	{
		for(j=1; j<NTIMES; j++) //skipping first iteration
		{
			avgTime[i] += elapsedTimes[i][j];
		}
		avgTime[i] = avgTime[i]/(NTIMES-1);
	}
	
	// Printing the results 
	printResults();
	
	cudaEventDestroy(start);
	cudaEventDestroy(stop);

	return 0;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Host Function to print the results
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void printResults()
{
	int j;
	printf("\n\n");
	printf(HLINE);
	printf("BLOCK PARTITIONING ANALYSIS\n");
	printf(HLINE);
	printDeviceDetails();
	printf(HLINE);
	printf("Array Size = %llu\n",(unsigned long long)ARRAY_SIZE);
	//printf("Block Size = %d\n",(int)BLOCK_SIZE);
	printf(HLINE);
 
	printf("Block Size    Rate (GB/s)   Avg time  \n");
   for (j=0; j<N; j++) 
	{
		printf("%3.1d%20.4f  %11.4f  \n", blockSize[j], 1.0E-06 * (2 * ARRAY_SIZE * sizeof(float))/avgTime[j],avgTime[j]);
	} 
	
	printf(HLINE);
}

void printDeviceDetails()
{
	int deviceId;
	cudaDeviceProp prop;
	CudaGetDevice(&deviceId);
	 
	CudaGetDeviceProperties(&prop, deviceId);
	
	printf("Device Name is %s\n", prop.name);
	//printf("Clock Rate of this device is %f GHz\n",(float)prop.clockRate * 1.0E-06);
	printf("Compute Capability of this device is %d.%d\n",prop.major,prop.minor);
	printf("Number of Multiprocessors = %d\n", prop.multiProcessorCount);
	printf("Max no. of blocks allowed in a 1D Grid = %d\n", prop.maxGridSize[0]);
	//printf("Max no. of threads allowed in 1D block = %d\n", prop.maxThreadsDim[0]);
	printf("Max no. of threads allowed in a block = %d\n", prop.maxThreadsPerBlock);
	//printf("No. of registers per block = %d\n", prop.regsPerBlock);
	//printf("Shared Memory Per block (in KB) = %f\n", (float)prop.sharedMemPerBlock * 1.0E-03);
	printf("Total Global Memory available = %f GB\n",(float)prop.totalGlobalMem * 1.0E-09);
	printf("Warp Size in threads = %d\n",prop.warpSize);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Wrapper Fuctions for error checking
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void cudaSafeMalloc(void ** devicePtr, size_t size)
{
	cudaMalloc(devicePtr, size);
	checkCudaErrors();
}

void CudaGetDeviceProperties(cudaDeviceProp *devicePropPtr, int deviceId)
{
	cudaGetDeviceProperties(devicePropPtr, deviceId);
	checkCudaErrors();
}

void CudaGetDevice(int *deviceIdPtr)
{
	cudaGetDevice(deviceIdPtr);
	checkCudaErrors();
}

void checkCudaErrors()
{
	cudaError_t error = cudaGetLastError();
	if(error != cudaSuccess)
	{   
   	printf("Cuda Error: %s\n",cudaGetErrorString(error));
   	cudaThreadExit();
		exit(-1);
	}
}
