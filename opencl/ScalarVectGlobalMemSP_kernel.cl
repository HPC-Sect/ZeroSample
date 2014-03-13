 /*  Kernel for scalar-vector multiplication global memory single precision  
*/
__kernel void scalVectMultKernelSp(__global float* input, __global float* output, __global int* scalar)
{
     size_t threadGId = get_global_id(0);
     output[threadGId] = input[threadGId] * (*scalar) ;
}
