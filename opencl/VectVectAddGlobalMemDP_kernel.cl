/* kernel for vector-addition  global memory double precsion 
*/

#if defined(cl_khr_fp64)  // Khronos extension available?
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#elif defined(cl_amd_fp64)  // AMD extension available?
#pragma OPENCL EXTENSION cl_amd_fp64 : enable
#endif
__kernel void VectVectAddDPKernel(__global double *d_VectA,__global double *d_VectB, __global double *output)
{
 size_t id = get_global_id(0);
 output[id] = d_VectA[id] + d_VectB[id];
}


