

/************************************************************************
	
       Source Code  : partitionCamping.cu  

       Objective    : To demonstrate the difference in bandwidth achieved when    
                      blocks access global memory with and without partition     
                      camping. This Program measures the bandwidth of global 
                      memory for the initialization operation [a(i) = value] 
                      using NVIDIA GPU

	Input	    : None 

        Output      : Bandwidth achieved and timing (average) 

    	Modified    :   Aug 2011
                    

	Author 	    : RarchK		

*************************************************************************/

#include <stdio.h>
#include <cuda.h>

#define ARRAY_SIZE 2195264
#define BLOCK_SIZE 64
#define NTIMES 2
#define HLINE "--------------------------------------------------------------\n"

void printResults();
void printDeviceDetails();
void cudaSafeMalloc(void ** , size_t );
void CudaGetDeviceProperties(cudaDeviceProp *, int);
void CudaGetDevice(int *);
void checkCudaErrors();


float avgTime[2] = {0};
char *label[] = {"Without Partition Camping","With Partition Camping   "};

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Kernel for initializing the array without Partition Camping
///////////////////////////////////////////////////////////////////////////////////////////////////////

__global__ void initializationWithoutPartitionCamping(float *array,  float value, int size)
{
	int idx = threadIdx.x + blockIdx.x * blockDim.x;
	if (idx < size)
		array[idx] = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////
// Kernel for initializing the array with Partition Camping
///////////////////////////////////////////////////////////////////////////////////////////////////////

__global__ void initializationWithPartitionCamping(float *array,  float value, int size)
{
	int blockIdx_x;
	blockIdx_x =  (blockIdx.x * 6) % gridDim.x;
	int idx = threadIdx.x + blockIdx_x * blockDim.x;
	if (idx < size)
		array[idx] = value;
}



int main()
{
	float *d_array;
	size_t size = ARRAY_SIZE *	sizeof(float);
	int i,j;	
	float elapsedTimes[2][NTIMES];
	cudaEvent_t start,stop;

	
	// event creation, which will be used for timing the code
	cudaEventCreate(&start);
	cudaEventCreate(&stop);

	cudaSafeMalloc((void **) &d_array, size);

	int gridSize = ARRAY_SIZE / BLOCK_SIZE;
	if(ARRAY_SIZE % BLOCK_SIZE != 0) gridSize += 1;
	dim3 grid, block;
	block.x = BLOCK_SIZE;
	grid.x = gridSize;

	for(i=0; i<NTIMES; i++)
	{

	   // timing the initialization without Partition Camping
		cudaEventRecord(start,0);
		initializationWithoutPartitionCamping<<<grid, block>>>(d_array, 1.0f, ARRAY_SIZE);
		cudaEventRecord(stop,0);
		cudaEventSynchronize(stop);
		cudaEventElapsedTime(&elapsedTimes[0][i],start,stop);
		checkCudaErrors();


	   // timing the initialization with Partition Camping
		cudaEventRecord(start,0);
		initializationWithPartitionCamping<<< grid, block>>>(d_array, 1.0f, ARRAY_SIZE);
		cudaEventRecord(stop,0);
		cudaEventSynchronize(stop);
		cudaEventElapsedTime(&elapsedTimes[1][i],start,stop);
		checkCudaErrors();
	}


	//Computing average time taken
	for(i=0; i<2; i++)
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
	printf("PARTITION CAMPING DEMONSTRATION\n");
	printf(HLINE);
	printDeviceDetails();
	printf(HLINE);
	printf("Array Size = %llu\n",(unsigned long long)ARRAY_SIZE);
	printf("Block Size = %d\n",(int)BLOCK_SIZE);
	printf(HLINE);
 
	printf("Initialization               Rate (GB/s)   Avg time  \n");
   for (j=0; j<2; j++) 
	{
		printf("%s%11.4f  %11.4f  \n", label[j], 1.0E-06 * (ARRAY_SIZE * sizeof(float))/avgTime[j],avgTime[j]);
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
	//printf("Number of Multiprocessors = %d\n", prop.multiProcessorCount);
	//printf("Max no. of blocks allowed in a 1D Grid = %d\n", prop.maxGridSize[0]);
	//printf("Max no. of threads allowed in 1D block = %d\n", prop.maxThreadsDim[0]);
	//printf("Max no. of threads allowed in a block = %d\n", prop.maxThreadsPerBlock);
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
