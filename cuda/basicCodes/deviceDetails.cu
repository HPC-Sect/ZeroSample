

/********************************************************************************************

	
  source Code : deviceDetails.cu

  Objective   : Example code to demonstrate the number of devices that are present on the 
                current system and their properties

  Description:  To query using the cuda API calls about the various properties of the devices like 
	        the device model,max number of threads per block, compute capability,warp size, 
                available Global, shared, and constant memories etc. 

  input: 	none

 output:	The various properties of all the devices that are present on the current system

**********************************************************************************************/

#include <cuda.h>
#include<stdio.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// main routene to find the gpu devices that are presented on the system
// querying the various details of all the devices that are presented and printing the details
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc,char* argv[])
{

int deviceCount;
cudaGetDeviceCount(&deviceCount);
int device;

for (device = 0; device < deviceCount; ++device) {
    cudaDeviceProp deviceProp;
	 cudaGetDeviceProperties(&deviceProp, device);
	 if (device == 0) 
	 {
       if (deviceProp.major == 9999 && deviceProp.minor == 9999)
		 {
	        printf("\n\nThere is no device supporting CUDA.\n");
			  break;
		 }
		 else
			printf("\n\nThere are %d device(s) supporting CUDA\n",deviceCount);
	 }
		 
	 
	 printf("\n\n********************* DEVICE-%d DETAILS *******************\n",device);
	 printf("The name of the device : %s\n",deviceProp.name);
	 printf("The compute capability : %d.%d\n",deviceProp.major,deviceProp.minor);
	 printf("The warp size : %d\n",deviceProp.warpSize);
	 printf("The Global memory available on device : %lf GBytes\n",(double)deviceProp.totalGlobalMem/1000000000);
	 printf("The Constant memory available on device: %ld Bytes\n",deviceProp.totalConstMem);
	 printf("The shared memory available per Block : %ld Bytes\n",deviceProp.sharedMemPerBlock);
	 printf("The registers available per Block : %d\n",deviceProp.regsPerBlock);
	 printf("The number of multiprocessors on the device : %d\n",deviceProp.multiProcessorCount);
	 printf("The max number of threads per Block : %d\n",deviceProp.maxThreadsPerBlock);
	 printf("The max sizes of each dimension of a block: (%d,%d,%d)\n",deviceProp.maxThreadsDim[0],deviceProp.maxThreadsDim[1],deviceProp.maxThreadsDim[2]);
	 printf("The max sizes of each dimension of a grid: (%d,%d,%d)\n",deviceProp.maxGridSize[0],deviceProp.maxGridSize[1],deviceProp.maxGridSize[2]);
	 printf("----------------------------------------------------------\n\n");
	
}


return 0;

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
