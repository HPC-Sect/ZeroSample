/*******************************************************************

comment	      :  Set of execution kernel and subroutine used in
		 memcpyLatencyHiding computing package.

File          :  ComputingKernel.cu

*******************************************************************/


/************************************************************************************
*  pragma routine to report the detail of cuda error 
************************************************************************************/
#define CUDA_SAFE_CALL(call)                                                    \
            do{                                                                 \
                cudaError_t err = call;                                         \
                if(err != cudaSuccess)                                          \
                 {                                                              \
                   fprintf(stderr, "Cuda error in file '%s' in line %i : %s.\n",\
                   __FILE__, __LINE__, cudaGetErrorString( err) );              \
                   exit(1);                                                     \
                 }                                                              \
             } while (0)                                                        \


clock_t last_time = 0;

/************************************************************************************
* routine displaying time detail
************************************************************************************/
void timestamp(char* message)
  {
        clock_t current_time = (clock()*1000) / CLOCKS_PER_SEC;
        fprintf(stderr,"%s +%dms (overall time=%dms)\n",message,current_time - last_time,current_time);
        last_time=current_time;
  }


/************************************************************************************
* Matrix transpose kernel
************************************************************************************/
__global__ void MatTranspose(float *dInMat, float *dOutMat, int matRowColSize, int threadDim)
  {
    int tidx = threadIdx.x;
    int tidy = threadIdx.y;
    int tindex = (threadDim * tidx) + tidy;
    int maxNumThread = threadDim * threadDim;
    int pass = 0;
    int rowCount ;
    int curColInd;

    while( (curColInd = (tindex + maxNumThread * pass))  < matRowColSize )
     {
        for( rowCount = 0; rowCount < matRowColSize; rowCount++)
          dOutMat[curColInd * matRowColSize + rowCount] = dInMat[rowCount* matRowColSize + curColInd];
        pass++;
      }
     __syncthreads();

  }//end of VectVect device function

/************************************************************************************
* Matrix Matrix Addision Kernel
************************************************************************************/
__global__ void MatAdd(float *dInMatA, float *dInMatB, int matRowColSize, int threadDim)
  {
    int tidx = threadIdx.x;
    int tidy = threadIdx.y;
    int tindex = (threadDim * tidx) + tidy;
    int maxNumThread = threadDim * threadDim;
    int pass = 0;
    int rowCount ;
    int curColInd;

    while( (curColInd = (tindex + maxNumThread * pass))  < matRowColSize )
     {
        for( rowCount = 0; rowCount < matRowColSize; rowCount++)
          dInMatA[curColInd * matRowColSize + rowCount] += dInMatB[curColInd * matRowColSize + rowCount];
        pass++;
      }
     __syncthreads();
  }/* end of Muld device code */


/************************************************************************************
* Multiplication of matrix with scaler value
*************************************************************************************/
__global__ void scalMatMult(float *dInMat,float scal,int matRowSize, int matColSize, int threadDim)
  {
    int tidx = threadIdx.x;
    int tidy = threadIdx.y;
    int tindex = (threadDim * tidx) + tidy;
    int maxNumThread = threadDim * threadDim;
    int pass = 0;
    int colCount;
    int curRowInd;

    while( (curRowInd = (tindex + maxNumThread * pass))  < matRowSize )
     {
        for( colCount = 0; colCount < matColSize; colCount++)
          dInMat[curRowInd * matRowSize + colCount] += scal * dInMat[curRowInd* matRowSize + colCount];
        pass++;
     }

     __syncthreads();

  }//end of VectVect device function

