
/*****************************************************************************


  Example     :  cuda-matrix-matrix-multiplication.cu
 
  Objective   : Write a CUDA Program to perform Matrix Matrix multiplication.

  Input       : None 

  Output      : Execution time in seconds , Gflops achieved
                                                                                                                            
  Created     : Aug 2011    

  E-mail      : RarchK         
                                 
****************************************************************************/

#include<stdio.h>
#include<cuda.h>
#define BLOCKSIZE 16
#define SIZE 128

cudaDeviceProp deviceProp;	
cudaEvent_t start,stop;
cudaError_t ret;
double *host_MatA,*host_MatB,*host_MatC,*CPU_Result;
double *device_MatA,*device_MatB,*device_MatC;
int size = SIZE;
float elapsedTime;
double Tsec,gflops;

/*kernel funtion*/
__global__ void Muld(double* A, double* B, int wA, int wB, double* C)
  {
   int bx = blockIdx.x;
   int by = blockIdx.y;
   int tx = threadIdx.x;
   int ty = threadIdx.y;
   int aBegin  = wA * BLOCKSIZE * by;
   int aEnd = aBegin + wA - 1;
   int aStep  = BLOCKSIZE;
   int bBegin  =  BLOCKSIZE * bx;
   int bStep = BLOCKSIZE * wB;
   double Csub = 0;
   for(int a = aBegin, b = bBegin; a <= aEnd ; a  += aStep, b += bStep)
     {
       __shared__ double As[BLOCKSIZE][BLOCKSIZE];
       __shared__ double Bs[BLOCKSIZE][BLOCKSIZE];
       As[ty][tx] = A[a + wA * ty + tx];
       Bs[ty][tx] = B[b+ wB * ty + tx];
       __syncthreads();
       for(int k= 0; k< BLOCKSIZE; ++k)
         Csub += As[ty][k] * Bs[k][tx];
       __syncthreads();
      }

     int c = wB * BLOCKSIZE * by + BLOCKSIZE * bx;
     C[ c+ wB * ty + tx] = Csub;

  }/* end of Muld device code */


/*mem error*/
void mem_error(char *arrayname, char *benchmark, int len, char *type)
{
        printf("\nMemory not sufficient to allocate for array %s\n\tBenchmark : %s  \n\tMemory requested = %d number of %s elements\n",arrayname, benchmark, len, type);
        exit(-1);
}


/*cuda safe call*/
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



/* void SetUp_CUDA_Exe_Config() */
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


/*function to free memory*/
void dfree(double * arr[],int len)
{
        for(int i=0;i<len;i++)
                CUDA_SAFE_CALL(cudaFree(arr[i]));
        printf("mem freed\n");
}


/*calculate Gflops*/
double calculate_gflops(double &Tsec)
{
	//printf("time taken is %.8lf\n",Tsec);
	double gflops=(1.0e-9 * (( 1.0 * size*size*size )/Tsec));
	//printf("Gflops is \t%f\n",gflops);
	return gflops;

}

/*get device count*/
int get_DeviceCount()
{
        int count;
        cudaGetDeviceCount(&count);
        return count;
}



/*launch kernel function is called in main()*/
void launch_kernel_MatMatMult()
{
/* threads_per_block= BLOCKSIZE, blocks_per_grid=size/dimBlock  */

	printf("in launch kernel\n");
	dim3 dimBlock(BLOCKSIZE,BLOCKSIZE);
	dim3 dimGrid(size/dimBlock.x,size/dimBlock.y);	
 
//checking the maximum limit of blocksize and gridsize-------------------	
	check_block_grid_dim(deviceProp,dimBlock,dimGrid);
 	printf("after check\n"); 
	cudaEventRecord(start,0);	
	Muld<<<dimGrid,dimBlock>>>(device_MatA,device_MatB,size,size,device_MatC);
	cudaEventRecord(stop,0);
	cudaEventSynchronize(stop);
	cudaEventElapsedTime(&elapsedTime,start,stop);
	Tsec=elapsedTime*1.0e-3;
	calculate_gflops(Tsec);

}

/* Fill in the vector with double precision values */
void fill_dp_vector(double* vec,int size)
{
        int ind;
        for(ind=0;ind<size;ind++)
                vec[ind]=drand48();
}

/*function to print on the screen*/
void print_on_screen(char * program_name,float tsec,double gflops,int size,int flag)//flag=1 if gflops has been calculated else flag =0
{
        printf("\n---------------%s----------------\n",program_name);
        printf("\tSIZE\t TIME_SEC\t Gflops\n");
        if(flag==1)
        printf("\t%d\t%f\t%lf\t",size,tsec,gflops);
        else
        printf("\t%d\t%lf\t%lf\t",size,"---","---");

}




/*-----main()-----*/

int main()
 {
	int device_Count=get_DeviceCount();
        printf("\n\nNUmber of Devices : %d\n\n", device_Count);

        // Device Selection, Device 1: Tesla C1060
        cudaSetDevice(1);
	
	int device;
        // Current Device Detection
        cudaGetDevice(&device);         
        cudaGetDeviceProperties(&deviceProp,device);
	printf("Using device %d: %s \n", device, deviceProp.name);
	/* allocate memory for GPU events 
	start = (cudaEvent_t) malloc (sizeof(cudaEvent_t));	
	stop = (cudaEvent_t) malloc (sizeof(cudaEvent_t));
	if(start==NULL)
		mem_error("start","MatMatMult",1,"cudaEvent_t");
	if(stop==NULL)
		mem_error("stop","MatMatMult",1,"cudaEvent_t");*/
	//event creation...
	CUDA_SAFE_CALL(cudaEventCreate (&start));
        CUDA_SAFE_CALL(cudaEventCreate (&stop));
   

   
    	/*allocating the memory for each matrix */
	host_MatA = new double[size*size];
	host_MatB = new double[size*size];
	host_MatC = new double[size*size];
	CPU_Result= new double[size*size];
	 if(host_MatA==NULL)
                mem_error("host_MatA","MatMatMult",size,"double");

	if(host_MatB==NULL)
                mem_error("host_MatB","MatMatMult",size,"double");
	if(host_MatC==NULL)
                mem_error("host_MatC","MatMatMult",size,"double");
	if(CPU_Result==NULL)
                mem_error("CPU_Result","MatMatMult",size,"double");


	//--------filling the matrix with double precision-----------
  	fill_dp_vector(host_MatA,size*size);
  	fill_dp_vector(host_MatB,size*size); 
  
 	//allocating memory on GPU
	CUDA_SAFE_CALL(cudaMalloc( (void**)&device_MatA,size*size*sizeof(double)));
	CUDA_SAFE_CALL(cudaMalloc( (void**)&device_MatB, size*size*sizeof(double)));
	CUDA_SAFE_CALL(cudaMalloc( (void**)&device_MatC,size*size*sizeof(double)));

   	// copying host matrix to device matrix
    	CUDA_SAFE_CALL(cudaMemcpy((void*)device_MatA, (void*)host_MatA, size*size* sizeof(double) , cudaMemcpyHostToDevice ));
    	CUDA_SAFE_CALL(cudaMemcpy((void*)device_MatB, (void*)host_MatB, size*size*sizeof(double) , cudaMemcpyHostToDevice ));
 
	launch_kernel_MatMatMult(); //launching the kernel
	
   
   	//retriving result from device
   	CUDA_SAFE_CALL(cudaMemcpy((void*)host_MatC, (void*)device_MatC, size*size*sizeof(double) , cudaMemcpyDeviceToHost ));

	//comparing result of CPU-GPU
	//relError(CPU_Result,host_MatC,size*size);

	//printing the result on screen 
     	print_on_screen("MAT MAT Mult",Tsec,calculate_gflops(Tsec),size,1);


   
	//free the device memory----------
	double *array[3];
	array[0]=device_MatA;
	array[1]=device_MatB;
	array[2]=device_MatC;
	
	dfree(array,3);
	//free host memory----------

	free(host_MatA);
	free(host_MatB);
	free(host_MatC);
	free(CPU_Result);

}// end of main
