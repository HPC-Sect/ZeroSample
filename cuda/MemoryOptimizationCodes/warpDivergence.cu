
/************************************************************************
	
       Source Code  : warpDivergence.cu                                      

        Objective   : To demonstrate the difference in bandwidth achieved when     
                      threads within a warp follow different execution paths       
                      This Program measures the bandwidth of global memory         
                      for the initialization operation [a(i) = value] using 
                      NVIDIA GPU which has a SIMT architecture                           

        Output      : Bandwidth achieved and timing(average) for initialization
                      kernels with varying no. of  execution paths               

    	Modified    : Aug 2011
                      

	Author 	    : RarchK		

****************************************************************************/

#include <stdio.h>
#include <cuda.h>

#define ARRAY_SIZE 5120000
#define BLOCK_SIZE 128
#define NTIMES 100
#define HLINE "--------------------------------------------------------------\n"

void printResults();
void printDeviceDetails();
void cudaSafeMalloc(void ** , size_t );
void CudaGetDeviceProperties(cudaDeviceProp *, int);
void CudaGetDevice(int *);
void checkCudaErrors();

float avgTime[4] = {0};
char *label[] = {"With 3 branch instructions ","With 2 branch instructions ","With 1 branch instruction  ","With no branch instructions"};


///////////////////////////////////////////////////////////////////////////////////////////////////////
// Warp Divergence Kernel1 : 4 execution paths within a warp
///////////////////////////////////////////////////////////////////////////////////////////////////////

__global__ void warpDivergence1(float *dest,  float value1,float value2, int size)
{
	int idx = threadIdx.x + blockIdx.x * blockDim.x;
	if(idx < size)
	{
		if(idx % 5 == 0)
			dest[idx] = value1;
		else if(idx % 3 == 0)
			dest[idx] = value2;
		else if(idx % 2 == 0)
			dest[idx] = value1;
		else
			dest[idx] = value2; 
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Warp Divergence Kernel2 : 3 execution paths within a warp
///////////////////////////////////////////////////////////////////////////////////////////////////////

__global__ void warpDivergence2(float *dest,  float value1, float value2, int size)
{
	int idx = threadIdx.x + blockIdx.x * blockDim.x;
	if(idx < size)
	{
		if ( idx % 3 == 0)
			dest[idx] = value1;
		else if( threadIdx.x % 2 == 0)
			dest[idx] = value2;
		else
			dest[idx] = value1;
	}
}



///////////////////////////////////////////////////////////////////////////////////////////////////////
// Warp Divergence Kernel3 : 1 branch within a warp leading to 2 execution paths within a warp
///////////////////////////////////////////////////////////////////////////////////////////////////////

__global__ void warpDivergence3(float *dest,  float value1, float value2, int size)
{
	int idx = threadIdx.x + blockIdx.x * blockDim.x;
	if(idx < size)
	{
		if ( idx % 2 == 0)
			dest[idx] = value1;
		else
			dest[idx] = value2;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
// Warp Divergence Kernel4 : No branches within a warp i.e. single execution path
///////////////////////////////////////////////////////////////////////////////////////////////////////

__global__ void warpDivergence4(float *dest,  float value1, float value2, int size)
{
	int idx = threadIdx.x + blockIdx.x * blockDim.x;
	if(idx < size)
		dest[idx] = value1;
}




///////////////////////////////////////////////////////////////////////////////////////////////////////
// Main function to time all the kernels
//////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
	float *d_dest;
	int i,j;	
	float elapsedTimes[4][NTIMES];
	cudaEvent_t start,stop;
	size_t size = 	ARRAY_SIZE *sizeof(float);
	
	// event creation, which will be used for timing the code
	cudaEventCreate(&start);
	cudaEventCreate(&stop);

	cudaSafeMalloc((void **) &d_dest, size);

	
	int gridSize = ARRAY_SIZE/BLOCK_SIZE;
	if(ARRAY_SIZE % BLOCK_SIZE != 0)
		gridSize += 1;
	dim3 grid,block;
	block.x = BLOCK_SIZE;
	grid.x = gridSize;

	for(i=0; i<NTIMES; i++)
	{

	   // timing the initialization kernel1
		cudaEventRecord(start,0);
		warpDivergence1<<<grid, block>>>(d_dest, 1.0,2.0,size);
		cudaEventRecord(stop,0);
		cudaEventSynchronize(stop);
		cudaEventElapsedTime(&elapsedTimes[0][i],start,stop);
		checkCudaErrors();


	   // timing the initialization kernel2
		cudaEventRecord(start,0);
		warpDivergence2<<<grid, block>>>(d_dest, 1.0,2.0,size);
		cudaEventRecord(stop,0);
		cudaEventSynchronize(stop);
		cudaEventElapsedTime(&elapsedTimes[1][i],start,stop);
		checkCudaErrors();
	   
		// timing the initialization kernel3
		cudaEventRecord(start,0);
		warpDivergence3<<<grid, block>>>(d_dest, 1.0,2.0,size);
		cudaEventRecord(stop,0);
		cudaEventSynchronize(stop);
		cudaEventElapsedTime(&elapsedTimes[2][i],start,stop);
		checkCudaErrors();
	   

		// timing the initialization kernel4
		cudaEventRecord(start,0);
		warpDivergence4<<<grid, block>>>(d_dest, 1.0,2.0,size);
		cudaEventRecord(stop,0);
		cudaEventSynchronize(stop);
		cudaEventElapsedTime(&elapsedTimes[3][i],start,stop);
		checkCudaErrors();
	}
	
	//Computing average time taken
	for(i=0; i<4; i++)
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
	printf("WARP DIVERGENCE DEMONSTRATION\n");
	printf(HLINE);
	printDeviceDetails();
	printf(HLINE);
	printf("Array Size = %llu\n",(unsigned long long)ARRAY_SIZE);
	printf("Block Size = %d\n",(int)BLOCK_SIZE);
	printf(HLINE);
 
	printf("Initialization Kernels        Rate (GB/s)   Avg time  \n");
   for (j=0; j<4; j++) 
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
	cudaError_t error = cudaGetLastError();
	if(error != cudaSuccess)
	{   
   	printf("Cuda Error: %s\n",cudaGetErrorString(error));
   	cudaThreadExit();
		exit(-1);
	}
}
void CudaGetDeviceProperties(cudaDeviceProp *devicePropPtr, int deviceId)
{
	cudaGetDeviceProperties(devicePropPtr, deviceId);
	cudaError_t error = cudaGetLastError();
	if(error != cudaSuccess)
	{   
   	printf("Cuda Error: %s\n",cudaGetErrorString(error));
   	cudaThreadExit();
		exit(-1);
	}
}
void CudaGetDevice(int *deviceIdPtr)
{
	cudaGetDevice(deviceIdPtr);
	cudaError_t error = cudaGetLastError();
	if(error != cudaSuccess)
	{   
   	printf("Cuda Error: %s\n",cudaGetErrorString(error));
   	cudaThreadExit();
		exit(-1);
	}
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
