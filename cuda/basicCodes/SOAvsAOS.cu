

/******************************************************************************************

	
  Source Code :  SOAvsAOS.cu

  Objective   : Example code to demonstrate the advantage of having Stucture of arrays rather 
                than array of structures in the application while representing data the 
                corresponding advantages in terms of the bandwidth of the global memory that 
                is achievable

  Description : This example takes "Triangle" as structure with three arrays of three floating 
                points each representing the three vertices of a triangle the same information 
                is also is stored using a structure "Triangles" which has arrays for each field 
                of each vertex  both the representations are intialised generating typical 
                access patterns that will be present while accessing those structures

  Input       :	none

  Output      :	The different bandwidths that are achieved while accessing data from different 
                data representations

 Modified     : Aug 2011
                

 Author	      : RarchK		

****************************************************************************************/

#include <stdio.h>
#include <cuda.h>
#include <float.h>

#define NO_OF_TRIANGLES 1000000 
#define BLOCK_SIZE 128


#define NO_OF_PATTERNS 2
#define NTIMES 10


#define MIN(x,y) ((x)<(y)?(x):(y))
#define MAX(x,y) ((x)>(y)?(x):(y))

void printResults(void);

// Triangle structure -- which is the structure in "Array of Structures"
struct Triangle {
						float A[3];
						float B[3];
						float C[3];
					 };

// Triangles structure -- which is the structure in "Structure of Arrays"
struct Triangles {
						float *Ax, *Ay, *Az;
						float *Bx, *By, *Bz;
						float *Cx, *Cy, *Cz;
					  };


/////////////////////////////////////////////////////////////////////////////////////
//
// initializing the array of Triangle types.
// each element is intialized to -- (0,0,0),(1,1,1),(2,2,2)
//
////////////////////////////////////////////////////////////////////////////////////

__global__ void setTriangles(Triangle *myArrayOfTriangles)
{
	int idx = threadIdx.x + blockIdx.x*blockDim.x;

	if(idx < NO_OF_TRIANGLES)
	{
		myArrayOfTriangles[idx].A[0] = 0;
		myArrayOfTriangles[idx].A[1] = 0;
		myArrayOfTriangles[idx].A[2] = 0;
		myArrayOfTriangles[idx].B[0] = 1;
		myArrayOfTriangles[idx].B[1] = 1;
		myArrayOfTriangles[idx].B[2] = 1;
		myArrayOfTriangles[idx].C[0] = 2;
		myArrayOfTriangles[idx].C[1] = 2;
		myArrayOfTriangles[idx].C[2] = 2;
	}
}

/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
//
// initializing the Triangles structure
// each triangle's vertices are initialized to -- (0,0,0),(1,1,1),(2,2,2)
//
////////////////////////////////////////////////////////////////////////////////////

__global__ void setTriangles2(Triangles myTrianglesStructure)
{
	int idx = threadIdx.x + blockIdx.x*blockDim.x;

	if(idx < NO_OF_TRIANGLES)
	{
		myTrianglesStructure.Ax[idx] = 0;
		myTrianglesStructure.Ay[idx] = 0;
		myTrianglesStructure.Az[idx] = 0;
		myTrianglesStructure.Bx[idx] = 1;
		myTrianglesStructure.By[idx] = 1;
		myTrianglesStructure.Bz[idx] = 1;
		myTrianglesStructure.Cx[idx] = 2;
		myTrianglesStructure.Cy[idx] = 2;
		myTrianglesStructure.Cz[idx] = 2;
	}
}

////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////
//
// checking for the equivalence of both the initialising kernals
// 
//////////////////////////////////////////////////////////////////////////////////////////////////////

__global__ void Test(Triangle *myArrayOfTriangles, Triangles myTrianglesStructure, int* dCorrectBool)
{

	int idx = threadIdx.x + blockIdx.x*blockDim.x;
	if(idx < NO_OF_TRIANGLES)
	if(myTrianglesStructure.Ax[idx] != myArrayOfTriangles[idx].A[0]
		||	myTrianglesStructure.Ay[idx] !=  myArrayOfTriangles[idx].A[1]
		|| myTrianglesStructure.Az[idx] !=  myArrayOfTriangles[idx].A[2]
		|| myTrianglesStructure.Bx[idx] !=  myArrayOfTriangles[idx].B[0]
		|| myTrianglesStructure.By[idx] !=  myArrayOfTriangles[idx].B[1]
		|| myTrianglesStructure.Bz[idx] !=  myArrayOfTriangles[idx].B[2]
		|| myTrianglesStructure.Cx[idx] !=  myArrayOfTriangles[idx].C[0]
		|| myTrianglesStructure.Cy[idx] !=  myArrayOfTriangles[idx].C[1]
		|| myTrianglesStructure.Cz[idx] !=  myArrayOfTriangles[idx].C[2] )
	
		*dCorrectBool = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////




static double  avgtime[NO_OF_PATTERNS] = {0}, maxtime[NO_OF_PATTERNS] = {0}, mintime[NO_OF_PATTERNS];
static float bandWidths[NO_OF_PATTERNS] = {0}; 
static double bytes = sizeof(float) * NO_OF_TRIANGLES * 9;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// the main routene 
// for timing the different kernals which are accessing array of structures and structure of arrays
// finding the bandwidths
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
	Triangle *myArrayOfTriangles;          // Array of Structures -- array of struct Triangle type
	Triangles myTrianglesStructure;        // Structure of Arrays
	int* dCorrectBool;                     // on the device variable
	int hCorrectBool = 1;

	float elapsedTimes[NO_OF_PATTERNS][NTIMES];
	cudaEvent_t start,stop;
	cudaError_t err = cudaSuccess;
	
	
	// event creation, which will be used for timing the code
	cudaEventCreate(&start);
	cudaEventCreate(&stop);


	// memory allocation on the device

	err = cudaMalloc((void **)&myArrayOfTriangles,sizeof(Triangle)*NO_OF_TRIANGLES);
	if(err == cudaErrorMemoryAllocation)
	{
		printf("error in device memory allocation for - myArrayOfTriangles\n exiting out of the program.....\n");
		exit(-1);
	}
	
	err = cudaMalloc((void **)&myTrianglesStructure.Ax,sizeof(float)*NO_OF_TRIANGLES);
	if(err == cudaErrorMemoryAllocation)
	{
		printf("error in device memory allocation for - myTrianglesStructure.Ax\n exiting out of the program.....\n");
		exit(-1);
	}

	err = cudaMalloc((void **)&myTrianglesStructure.Ay,sizeof(float)*NO_OF_TRIANGLES);
	if(err == cudaErrorMemoryAllocation)
	{
		printf("error in device memory allocation for - myTrianglesStructure.Ay\n exiting out of the program.....\n");
		exit(-1);
	}
	
	err = cudaMalloc((void **)&myTrianglesStructure.Az,sizeof(float)*NO_OF_TRIANGLES);
	if(err == cudaErrorMemoryAllocation)
	{
		printf("error in device memory allocation for - myTrianglesStructure.Az\n exiting out of the program.....\n");
		exit(-1);
	}
	
	err = cudaMalloc((void **)&myTrianglesStructure.Bx,sizeof(float)*NO_OF_TRIANGLES);
	if(err == cudaErrorMemoryAllocation)
	{
		printf("error in device memory allocation for - myTrianglesStructure.Bx\n exiting out of the program.....\n");
		exit(-1);
	}
	
	err = cudaMalloc((void **)&myTrianglesStructure.By,sizeof(float)*NO_OF_TRIANGLES);
	if(err == cudaErrorMemoryAllocation)
	{
		printf("error in device memory allocation for - myTrianglesStructure.By\n exiting out of the program.....\n");
		exit(-1);
	}
	
	err = cudaMalloc((void **)&myTrianglesStructure.Bz,sizeof(float)*NO_OF_TRIANGLES);
	if(err == cudaErrorMemoryAllocation)
	{
		printf("error in device memory allocation for - myTrianglesStructure.Bz\n exiting out of the program.....\n");
		exit(-1);
	}
	
	err = cudaMalloc((void **)&myTrianglesStructure.Cx,sizeof(float)*NO_OF_TRIANGLES);
	if(err == cudaErrorMemoryAllocation)
	{
		printf("error in device memory allocation for - myTrianglesStructure.Cx\n exiting out of the program.....\n");
		exit(-1);
	}
	
	err = cudaMalloc((void **)&myTrianglesStructure.Cy,sizeof(float)*NO_OF_TRIANGLES);
	if(err == cudaErrorMemoryAllocation)
	{
		printf("error in device memory allocation for - myTrianglesStructure.Cy\n exiting out of the program.....\n");
		exit(-1);
	}
	
	err = cudaMalloc((void **)&myTrianglesStructure.Cz,sizeof(float)*NO_OF_TRIANGLES);
	if(err == cudaErrorMemoryAllocation)
	{
		printf("error in device memory allocation for - myTrianglesStructure.Cz\n exiting out of the program.....\n");
		exit(-1);
	}
	
	err = cudaMalloc((void**)&dCorrectBool, sizeof(int));
	if(err == cudaErrorMemoryAllocation)
	{
		printf("error in device memory allocation for dCorrectBool\n exiting out of the program.....\n");
		exit(-1);
	}

	// copying hCorrectBool into dCorrectBool
	cudaMemcpy(dCorrectBool,&hCorrectBool,sizeof(int),cudaMemcpyHostToDevice);


	//finding the 1D grid size 
	int gridSize = NO_OF_TRIANGLES/BLOCK_SIZE;
	
	if( NO_OF_TRIANGLES % BLOCK_SIZE != 0 ) 
			gridSize += 1;

	// running each pattern NTIMES
	for(int k=0; k < NTIMES; k++)
	{
		// timing the kernels corresponding to different access patterns
			
			// PATTERN 1
			cudaEventRecord(start,0);
			setTriangles <<< gridSize , BLOCK_SIZE >>> (myArrayOfTriangles);
			cudaEventRecord(stop,0);
			cudaEventSynchronize(stop);
			cudaEventElapsedTime(&elapsedTimes[0][k],start,stop);

			// PATTERN 2
			cudaEventRecord(start,0);
			setTriangles2 <<< gridSize , BLOCK_SIZE >>> (myTrianglesStructure);
			cudaEventRecord(stop,0);
			cudaEventSynchronize(stop);
			cudaEventElapsedTime(&elapsedTimes[1][k],start,stop);


	} // end of the for loop involving NTIMES

	Test  <<< gridSize, BLOCK_SIZE >>>  (myArrayOfTriangles,myTrianglesStructure,dCorrectBool); // testing the equivalence of both the intializing kernals
	
	// copying back the value of dCorrectBool into hCorrectBool
	cudaMemcpy(&hCorrectBool,dCorrectBool,sizeof(int),cudaMemcpyDeviceToHost);

	if(hCorrectBool != 1) // if the kernels are not equivalent
	{
		printf("the kernel executions are not equivalent\n");
		printf("exiting out of the program\n");
		
	}
 
   else // if kernals are equivalent 
	{
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
   }

	//destroying the events
	cudaEventDestroy(start);
	cudaEventDestroy(stop);
	
	//freeing the allocated memory on the device 
	cudaFree(myArrayOfTriangles);
	cudaFree(myTrianglesStructure.Ax);
	cudaFree(myTrianglesStructure.Ay);
	cudaFree(myTrianglesStructure.Az);
	cudaFree(myTrianglesStructure.Bx);
	cudaFree(myTrianglesStructure.By);
	cudaFree(myTrianglesStructure.Bz);
	cudaFree(myTrianglesStructure.Cx);
	cudaFree(myTrianglesStructure.Cy);
	cudaFree(myTrianglesStructure.Cz);

	return 0;

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// prints the results containig the minimum, maximum, average times taken by the two initializing kernels
// the associated maximum bandwidth of global memory achieved by the kernels
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void printResults()
{

	printf("Demonstrating the advantages of Structure of arrays over Array of structures in data representation\n");
		
   printf("The transfered data size (Bytes): %ld\n",NO_OF_TRIANGLES*sizeof(float)*9);

   printf("\n-------------------------------------------------------------------------------------------------------------------------------\n");
   printf("Pattern \t\t Bandwidth (GB/sec) \t Avg time (ms)  \t Min time (ms)  \t Max time (ms)\n");
   printf("-------------------------------------------------------------------------------------------------------------------------------\n");
	// printing the results for different access patterns
	for(int i=0; i < NO_OF_PATTERNS; i++)
	{
		switch(i)
		{
			case 0: printf("Array of Structures");
					  break;

			case 1: printf("Structure of Arrays");
					  break;

		}

		printf("\t %.6f \t\t %f \t\t  %f \t\t  %f\n",bandWidths[i]/1000000,avgtime[i],mintime[i],maxtime[i]);
  }

   printf("\n ------------------------------------------------------------------------------------------------------------------------------\n");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


