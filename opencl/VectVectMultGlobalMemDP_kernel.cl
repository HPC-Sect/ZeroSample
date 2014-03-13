/*  Kernel for Vector-vector multiplication global memory double precision 
*/

#if defined(cl_khr_fp64)  // Khronos extension available?
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#elif defined(cl_amd_fp64)  // AMD extension available?
#pragma OPENCL EXTENSION cl_amd_fp64 : enable
#endif
__kernel void VectVectMulKernel(__global double *d_VectA,__global double *d_VectB, __global double *outScalar, __global int *length)
{
    unsigned int gid = get_global_id(0);
     int currCell;
        if(gid<*length)
      d_VectA[ gid]*= d_VectB[gid];

    barrier(CLK_LOCAL_MEM_FENCE);
    if( gid == 0 )
        {

        *outScalar = 0.0;
        for(currCell=0;currCell<*length;currCell++)
                *outScalar = (*outScalar) +  d_VectA[currCell];
    }
}


