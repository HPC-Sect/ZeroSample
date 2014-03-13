/* Kernel for Vector Addition  global memory single precsion 
*/
__kernel void VectVectAddKernel(__global float *d_VectA,__global float *d_VectB, __global float *output)
{
 size_t id = get_global_id(0);
 output[id] = d_VectA[id] + d_VectB[id];
}
