/* Kernel for scalar-matrix multiplication global memory double precison 
*/
#if defined(cl_khr_fp64)  // Khronos extension available?
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#elif defined(cl_amd_fp64)  // AMD extension available?
#pragma OPENCL EXTENSION cl_amd_fp64 : enable
#endif
__kernel void scalarMatrixMultDP(__global double *d_Mat, __global int* d_scalar,__global double *result_mat, __global int* numCols , __global int* numRows)        
{   
       int threadGid = get_global_id(0);    
       int numThread = get_global_size(0); 
       int colCount;
       if( threadGid < (numThread))         
       {
               for(colCount = 0; colCount < (*numCols); colCount++ )
               {
                       result_mat[threadGid * (*numCols) + colCount] =  d_Mat[threadGid * (*numCols) + colCount] *  (*d_scalar);
               }
        }
};          

