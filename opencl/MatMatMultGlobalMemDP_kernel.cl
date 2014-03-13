/* Kernel for Matrix Multiplication Global memory -single precision 
*/

#if defined(cl_khr_fp64)  // Khronos extension available?
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#elif defined(cl_amd_fp64)  // AMD extension available?
#pragma OPENCL EXTENSION cl_amd_fp64 : enable
#endif
__kernel void matMatMultKernel(__global double *output,__global double *d_MatA, __global double *d_MatB, __global int* d_rows, __global int* d_cols)
{
	int globalIdx = get_global_id(0);
       int globalIdy = get_global_id(1);
       double sum =0.0;
	int i;
	double tempA,tempB;
       for ( i=0; i< (*d_rows); i++)
       {
         tempA = d_MatA[globalIdy * (*d_rows) + i];
         tempB = d_MatB[i * (*d_cols) + globalIdx];
         sum += tempA * tempB;
        }
       output[globalIdy * (*d_rows) + globalIdx] = sum;
}

