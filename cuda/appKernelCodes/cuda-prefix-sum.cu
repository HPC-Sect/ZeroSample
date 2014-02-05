/*****************************************************************************


  Example     : cuda-prefix-sum.cu
 
  Objective   : Write a CUDA program to find prefix sum of an given array.
  Input       : None 

  Output      : Execution time in seconds , Gflops achieved
                                                                                                                            
  Created     : Aug 2011    

  E-mail      : RarchK         
                                 
****************************************************************************/



// This is code for Prefix sum....
#include<stdio.h>
#include<cuda.h>
#define BLOCKSIZE 16
#define SIZE 1024
#define EPS 1.0e-15
cudaDeviceProp deviceProp;	


double *host_Vect,*host_ResVect,*cpu_ResVect;
double *device_Vect,*device_ResVect;
int     vlength ;
 int device_Count;
int size = SIZE;

/*mem error*/
void mem_error(char *arrayname, char *benchmark, int len, char *type)
{
        printf("\nMemory not sufficient to allocate for array %s\n\tBenchmark : %s  \n\tMemory requested = %d number of %s elements\n",arrayname, benchmark, len, type);
        exit(-1);
}

/*calculate Gflops*/
double calculate_gflops(float &Tsec)
{
        float gflops=(1.0e-9 * (( 2.0 * size*size )/Tsec));
	return gflops;
}

/*sequential function for Prefix sum*/
void CPU_PrefixSum()
{
	int curElementIndex=0,counter;
	double  temp_result;
	cpu_ResVect = (double *)malloc(vlength*sizeof(double));
	if(cpu_ResVect==NULL)
                mem_error("cpu_ResVect","Prefix sum",size,"double");
	while(curElementIndex < vlength)
        {
                temp_result = 0.00;
                for(counter = 0 ; counter < curElementIndex ; counter++)
                        temp_result = temp_result + host_Vect[counter];

                cpu_ResVect[curElementIndex] = temp_result;
                curElementIndex++ ;            
        }

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
 

/*free memory*/
void dfree(double * arr[],int len)
{
        for(int i=0;i<len;i++)
                CUDA_SAFE_CALL(cudaFree(arr[i]));
        printf("mem freed\n");
}

/* function to calculate relative error*/
void relError(double* dRes,double* hRes,int size)
{
        double relativeError=0.0,errorNorm=0.0;
        int flag=0;
        int i;

        for( i = 0; i < size; ++i) {
                if (fabs(hRes[i]) > fabs(dRes[i]))
                        relativeError = fabs((hRes[i] - dRes[i]) / hRes[i]);
                else
                        relativeError = fabs((dRes[i] - hRes[i]) / dRes[i]);

                if (relativeError > EPS && relativeError != 0.0e+00 )
                {
                        if(errorNorm < relativeError)
                        {
                                errorNorm = relativeError;
                                flag=1;
                        }
                }

        }
        if( flag == 1)
        {
                printf(" \n Results verfication : Failed");
                printf(" \n Considered machine precision : %e", EPS);
                printf(" \n Relative Error                  : %e\n", errorNorm);

        }
        else
                printf("\n Results verfication : Success\n");

}


/*prints the result in screen*/
void print_on_screen(char * program_name,float tsec,double gflops,int size,int flag)//flag=1 if gflops has been calculated else flag =0
{
        printf("\n---------------%s----------------\n",program_name);
        printf("\tSIZE\t TIME_SEC\t Gflops\n");
        if(flag==1)
        printf("\t%d\t%f\t%lf\t",size,tsec,gflops);
        else
        printf("\t%d\t%lf\t%lf\t",size,"---","---");

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


/*Fill in the vector with double precision values */
void fill_dp_vector(double* vec,int size)
{
        int ind;
        for(ind=0;ind<size;ind++)
                vec[ind]=drand48();
}


/////////////////////////////////////////////////////////////////////////////////////////
//
// Prefix sum : this kernel will perform actual Prefix sum
//
/////////////////////////////////////////////////////////////////////////////////////////
__global__ void PrefixSum(double *dInArray, double *dOutArray, int arrayLen, int threadDim)
  {
	int tidx = threadIdx.x;
    	int tidy = threadIdx.y;
    	int tindex = (threadDim * tidx) + tidy;
    	int maxNumThread = threadDim * threadDim; 
    	int pass = 0;  
    	int count ;
    	int curEleInd;
    	double tempResult = 0.0;

    	while( (curEleInd = (tindex + maxNumThread * pass))  < arrayLen )
     	{
        	tempResult = 0.0f;
        	for( count = 0; count < curEleInd; count++)
             		tempResult += dInArray[count];
        	dOutArray[curEleInd] = tempResult;
        	pass++;
      	}
     	__syncthreads();
  }//end of Prefix sum function

/*function to launch kernel*/
void launch_Kernel_PrefixSum()
{
/*          threads_per_block, blocks_per_grid  */
	dim3 dimBlock(BLOCKSIZE,BLOCKSIZE);
	dim3 dimGrid(1,1);
	check_block_grid_dim(deviceProp,dimBlock,dimGrid);
	PrefixSum<<<dimGrid,dimBlock>>>(device_Vect,device_ResVect,vlength,BLOCKSIZE);
}


/*main function*/
int main()
{
       	vlength  = SIZE;
     	
	float elapsedTime,Tsec;
	cudaEvent_t start,stop;

	device_Count=get_DeviceCount();
        printf("\n\nNUmber of Devices : %d\n\n", device_Count);

        // Device Selection, Device 1: Tesla C1060
        cudaSetDevice(0);
      
        int device;
        // Current Device Detection
        cudaGetDevice(&device);
        cudaGetDeviceProperties(&deviceProp,device);
        printf("Using device %d: %s \n", device, deviceProp.name);


	
  	/*allocating the memory for each vector */
	host_Vect = new double[vlength];
	host_ResVect = new double[vlength];

	
	// ---------------checking host memory  for error..............................
         if(host_Vect==NULL)
                mem_error("host_Vect","Prefix sum",vlength,"double");

         if(host_ResVect==NULL)
                mem_error("host_ResVect","Prefix sum",vlength,"double");

	//--------------Initializing the input arrays..............
	fill_dp_vector(host_Vect,vlength);

 	/* allocate memory for GPU events 
        start = (cudaEvent_t) malloc (sizeof(cudaEvent_t));
        stop = (cudaEvent_t) malloc (sizeof(cudaEvent_t));
        if(start==NULL)
                mem_error("start","Prefix sum",1,"cudaEvent_t");
        if(stop==NULL)
                mem_error("stop","Prefix sum",1,"cudaEvent_t");*/
  	
	//event creation...
        CUDA_SAFE_CALL(cudaEventCreate (&start));
        CUDA_SAFE_CALL(cudaEventCreate (&stop));

  	//allocating memory on GPU
	CUDA_SAFE_CALL(cudaMalloc( (void**)&device_Vect, vlength* sizeof(double)));
	CUDA_SAFE_CALL(cudaMalloc( (void**)&device_ResVect, vlength* sizeof(double)));

 	//moving data from CPU to GPU
	CUDA_SAFE_CALL(cudaMemcpy((void*)device_Vect, (void*)host_Vect,vlength*sizeof(double),cudaMemcpyHostToDevice));

	// Launching kernell..........	
	CUDA_SAFE_CALL(cudaEventRecord (start, 0));
	
	launch_Kernel_PrefixSum();
	
	CUDA_SAFE_CALL(cudaEventRecord (stop, 0));
	CUDA_SAFE_CALL(cudaEventSynchronize (stop));
	CUDA_SAFE_CALL(cudaEventElapsedTime ( &elapsedTime, start, stop));

	Tsec= 1.0e-3*elapsedTime;
 	
	// calling funtion for measuring Gflops
        calculate_gflops(Tsec);
	
	//printing the result on screen
    	print_on_screen("Prefix Sum",Tsec,calculate_gflops(Tsec),size,1); 

 
	//retriving result from device
  	CUDA_SAFE_CALL(cudaMemcpy((void*)host_ResVect, (void*)device_ResVect,vlength*sizeof(double),cudaMemcpyDeviceToHost));

	// CPU calculation..and checking error deviation....
	CPU_PrefixSum();
  	relError(cpu_ResVect,host_ResVect,size);
   	printf("\n ----------------------------------------------------------------------\n");

	/*free the memory from GPU */
	double *array[2];
        array[0]=device_Vect;
        array[1]=device_ResVect;
        dfree(array,2);

	//free host memory----------
        free(host_Vect);
        free(host_ResVect);
        free(cpu_ResVect);

	return 0;
}// end of main
