

#include<stdio.h>
#include<math.h>
#include<stdlib.h>

/*****************************************************************************


  Example     : VectVectMult_shared.cu

  Objective   : Write a CUDA Program to perform Vector Vector multiplication
                using local memory implementation.

  Input       : None

  Output      : Execution time in seconds , Gflops achieved

  Created     : Aug 2011

  E-mail      : RarchK

****************************************************************************/


#include<cuda.h>
#include<cuda_runtime.h>

#define EPS 1.0e-12
#define GRIDSIZE 10
#define BLOCKSIZE 16

#define SIZE 128

/* Kernel Function */
__global__ void vectvectshared(double *A,double *B,double *r,int N)
{
	int tx    = threadIdx.x;
	int ty    = threadIdx.y;
	int bx    = blockIdx.x;
	int by    = blockIdx.y;
	int threadsperblock = BLOCKSIZE*BLOCKSIZE;

	__shared__ double temp[BLOCKSIZE*BLOCKSIZE];
	
	int tid   = (ty * blockDim.x) + tx; // relative to block
	int id    = tid + (threadsperblock * gridDim.x * by ) + bx * threadsperblock;
	temp[tid] = 0.00f;
	while(id < N)
	{
		temp[tid] += A[id] * B[id];
		id += gridDim.x * gridDim.y * threadsperblock;
	}
        
	__syncthreads();
	int i = blockDim.x * blockDim.y/2;
	while(i!=0)
	{
		if( tid < i)
			temp[tid] += temp[tid+i];
		__syncthreads();
	i = i/2;
	}
	if(tid == 0)
		r[blockIdx.x] = temp[0];
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


/* prints the result in screen */
void print_on_screen(char * program_name,float tsec,double gflops,int size,int flag)//flag=1 if gflops has been calculated else flag =0
{
        printf("\n---------------%s----------------\n",program_name);
        printf("\tSIZE\t TIME_SEC\t Gflops\n");
        if(flag==1)
        printf("\t%d\t%f\t%lf\t",size,tsec,gflops);
        else
        printf("\t%d\t%lf\t%lf\t",size,"---","---");

}

/* Function to launch kernel for execution */
void launch_kernel(double *A,double *B,double *r,int vlen, dim3 blocks, dim3 threads)
{
	cudaEvent_t start,stop;
	float elapsedTime,Tsec = 0.0,gflops;
	
	CUDA_SAFE_CALL(cudaEventCreate(&start));
	CUDA_SAFE_CALL(cudaEventCreate(&stop));
	
	CUDA_SAFE_CALL(cudaEventRecord (start, 0));
	
	vectvectshared<<<blocks, threads>>>(A, B ,r,vlen);
	
	CUDA_SAFE_CALL(cudaEventRecord (stop, 0));
        CUDA_SAFE_CALL(cudaEventSynchronize (stop));
	
        CUDA_SAFE_CALL(cudaEventElapsedTime ( &elapsedTime, start, stop));
        Tsec = elapsedTime *1.0e-3;
        gflops=(1.0e-9 * (( 1.0 * vlen )/Tsec));

	print_on_screen("Vect Vect Mult - Shared Mem.",Tsec,gflops,vlen,1);
	
}

/* Function to compare cpu and gpu results  */
void compare(double a,double b)
{
	double threshold=0.00000000000001;
	if(a-b>threshold)
		printf("cpu,gpu results do not match!!!\n");
	else
		printf("results matched :) :) \n");
}
/* Function to perform multiplication on CPU */
void  cpu_vectvectMul(double *A,double *B,int length,double &cpu_result)
{
	for(int i=0;i<length-1;i++)
	{
		cpu_result +=(A[i]*B[i]);
	}
	//printf("cpu result=%f\n",sum);
	//printf("gpu result=%f\n",gpu_result);
}

/* Fill in the vector with double precision values */
void fill_dp_vector(double* vec,int size)
{
        int ind;
        for(ind=0;ind<size;ind++)
                vec[ind]=drand48();
}


int main(int argc, char *argv[])
{
	double *hostA, *hostB, *res_partial_host, *res;
	double *devA, *devB, *res_partial_dev;
	double cpu_result;
	int vlen,blockspergrid;
	int i=0;

	vlen=SIZE;
	
	dim3 threadspblock(BLOCKSIZE,BLOCKSIZE);
	
	blockspergrid = vlen / (BLOCKSIZE * BLOCKSIZE);
	if( vlen < BLOCKSIZE*BLOCKSIZE )
		blockspergrid = 1;
	
	hostA = (double *)malloc(vlen * sizeof(double));
	hostB = (double *)malloc(vlen * sizeof(double));
	res_partial_host = (double *)malloc(blockspergrid * sizeof(double));
	res = (double *)malloc(sizeof(double));
	res[0] = 0;

	fill_dp_vector(hostA, vlen);
	fill_dp_vector(hostB, vlen);

	CUDA_SAFE_CALL(cudaMalloc((void **)&devA, vlen * sizeof(double)));
	CUDA_SAFE_CALL(cudaMalloc((void **)&devB, vlen * sizeof(double)));
	CUDA_SAFE_CALL(cudaMalloc((void **)&res_partial_dev, blockspergrid * sizeof(double)));
	CUDA_SAFE_CALL(cudaMemcpy((void*)devA,(void*)hostA,vlen * sizeof(double),cudaMemcpyHostToDevice));
	CUDA_SAFE_CALL(cudaMemcpy((void*)devB,(void*)hostB,vlen * sizeof(double),cudaMemcpyHostToDevice));
		
	launch_kernel(devA, devB ,res_partial_dev,vlen,blockspergrid,threadspblock);
		
	CUDA_SAFE_CALL(cudaMemcpy((void*)res_partial_host,(void*)res_partial_dev,blockspergrid * sizeof(double),cudaMemcpyDeviceToHost));
	for(i=1; i<blockspergrid; i++)
        {
		res_partial_host[0] += res_partial_host[i];
        }

	cpu_vectvectMul(hostA,hostB,vlen,cpu_result);
	
	compare(cpu_result,res_partial_host[0]);
	
	cudaFree(devA);
	cudaFree(devB);
	cudaFree(res_partial_dev);

	free(hostA);
	free(hostB);
	free(res);
	free(res_partial_host);
}
