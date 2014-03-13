/*  Kernel for scalar-vector multiplication global memory double precision  
*/

#if defined(cl_khr_fp64)  // Khronos extension available?
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#elif defined(cl_amd_fp64)  // AMD extension available?
#pragma OPENCL EXTENSION cl_amd_fp64 : enable
#endif
__kernel void scalVectMultKernelDp(__global double* input, __global double* output, __global int* scalar)
{
     int threadGId = get_global_id(0);
     output[threadGId] = input[threadGId] * (*scalar) ;
}
