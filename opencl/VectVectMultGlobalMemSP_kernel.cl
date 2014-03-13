/*  Kernel for Vector-Vector multiplication global memory single precision 
*/
__kernel void VectVectMulKernel(__global float *d_VectA,__global float *d_VectB, __global float *outScalar, __global int *length)
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
