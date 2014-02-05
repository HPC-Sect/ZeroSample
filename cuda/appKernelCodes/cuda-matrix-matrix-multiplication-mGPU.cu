
/*****************************************************************************


  Example     :cuda-matrix-matrix-multiplication-mgpu.cu
 
  Objective   : Write CUDA program to compute Matrix-Matrix multiplication 
                to be executed on multiple GPUs.(using global memory)

  Input       : None 

  Output      : Execution time in seconds , Gflops achieved
                                                                                                                            
  Created     : Aug 2011    

  E-mail      : RarchK         
                                 
****************************************************************************/


#include<stdio.h>
#include<cuda.h>
#include<math.h>

#define SIZE 128

#define EPS 1.0e-12
#define GRIDSIZE 10
#define BLOCKSIZE 16

#define MAX_GPU 2


typedef struct
{
	int hA;
	int wA;
	int wB;
	double* hMatA;
	double* hMatB;
	double* hMatC;
	double* dMatA;
	double* dMatB;
	double* dMatC;
	cudaStream_t stream;
}TGPUPlan;

int hA, wA,wB;
double *hMatA,*hMatB,*hMatC,*dMatA,*dMatB,*dMatC;

void checkResult(double *InMatA, double *InMatB, double *outMatC, int m, int n , int k );

__global__ void mmmul(double* dm1,double* dm2,double *dres,int r,int m,int c)
{
        int tx = blockIdx.x*blockDim.x + threadIdx.x;
        int ty = blockIdx.y*blockDim.y + threadIdx.y;

        if(tx<c&&ty<r)
        {
        int i;
	dres[ty*c+tx]=0.00;
        for(i=0;i<m;i++)
        dres[ty*c+tx]+=dm1[ty*m+i]*dm2[i*c+tx];

        }

}

void print_on_screen(char * program_name,float tsec,double gflops,int size,int flag)//flag=1 if gflops has been calculated else flag =0
{
        printf("\n---------------%s----------------\n",program_name);
        printf("\tSIZE\t TIME_SEC\t Gflops\n");
        if(flag==1)
        printf("\t%d\t%f\t%lf\t",size,tsec,gflops);
        else
        printf("\t%d\t%lf\t%lf\t",size,"---","---");

}

/*
 * Check for safe return of all calls to the device
 */

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


/* Function to check cpu and gpu results */
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
            
int main(int argc,char** argv)
{
	int numGPU;
	int hA,wA,wB;
	double *host_A,*host_B,*host_C;	
	int gpuBase,offset;	
	int i,j;
	
	/* FOR TIMING MEASUREMENTS */
	/*cudaEvent_t* start,*stop;
	float *elapsedTime;
	float Tsec=0,gflops;*/
	
	
	/* ----- MULTI DEVICE COUNT --------*/
	CUDA_SAFE_CALL(cudaGetDeviceCount(&numGPU));
	if(numGPU > MAX_GPU )
		numGPU=MAX_GPU;
	printf("CUDA CAPABLE DEVICE COUNT: %d\n",numGPU);	

	hA=SIZE;
	wA=SIZE;
	wB=SIZE;

	
	/*---------FILLING HOST MATRICES---------*/
	
	host_A=(double*)malloc(hA*wA*sizeof(double));
	host_B=(double*)malloc(wA*wB*sizeof(double));
	host_C=(double*)malloc(hA*wB*sizeof(double));
	
	for(i =0;i < hA * wA;i++)
		host_A[i] = drand48();
			
	for(i =0;i < wA*wB;i++)
		host_B[i] = drand48();

	
	/*start = (cudaEvent_t*)malloc(numGPU*sizeof(cudaEvent_t));	
	stop = (cudaEvent_t*)malloc(numGPU*sizeof(cudaEvent_t));	
	elapsedTime = (float *)malloc(numGPU*sizeof(float));	*/
	

	/*-------INITIATING THE DATA FOR EACH DEVICE ----*/
	
	TGPUPlan plan[numGPU];
	for(i =0;i < numGPU; i++)
	{
		plan[i].hA = hA / numGPU;
		plan[i].wA = wA;
		plan[i].wB = wB;
		//cudaEventCreate(&start[i]);
		//cudaEventCreate(&stop[i]);
	}
	
	/*.........To handle odd size of vectors.........*/
	for(i = 0;i < hA % numGPU; i++)		
		plan[i].hA++;

	for(i = 0; i<numGPU ; i++)	
	{
		plan[i].hMatA=(double*)malloc(plan[i].hA*plan[i].wA*sizeof(double));
		plan[i].hMatB=(double*)malloc(plan[i].wA*plan[i].wB*sizeof(double));
		plan[i].hMatC=(double*)malloc(plan[i].hA*plan[i].wB*sizeof(double));
	}

	
	/*--------Division of input matrix for different GPU's-----*/

	gpuBase=0;
	for(i =0;i < numGPU ;i++)
	{
		plan[i].hMatA = host_A + gpuBase ;
		plan[i].hMatB = host_B ;
		gpuBase += plan[i].hA * plan[i].wA ; 	
	}


	for( i=0; i<numGPU ;i++)
	{
		CUDA_SAFE_CALL(cudaSetDevice(i));
		CUDA_SAFE_CALL(cudaStreamCreate(&plan[i].stream));
	}


	/*-----------GPU Computation------------*/
	
	for( i=0; i<numGPU ;i++)
	{
		CUDA_SAFE_CALL(cudaSetDevice(i));
		
		
		CUDA_SAFE_CALL(cudaMalloc((void**)&plan[i].dMatA,plan[i].hA*plan[i].wA*sizeof(double)));
		CUDA_SAFE_CALL(cudaMalloc((void**)&plan[i].dMatB,plan[i].wA*plan[i].wB*sizeof(double)));
		CUDA_SAFE_CALL(cudaMalloc((void**)&plan[i].dMatC,plan[i].hA*plan[i].wB*sizeof(double)));



		CUDA_SAFE_CALL(cudaMemcpyAsync(plan[i].dMatA,plan[i].hMatA,plan[i].hA*plan[i].wA*sizeof(double),cudaMemcpyHostToDevice,plan[i].stream));
		CUDA_SAFE_CALL(cudaMemcpyAsync(plan[i].dMatB,plan[i].hMatB,plan[i].wA*plan[i].wB*sizeof(double),cudaMemcpyHostToDevice,plan[i].stream));
		

		//CUDA_SAFE_CALL(cudaEventRecord(start[i],plan[i].stream));
		
		
		dim3 dimBlock(BLOCKSIZE,BLOCKSIZE);
		
		int gridX=1,gridY=1;
		if( plan[i].wB >= BLOCKSIZE )
			gridX=plan[i].wB/BLOCKSIZE;
		if( plan[i].hA >= BLOCKSIZE )
			gridY=plan[i].hA/BLOCKSIZE;
		
		dim3 dimGrid(gridX,gridY);

		 mmmul<<<dimGrid,dimBlock,0,plan[i].stream>>>(plan[i].dMatA,plan[i].dMatB,plan[i].dMatC,hA,wA,wB);
      
		//CUDA_SAFE_CALL(cudaEventRecord(stop[i],plan[i].stream));
		//CUDA_SAFE_CALL(cudaEventSynchronize(stop[i]));
		
		//printf("\nDevice status:%d:%d:%s\n",i,cudaPeekAtLastError(),cudaGetErrorString(cudaPeekAtLastError()));	
		
		CUDA_SAFE_CALL(cudaMemcpyAsync(plan[i].hMatC,plan[i].dMatC,plan[i].hA*plan[i].wB*sizeof(double),cudaMemcpyDeviceToHost,plan[i].stream));
	
	}
	


	/*--------- PROCESS RESULTS FROM GPU ----------*/

	offset=0;
	for(i=0; i<numGPU ; i++)
	{	 
		CUDA_SAFE_CALL(cudaSetDevice(i));
		cudaStreamSynchronize(plan[i].stream);
		
	
	
		for( j=0;j < plan[i].hA*plan[i].wB ; j++ )
			host_C[j+offset] = plan[i].hMatC[j];	
		offset += plan[i].hA * plan[i].wB ;
		//printf("Device status:%d:%s\n",cudaPeekAtLastError(),cudaGetErrorString(cudaPeekAtLastError()));	
		
		/*
		free(plan[i].hMatA);	
		free(plan[i].hMatB);	
		free(plan[i].hMatC);	
		*/	
		CUDA_SAFE_CALL(cudaFree(plan[i].dMatA));
		CUDA_SAFE_CALL(cudaFree(plan[i].dMatB));
		CUDA_SAFE_CALL(cudaFree(plan[i].dMatC));

		CUDA_SAFE_CALL(cudaStreamDestroy(plan[i].stream));
		
	//	CUDA_SAFE_CALL(cudaEventDestroy(start[i]));
	//	CUDA_SAFE_CALL(cudaEventDestroy(stop[i]));
		
	//	cudaEventElapsedTime(&elapsedTime[i],start[i],stop[i]);
	//	Tsec +=elapsedTime[i];
	}
		
	//CUDA_SAFE_CALL(cudaEventRecord(stop[0],0));//plan[i].stream));
	//CUDA_SAFE_CALL(cudaEventSynchronize(stop[0]));

//	cudaEventElapsedTime(&elapsedTime[0],start[0],stop[0]);
//	Tsec +=elapsedTime[0];

	//printf("\n\nTsec:%f\n",Tsec);
	//gflops = (1.0e-12)*((2.0*hA*wA*wB)/Tsec);	
	//printf("\n\nGflops:%f\n",gflops);

	checkResult(host_A,host_B,host_C,hA,wB,wA);	
	//print_on_screen("MatMatMult_mGPU",Tsec,gflops,SIZE,1);
	
}


/***********************************************************************************
Routine for verifiying the CPU+GPU results against the CPU results
************************************************************************************/
void checkResult(double *InMatA, double *InMatB, double *outMatC, int m, int n , int k )
{
        int     i;
        int     j;
        int     k1;
        double  *tempOut;

        tempOut  = (double*) malloc (m * n * sizeof(double));
        if (tempOut == 0){
        printf("\n Memory allocation Failed for Resultant Matrix");
        exit (0);
        }

	/* CPU Compuation Performs operation using CBLAS */
        //cblas_dgemm (CblasColMajor, CblasNoTrans, CblasNoTrans, m, n , k, alpha, InMatA, m , InMatB , k, beta, tempOut, m);

        /****************************************************************** 
	Serial computation
	uncomment the below section if want to do the CPU computation
	using i,j,k loop method. Method work only for square matrices.
	 *******************************************************************/
         for (i = 0; i < m  ; ++i) {
                for (j = 0; j < n; ++j) {
                        double  cprod = 0;
                        for (k1 = 0; k1 < k; ++k1) 
                                cprod += InMatA[k1 + k* i] * InMatB[j + n * k1];
                tempOut[j + n * i] = cprod;//alpha * cprod + beta * tempOut[j * n + i];

                }
        }
	
	printf("\n..............\n");

	relError(outMatC,tempOut,m*n);
	free(tempOut);
}
