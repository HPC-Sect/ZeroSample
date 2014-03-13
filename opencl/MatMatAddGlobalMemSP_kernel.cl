/* Kernel for Matrix addtion global memory single precision 
*/
__kernel void MatMatAddKernelGMSP(__global float *d_MatA,__global float *d_MatB, __global float *output)
{
 size_t id = (get_global_id(1) * get_global_size(0) + get_global_id(0));
 output[id] = d_MatA[id] + d_MatB[id];
}

