/* Kernel for Matrix Addition 
*/

#if defined(cl_khr_fp64)  // Khronos extension available?
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#elif defined(cl_amd_fp64)  // AMD extension available?
#pragma OPENCL EXTENSION cl_amd_fp64 : enable
#endif

__kernel void MatMatAddKernelGMDP(__global double *d_MatA,__global double *d_MatB, __global double *output)
{
 size_t id = (get_global_id(1) * get_global_size(0) + get_global_id(0));
 output[id] = d_MatA[id] + d_MatB[id];
}

