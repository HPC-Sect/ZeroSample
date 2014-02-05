
/*****************************************************************************


  Example     : cuda-vector-vector-multiplication-mGPU.cu

  Objective   : Write a CUDA Program to perform Vector Vector multiplication
                using global memory implementation to be executed on multiple GPUs.

  Input       : None

  Output      : Execution time in seconds , Gflops achieved

  Created     : Aug 2011

  E-mail      : RarchK

****************************************************************************/



#include<stdio.h>
#include<cuda.h>
#include<pthread.h>
#include<error.h>

#define EPS 1.0e-12
#define GRIDSIZE 10
#define BLOCKSIZE 16

#define SIZE 128

int blocksPerGrid;
int gridsPerBlock;
struct Data
{
	int deviceId;
	int size;
	double* a;
	double* b;
	double retVal;	
	double Tsec;
};

__global__ void vvmul(int len,double* A,double* B,double *C)
{
	int tid= blockIdx.x*blockDim.x*blockDim.y + threadIdx.x +threadIdx.y * blockDim.x;
	
	while(tid < len)
	{
		C[tid] = A[tid] * B[tid];
		tid += blockDim.x * gridDim.x;
	}	
}

/* Check for safe return of all calls to the devic */
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

/* Function for vector multiplication on host*/
void host_vvmul(double* A,double* B,int len,double &C)
{
        int i;
        for(i = 0;i <len;i++)
                C += A[i]*B[i];
}

/* Function to calulate Gflops */
double calculate_gflops(double &Tsec)
{
        //printf("time taken is %.8lf\n",Tsec);
        double gflops=(1.0e-9 * (( 2.0 * SIZE  )/Tsec));
        //printf("Gflops is \t%f\n",gflops);
        return gflops;

}

/* Function to display output */
void display(double* arr,int size)
{
	int i;
	for(i=0;i<size;i++)
		printf("%f ",arr[i]);
	printf("\t%d\n",i);
}	

/*Function doing device related computations */
void* routine(void* givendata)
{
	Data *data = (Data*)givendata;
	int len = data->size;
	double *a,*b,*part_c;
	double *d_a,*d_b,*d_part_c;
	double c;
	cudaEvent_t start,stop;
	float elapsedTime;

	a=data->a;
	b=data->b;
	part_c = (double*)malloc(len*sizeof(double));

		
	CUDA_SAFE_CALL(cudaSetDevice(data->deviceId));
	
	CUDA_SAFE_CALL(cudaEventCreate(&start));
	CUDA_SAFE_CALL(cudaEventCreate(&stop));
	
	CUDA_SAFE_CALL(cudaMalloc((void**)&d_a,len*sizeof(double)));
	CUDA_SAFE_CALL(cudaMalloc((void**)&d_b,len*sizeof(double)));
	CUDA_SAFE_CALL(cudaMalloc((void**)&d_part_c,len*sizeof(double)));
	

	CUDA_SAFE_CALL(cudaMemcpy(d_a,a,len*sizeof(double),cudaMemcpyHostToDevice));	
	CUDA_SAFE_CALL(cudaMemcpy(d_b,b,len*sizeof(double),cudaMemcpyHostToDevice));	


	dim3 threadsPerBlock(16,16);
	
	int numBlocks;
	if( len /256 == 0)
		numBlocks=1;
	else
		numBlocks = len/100;
	dim3 blocksPerGrid(numBlocks ,1);
	
	CUDA_SAFE_CALL(cudaEventRecord(start,0));
	
	vvmul<<<blocksPerGrid,threadsPerBlock>>>(len,d_a,d_b,d_part_c);

	if(cudaPeekAtLastError())
		printf("KERNEL ERROR: %s\t on device:%d\n",cudaGetErrorString(cudaPeekAtLastError()),data->deviceId);

	CUDA_SAFE_CALL(cudaEventRecord(stop,0));
	CUDA_SAFE_CALL(cudaEventSynchronize(stop));

	CUDA_SAFE_CALL(cudaMemcpy(part_c,d_part_c,len*sizeof(double),cudaMemcpyDeviceToHost));	
		
	
	int ind;
	for(ind=0;ind<len;ind++)
		c += part_c[ind];

	CUDA_SAFE_CALL(cudaEventElapsedTime(&elapsedTime,start,stop));
	data->Tsec=elapsedTime*(1.0e-3);	


	CUDA_SAFE_CALL(cudaFree(d_a));
	CUDA_SAFE_CALL(cudaFree(d_b));
	CUDA_SAFE_CALL(cudaFree(d_part_c));

	free(part_c);
	data->retVal=c;
	return 0;
}

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


int main(int argc,char** argv)
{
	
	int devCount;
	CUDA_SAFE_CALL(cudaGetDeviceCount(&devCount));
	if(devCount < 2)
	{
		printf("Atleast 2 GPU's are needed :%d\n",devCount);
		exit(0);
	}

	double *hVectA,*hVectB,hRes,host_hRes;
	int vlen=SIZE;
	int ind;

	hVectA=(double*)malloc(vlen*sizeof(double));
	hVectB=(double*)malloc(vlen*sizeof(double));

	for(ind=0;ind < vlen;ind++)
	{
		hVectA[ind]=2.00;
		hVectB[ind]=2.00;
	}

	Data vector[2];
	
	vector[0].deviceId 	= 0;
	vector[0].size		=vlen/2;
	vector[0].a		=hVectA;
	vector[0].b		=hVectB;	   
	
	vector[1].deviceId 	= 1;
	vector[1].size		=vlen/2;
	vector[1].a		=hVectA + vlen/2 ;
	vector[1].b		=hVectB + vlen/2 ;	   

	
	pthread_t thread;
	if(pthread_create(&thread,NULL,routine,(void*)&vector[0]) != 0)
		perror("Thread creation error\n");
	routine(&vector[1]);
	pthread_join(thread,NULL);

	hRes=vector[0].retVal + vector[1].retVal;

	/* ---------Check result with host CPU result ---------*/	
	
        host_vvmul(hVectA,hVectB,vlen,host_hRes);

	relError(&hRes,&host_hRes,1);                

	print_on_screen("MatMatMult_mGPU",vector[0].Tsec,calculate_gflops(vector[0].Tsec),vlen,1);
	print_on_screen("MatMatMult_mGPU",vector[1].Tsec,calculate_gflops(vector[1].Tsec),vlen,1);

	
	free(hVectA);
	free(hVectB);
	
	return 0;
}
