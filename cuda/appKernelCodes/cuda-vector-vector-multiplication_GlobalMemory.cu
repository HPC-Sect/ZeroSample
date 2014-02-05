
/*****************************************************************************


  Example     : VectVectMult.cu
 
  Objective   : Write a CUDA Program to perform Vector Vector multiplication
                using global memory implementation.

  Input       : None 

  Output      : Execution time in seconds , Gflops achieved
                                                                                                                            
  Created     : Aug 2011    

  E-mail      : RarchK         
                                 
****************************************************************************/


#include<stdio.h>
#include<cuda.h>

#define EPS 1.0e-12
#define GRIDSIZE 10
#define BLOCKSIZE 16

#define SIZE 128

double            *dMatA, *dMatB;
double             *hMatA, *hMatB;
double             *dresult, *hresult;
int               vlength , count = 0;
int               blockWidth;

cudaEvent_t start,stop;
cudaDeviceProp deviceProp;
int device_Count;
float elapsedTime;
double Tsec,gflops;
long long int start_time,end_time; 


/* Kernel Function */
__global__ void vvmul(int len,double* A,double* B,double *C)
{
        int tid= blockIdx.x*blockDim.x*blockDim.y + threadIdx.x +threadIdx.y * blockDim.x;

        while(tid < len)
        {
                C[tid] = A[tid] * B[tid];
                tid += blockDim.x * gridDim.x;
        }
}

/* Check for safe return of all calls to the device */
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



/* Get the number of GPU devices present on the host */
int get_DeviceCount()
{
	int count;
	cudaGetDeviceCount(&count);	
	return count;	
}

/* Function to launch kernel for execution */
void launch_kernel()
{
	dim3 threadsPerBlock(16,16);
	int numBlocks;

        if( vlength /256 == 0)
                numBlocks=1;
        else
                numBlocks = vlength/100;

        dim3 blocksPerGrid(numBlocks ,1);
	
	cudaEventRecord(start,0);
 	vvmul<<<blocksPerGrid,threadsPerBlock>>>(vlength,dMatA,dMatB,dresult);
	cudaEventRecord(stop,0);

        cudaEventSynchronize(stop);
        cudaEventElapsedTime(&elapsedTime,start,stop);
        Tsec=elapsedTime*1.0e-3;
        printf("time taken is %.8lf\n",Tsec);
        gflops=(2.0e-9 * ((  vlength )/Tsec));
        printf("Gflops is \t%f\n",gflops);

}

/* Function to get device informatin */
void deviceQuery()
{
	 int device_Count;
        device_Count=get_DeviceCount();
        printf("\n\nNUmber of Devices : %d\n\n", device_Count);

        cudaSetDevice(0);
        int device;
        cudaGetDevice(&device);
        cudaGetDeviceProperties(&deviceProp,device);
        printf("Using device %d: %s \n", device, deviceProp.name);
}

/* function for memory check */
void mem_error(char *arrayname, char *benchmark, int len, char *type)
{

        printf("\nMemory not sufficient to allocate for array %s\n\tBenchmark : %s  \n\tMemory requested = %d number of %s elements\n",arrayname, benchmark, len, type);
        printf("\n\tAborting\n\n");
        exit(-1);
}


/* Fill in the vector with double precision values */
void fill_dp_vector(double* vec,int size)
{
        int ind;
        for(ind=0;ind<size;ind++)
                vec[ind]=drand48();
}

/* Function to compare CPU-GPU results */
double cpu_vectvectMul(double *A,double *B,int length,double *gpu_result)
{
	double	sum=0;
	for(int i=0;i<length-1;i++)
	{
		sum+=(A[i]*B[i]);
	}
	//double threshold=1;
	printf("cpu result=%f\n",sum);
	printf("gpu result=%f\n",gpu_result);
	return sum;
}

/* print the result on screen */
void print_on_screen(char * program_name,float tsec,double gflops,int size,int flag)//flag=1 if gflops has been calculated else flag =0
{
        printf("\n---------------%s----------------\n",program_name);
        printf("\tSIZE\t TIME_SEC\t Gflops\n");
        if(flag==1)
        printf("\t%d\t%f\t%lf\t",size,tsec,gflops);
        else
        printf("\t%d\t%lf\t%lf\t",size,"---","---");

}

/* free device objects */
void dfree(double * arr[],int len)
{
        for(int i=0;i<len;i++)
                CUDA_SAFE_CALL(cudaFree(arr[i]));
        printf("mem freed\n");
}


/* main */

int main()
{
	start=(cudaEvent_t)malloc(sizeof(cudaEvent_t));
	stop=(cudaEvent_t)malloc(sizeof(cudaEvent_t));

	CUDA_SAFE_CALL(cudaEventCreate(&start));
	CUDA_SAFE_CALL(cudaEventCreate(&stop));

	double *array[3];
	array[0]=dMatA;
	array[1]=dMatB;
	array[2]=dresult;
	
	deviceQuery();

	vlength=SIZE;

	/* allocate memory for GPU events 
	start = (cudaEvent_t) malloc (sizeof(cudaEvent_t));	
	stop = (cudaEvent_t) malloc (sizeof(cudaEvent_t));

	if(start==NULL)
		mem_error("start","vectvectmul",1,"cudaEvent_t");
	if(stop==NULL)
		mem_error("stop","vectvectmul",1,"cudaEvent_t");*/

	/* event creation */
	CUDA_SAFE_CALL(cudaEventCreate (&start));
        CUDA_SAFE_CALL(cudaEventCreate (&stop));
   

   
   	/* allocation host memory */
   	hMatA = (double*) malloc( vlength *  sizeof(double));
   	hMatB = (double*) malloc( vlength * sizeof(double));
   	hresult = (double*) malloc( sizeof(double));

	 if(hMatA==NULL)
                mem_error("hMatA","vectvectmul",vlength,"double");

	 if(hMatB==NULL)
                mem_error("hMatB","vectvectmul",vlength,"double");

	 if(hresult==NULL)
                mem_error("hresult","vectvectmul",1,"double");

	 /* allocation device memory */
   	CUDA_SAFE_CALL(cudaMalloc( (void**)&dMatA, vlength * sizeof(double)));
   	CUDA_SAFE_CALL(cudaMalloc( (void**)&dMatB, vlength * sizeof(double)));
   	CUDA_SAFE_CALL(cudaMalloc( (void**)&dresult, sizeof(double)));

  	fill_dp_vector(hMatA,vlength);
  	fill_dp_vector(hMatB,vlength); 

    	CUDA_SAFE_CALL(cudaMemcpy((void*)dMatA, (void*)hMatA, vlength* sizeof(double) , cudaMemcpyHostToDevice ));
    	CUDA_SAFE_CALL(cudaMemcpy((void*)dMatB, (void*)hMatB, vlength* sizeof(double) , cudaMemcpyHostToDevice ));
 
 	hresult[0] = 0.00f;
   	CUDA_SAFE_CALL(cudaMemcpy((void*)dresult, (void*)hresult, sizeof(double) , cudaMemcpyHostToDevice ));

   	/* calling device kernel */
	launch_kernel(); 
  	CUDA_SAFE_CALL(cudaMemcpy((void*)hresult, (void*)dresult, sizeof(double) , cudaMemcpyDeviceToHost ));
   	printf("\n ----------------------------------------------------------------------");
   	printf( "\n Result : %f \n", hresult[0]);

 	/* comparing results at cpu and gpu */
	double cpu_result=cpu_vectvectMul(hMatA,hMatB,vlength,hresult);
	//printf("%f",cpu_result);   

	/* printing result on screen */
 	print_on_screen("VECTOR VECTOR MULTIPLICATION",Tsec,gflops,vlength,1);

	dfree(array,3);
   	free(hMatA);
   	free(hMatB);
   	free(hresult);
}
