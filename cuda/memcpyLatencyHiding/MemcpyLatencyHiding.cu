/*******************************************************************
                     
     
File          :  MemcpyLatencyHiding.cu  

Description   :  This program is designed to demonstrate some strategy to hide bandwith latency
	         and do concurrent execution of some execution kernel through one stream, while
	         memory copy of data set is also going one for the purpose of executon through
	         other kernel.

Input	      : Matrix Dimension. [ specified through preprocessor directive. ] 


Output        : This primary version of code with un-optimized kernel shows 3 to 4 percent 
		improvement in execution time because of applying ladder execution model 
		approach. 
		The Output shows the comparision of execution time of executing same set of task 
		in both mode[ no-stream and stream ].
**********************************************************************/


#include<stdio.h>
#include<cuda.h>
#include<math.h>
#include<time.h>
#include "ComputingKernel.cu"  

#define NSTREAM 3                // number of stream will be used for execution : this also specify , the type of 
				 // execution. currently we are  doing one three king of execution mat-mat-mult, 
				 // mat-transpose- and mat-scal-mult.

#define BLOCK_SIZE 16             // Thread Block Dimension

/************************************************************************************
* Number of repetation of execution of algorithm through stream
************************************************************************************/
#define REP_COUNT 1
//#define REP_COUNT 2
//#define REP_COUNT 4
//#define REP_COUNT 8
//#define REP_COUNT 16
//#define REP_COUNT 32
//#define REP_COUNT 64


/************************************************************************************
* Define matrix dimension for execution
************************************************************************************/
//#define MAT_DIMM  1024
//#define MAT_DIMM  2048
#define MAT_DIMM  4096
//#define MAT_DIMM  8192

/************************************************************************************
* Show info regarding code
************************************************************************************/
void info()
 {
   printf("\n---------------------------------------------------------------------\n");
   printf(" Kernels : A = A + B, A = A(T) , A = x * A   \n \t\t[Where A and B is Block Matrix]");
   printf("\n Matrix Dimension : %d", MAT_DIMM);
   printf("\n Number of execution stream  : %d", NSTREAM);
   printf("\n---------------------------------------------------------------------\n");
 }

/************************************************************************************
* Memory Copy latency hide function
************************************************************************************/
int memcpyLatencyHide()
 {
     int count, rCount;
     cudaStream_t *stream = (cudaStream_t*) malloc ( NSTREAM * sizeof(cudaStream_t));
  // define all host matrix block
     float *hTransMat, *hAddMatMatA, *hAddMatMatB, *hMatScaler;
  
  // define device matrix block
     float *dTransMat, *dTransMatOut; 
     float *dAddMatMatA, *dAddMatMatB;  
     float *dMatScaler; 	
    
  // allocate and initialize an array of stream handles
     for(count = 0; count< NSTREAM; count++)
	CUDA_SAFE_CALL( cudaStreamCreate(&(stream[count])));
 
  // allocate memory at host
     CUDA_SAFE_CALL( cudaMallocHost((void**)&hTransMat , MAT_DIMM * MAT_DIMM * sizeof(float)));
     CUDA_SAFE_CALL( cudaMallocHost((void**)&hAddMatMatA , MAT_DIMM * MAT_DIMM * sizeof(float)));
     CUDA_SAFE_CALL( cudaMallocHost((void**)&hAddMatMatB , MAT_DIMM * MAT_DIMM * sizeof(float)));
     CUDA_SAFE_CALL( cudaMallocHost((void**)&hMatScaler , MAT_DIMM * MAT_DIMM * sizeof(float)));

  // assign value to input matrises
     for(count=0; count< MAT_DIMM * MAT_DIMM; count++){
	hTransMat[count] = rand() * 2.109;
	hAddMatMatA[count] = rand() * 1.02;
	hAddMatMatB[count] = rand() * 1.99;
	hMatScaler[count] = rand() * 1.11;
     }
  
  // allocate device memory
     CUDA_SAFE_CALL( cudaMalloc((void**) &dTransMat, MAT_DIMM * MAT_DIMM * sizeof(float))); 
     CUDA_SAFE_CALL( cudaMalloc((void**) &dTransMatOut, MAT_DIMM * MAT_DIMM * sizeof(float))); 
     CUDA_SAFE_CALL( cudaMalloc((void**) &dAddMatMatA, MAT_DIMM * MAT_DIMM * sizeof(float))); 
     CUDA_SAFE_CALL( cudaMalloc((void**) &dAddMatMatB, MAT_DIMM * MAT_DIMM * sizeof(float))); 
     CUDA_SAFE_CALL( cudaMalloc((void**) &dMatScaler, MAT_DIMM * MAT_DIMM * sizeof(float))); 

  // define cuda event variable and create handles
     //cudaEvent_t start, stop;
     //CUDA_SAFE_CALL( cudaEventCreate(&start));
     //CUDA_SAFE_CALL( cudaEventCreate(&stop));
 
  // define kernel dimension 
     dim3 transGrid(1,1), addGrid(1,1), scalGrid(1,1);
     dim3 transBlock(BLOCK_SIZE, BLOCK_SIZE), addBlock(BLOCK_SIZE, BLOCK_SIZE), scalBlock(BLOCK_SIZE, BLOCK_SIZE);


     timestamp(" Starting Stream execution Block :  ");
     //-------------------------------------------
 for(rCount = 0; rCount < REP_COUNT; rCount++)
   {
     CUDA_SAFE_CALL( cudaMemcpyAsync(dTransMat, hTransMat, MAT_DIMM * MAT_DIMM * sizeof(float), cudaMemcpyHostToDevice, stream[0])); 
     //-------------------------------------------
     MatTranspose<<<transGrid, transBlock,128, stream[0]>>>( dTransMat, dTransMatOut, MAT_DIMM,BLOCK_SIZE);
     CUDA_SAFE_CALL( cudaMemcpyAsync(dAddMatMatA, hAddMatMatA, MAT_DIMM * MAT_DIMM * sizeof(float), cudaMemcpyHostToDevice, stream[1])); 
     CUDA_SAFE_CALL( cudaMemcpyAsync(dAddMatMatB, hAddMatMatB, MAT_DIMM * MAT_DIMM * sizeof(float), cudaMemcpyHostToDevice, stream[1])); 
     //-------------------------------------------
     CUDA_SAFE_CALL( cudaMemcpyAsync(hTransMat,dTransMatOut, MAT_DIMM * MAT_DIMM * sizeof(float), cudaMemcpyDeviceToHost, stream[0])); 
     MatAdd<<<addGrid, addBlock, 128, stream[1]>>>(dAddMatMatA,dAddMatMatB, MAT_DIMM,BLOCK_SIZE);
     //-------------------------------------------
     CUDA_SAFE_CALL( cudaMemcpyAsync(dMatScaler,hMatScaler, MAT_DIMM * MAT_DIMM * sizeof(float), cudaMemcpyHostToDevice, stream[2])); 
     //-------------------------------------------
     CUDA_SAFE_CALL( cudaMemcpyAsync(hAddMatMatA, dAddMatMatA, MAT_DIMM * MAT_DIMM * sizeof(float), cudaMemcpyDeviceToHost, stream[1])); 
     scalMatMult<<<scalGrid,scalBlock,128,stream[2]>>>(dMatScaler,(float)(rand()*2.99),MAT_DIMM, MAT_DIMM, BLOCK_SIZE);
   }
     cudaThreadSynchronize();
     timestamp(" End Stream execution Block :  ");
   
   
     timestamp("\n Starting Non Stream Execution Block :  ");
 for(rCount = 0; rCount<REP_COUNT; rCount++)
  {
     //-------------------------------------------
     CUDA_SAFE_CALL( cudaMemcpy(dTransMat, hTransMat, MAT_DIMM * MAT_DIMM * sizeof(float), cudaMemcpyHostToDevice)); 
     MatTranspose<<<transGrid, transBlock,128>>>( dTransMat, dTransMatOut, MAT_DIMM,BLOCK_SIZE);
     CUDA_SAFE_CALL( cudaMemcpy(dAddMatMatA, hAddMatMatA, MAT_DIMM * MAT_DIMM * sizeof(float), cudaMemcpyHostToDevice)); 
     CUDA_SAFE_CALL( cudaMemcpy(dAddMatMatB, hAddMatMatB, MAT_DIMM * MAT_DIMM * sizeof(float), cudaMemcpyHostToDevice)); 
     //-------------------------------------------
     CUDA_SAFE_CALL( cudaMemcpy(hTransMat,dTransMatOut, MAT_DIMM * MAT_DIMM * sizeof(float), cudaMemcpyDeviceToHost)); 
     MatAdd<<<addGrid, addBlock, 128>>>(dAddMatMatA,dAddMatMatB, MAT_DIMM, BLOCK_SIZE);
     //-------------------------------------------
     CUDA_SAFE_CALL( cudaMemcpy(dMatScaler,hMatScaler, MAT_DIMM * MAT_DIMM * sizeof(float), cudaMemcpyHostToDevice)); 
     //-------------------------------------------
     CUDA_SAFE_CALL( cudaMemcpy(hAddMatMatA, dAddMatMatA, MAT_DIMM * MAT_DIMM * sizeof(float), cudaMemcpyDeviceToHost)); 
     scalMatMult<<<scalGrid,scalBlock,128>>>(dMatScaler,(float)(rand()*2.99),MAT_DIMM, MAT_DIMM, BLOCK_SIZE);
  }
     cudaThreadSynchronize();
     timestamp(" End Non Stream execution Block :  ");
     printf("\n---------------------------------------------------------------------\n");

 return 0; 
 }// end of strassenMatMat

int main(int argc, char* argv[])
 {
   info();
   memcpyLatencyHide();
 }// end of main 
