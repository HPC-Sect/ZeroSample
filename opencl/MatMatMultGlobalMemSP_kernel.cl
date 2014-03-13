/* Kernel for Matrix Multiplication Global memory -single precision 
*/ 
__kernel void matMatMultKernel(__global float *output,__global float *d_MatA, __global float *d_MatB, __global int* d_rows, __global int* d_cols)
{
	int globalIdx = get_global_id(0);
       int globalIdy = get_global_id(1);
       int sum =0,i,tempA,tempB;
       for ( i=0; i< (*d_rows); i++)
       {
         tempA = d_MatA[globalIdy * (*d_rows) + i];
         tempB = d_MatB[i * (*d_cols) + globalIdx];
         sum += tempA * tempB;
        }
       output[globalIdy * (*d_rows) + globalIdx] = sum;
}

