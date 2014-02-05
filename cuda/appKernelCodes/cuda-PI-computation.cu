/*****************************************************************************


  Example     : cuda-PI-computation.cu
 
  Objective   : Write a CUDA Program to calculate value of PI using numerical integration method..

  Input       : None 

  Output      : Execution time in seconds
                                                                                                                            
  Created     : Aug 2011    

  E-mail      : RarchK         
                                 
****************************************************************************/


// This is code for Pi calculation....
#include<stdio.h>
#include<cuda.h>
#define BLOCKSIZE 16
#define SIZE 1024
#define EPS 1.0e-15
cudaDeviceProp deviceProp;	


double *host_Vect,*host_Result,cpu_Result;
double *device_Vect,*device_Result,totalsum;
int     vlength ;
 int device_Count;
int noOfIntervals = SIZE;
/*mem error*/
void mem_error(char *arrayname, char *benchmark, int len, char *type)
{
        printf("\nMemory not sufficient to allocate for array %s\n\tBenchmark : %s  \n\tMemory requested = %d number of %s elements\n",arrayname, benchmark, len, type);
        exit(-1);
}


/*sequential function for Pi calculation*/
void CPU_PiCalculation()
{
	double x;
	 double h = 1.0 / noOfIntervals;
	int i;
	for (i = 1; i < noOfIntervals +1; i = i+1) 
        {
                x = h * (i + 0.5);
                cpu_Result = cpu_Result + 4.0/(1.0 + x * x);
        } 
	cpu_Result = cpu_Result * h;
}

/*Check for safe return of all calls to the device */
void CUDA_SAFE_CALL(cudaError_t call)
{
        cudaError_t ret = call;
        //printf("RETURN FROM THE CUDA CALL:%d\t:",ret);                                        
        switch(ret)
        {
                case cudaSuccess:
                //              printf("Success\n");                    
                                break;
        /*      case cudaErrorInvalidValue:                             
                                {
                                printf("ERROR: InvalidValue:%i.\n",__LINE__);
                                exit(-1);
                                break;  
                                }                       
                case cudaErrorInvalidDevicePointer:                     
                                {
                                printf("ERROR:Invalid Device pointeri:%i.\n",__LINE__);
                                exit(-1);
                                break;
                                }                       
                case cudaErrorInvalidMemcpyDirection:                   
                                {
                                printf("ERROR:Invalid memcpy direction:%i.\n",__LINE__);        
                                exit(-1);
                                break;
                                }                       */
                default:
                        {
                                printf(" ERROR at line :%i.%d' ' %s\n",__LINE__,ret,cudaGetErrorString(ret));
                                exit(-1);
                                break;
                        }
        }
}
 


/* function to calculate relative error*/
void relError(int noOfIntervals,double *host_Result)
{
	int flag=0;
	double  errorNorm = 0.0;
        double  eps=EPS;
        double  relativeError=0.0;

	relativeError = ((cpu_Result - ((*host_Result))) / cpu_Result);
        if (relativeError > eps && relativeError != 0.0e+00 )
        {
               if(errorNorm < relativeError)
                {
                        errorNorm = relativeError;
                        flag=1;
                }
         }
        if( flag == 1) {

                printf(" \nresult failed");

        }
        if(flag==0)
        {
                printf("\n\n\t\t Result Verification: success\n\n");
        }

}


/*prints the result in screen*/
void print_on_screen(char * program_name,float tsec,double gflops,int noOfIntervals,int flag)//flag=1 if gflops has been calculated else flag =0
{
        printf("\n---------------%s----------------\n",program_name);
        printf("\tNoOfIntervals\t TIME_SEC\t Gflops\n");
        if(flag==1)
        printf("\t%d\t\t%f\t%lf\t",noOfIntervals,tsec,gflops);
        else
        printf("\t%d\t\t %lf\t  %s\t",noOfIntervals,tsec,"---");

}

/*funtion to check blocks per grid and threads per block*/
void check_block_grid_dim(cudaDeviceProp devProp,dim3 blockDim,dim3 gridDim)
{

        if( blockDim.x >= devProp.maxThreadsDim[0] || blockDim.y >= devProp.maxThreadsDim[1] || blockDim.z >= devProp.maxThreadsDim[2] )
        {
                printf("\nBlock Dimensions exceed the maximum limits:%d * %d * %d \n",devProp.maxThreadsDim[0],devProp.maxThreadsDim[1],devProp.maxThreadsDim[2]);
               exit(-1);
        }

        if( gridDim.x >= devProp.maxGridSize[0] || gridDim.y >= devProp.maxGridSize[1] || gridDim.z >= devProp.maxGridSize[2] )
        {
                printf("\nGrid Dimensions exceed the maximum limits:%d * %d * %d \n",devProp.maxGridSize[0],devProp.maxGridSize[1],devProp.maxGridSize[2]);
               exit(-1);
        }
}


/*Get the number of GPU devices present on the host */
int get_DeviceCount()
{
        int count;
        cudaGetDeviceCount(&count);
        return count;
}




/////////////////////////////////////////////////////////////////////////////////////////
//
// Pi calculation : this kernel will perform Pi calculation 
//
/////////////////////////////////////////////////////////////////////////////////////////
__global__ void PiCalculation(double *dIntervalLocalValue,double numInterval, int threadDim)
  {
	int tidx = threadIdx.x;
    	int tidy = threadIdx.y;
    	int tindex = (threadDim * tidx) + tidy;
    	int maxNumThread = threadDim * threadDim; 
    	int intervalCount ;
    	double tempResult = 0.0;
    
    	double h = 1.0 / numInterval;
    	double sum  = 0.0; 
    	double x = 0.0;
    	dIntervalLocalValue[tindex] = 0.0;
 
    	for( intervalCount = tindex + 1; intervalCount <= numInterval; intervalCount += maxNumThread )
      	{
          	x = h * ( (double)intervalCount - 0.5 ); 
          	sum += (4.0 /(1.0 + x*x)); 
      	}
    	dIntervalLocalValue[tindex] = h * sum;
     	__syncthreads();

     	if( tindex == 0 )
      	{
        	tempResult = 0.0f;
        	for( intervalCount = 0; intervalCount < maxNumThread; intervalCount++)
             		tempResult += dIntervalLocalValue[intervalCount];
        	dIntervalLocalValue[0] = tempResult;
      	}
  }//end of Pi calculation function

/*function to launch kernel*/
void launch_Kernel_PiCalculation()
{
/*          threads_per_block, blocks_per_grid  */
	dim3 dimBlock(BLOCKSIZE,BLOCKSIZE);
	dim3 dimGrid(1,1);
	check_block_grid_dim(deviceProp,dimBlock,dimGrid);
	PiCalculation<<<dimGrid,dimBlock>>>(device_Result,noOfIntervals,BLOCKSIZE);
}


/*main function*/
int main()
{
	double gflops=0.0;
       	vlength  = SIZE;
     	
	float elapsedTime,Tsec;
	cudaEvent_t start,stop;
	
	host_Result = (double*) malloc( sizeof(double));	
	device_Count=get_DeviceCount();
        printf("\n\nNUmber of Devices : %d\n\n", device_Count);

        // Device Selection, Device 1: Tesla C1060
        cudaSetDevice(0);
      
        int device;
        // Current Device Detection
        cudaGetDevice(&device);
        cudaGetDeviceProperties(&deviceProp,device);
        printf("Using device %d: %s \n", device, deviceProp.name);

 	/* allocate memory for GPU events 
        start = (cudaEvent_t) malloc (noOfIntervalsof(cudaEvent_t));
        stop = (cudaEvent_t) malloc (noOfIntervalsof(cudaEvent_t));
        if(start==NULL)
                mem_error("start","Pi calculation",1,"cudaEvent_t");
        if(stop==NULL)
                mem_error("stop","Pi calculation",1,"cudaEvent_t");*/
  	
	//event creation...
        CUDA_SAFE_CALL(cudaEventCreate (&start));
        CUDA_SAFE_CALL(cudaEventCreate (&stop));

  	//allocating memory on GPU
	CUDA_SAFE_CALL(cudaMalloc( (void**)&device_Result, sizeof(double)));


	// Launching kernell..........	
	CUDA_SAFE_CALL(cudaEventRecord (start, 0));
	
	launch_Kernel_PiCalculation();
	
	CUDA_SAFE_CALL(cudaEventRecord (stop, 0));
	CUDA_SAFE_CALL(cudaEventSynchronize (stop));
	CUDA_SAFE_CALL(cudaEventElapsedTime ( &elapsedTime, start, stop));

	Tsec= 1.0e-3*elapsedTime;
 	
	
	//printing the result on screen
    	print_on_screen("PiCalculation",Tsec,gflops,noOfIntervals,0); 

 
	//retriving result from device
  	CUDA_SAFE_CALL(cudaMemcpy((void *)host_Result, (void *)device_Result,sizeof(double),cudaMemcpyDeviceToHost));

	// CPU calculation..and checking error deviation....
	CPU_PiCalculation();
  	relError(noOfIntervals,host_Result);
   	printf("\n ----------------------------------------------------------------------\n");

	//free host memory----------
        free(host_Result);

	return 0;
}// end of main
