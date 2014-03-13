/* Kernel for matrix-vector multiplication global memory double precision 
*/

#if defined(cl_khr_fp64)  // Khronos extension available?
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#elif defined(cl_amd_fp64)  // AMD extension available?
#pragma OPENCL EXTENSION cl_amd_fp64 : enable
#endif
__kernel void matrixvectorMult(__global double* d_Mat, __global double* d_Vect,__global double* result_vec, __global int* vecLen , __global int* numRows)
{   
       int threadGid = get_global_id(0);
       int numThread = get_global_size(0);
       double tempResult = 0.0; 
       if( threadGid < (*numRows))
       {
               for(int colCount = 0; colCount < (*vecLen); colCount++ )
               {
                       tempResult = tempResult + d_Vect[colCount] * d_Mat[threadGid * (*vecLen) + colCount];
               }    
               result_vec[threadGid] = tempResult; 
        }   
}; 
