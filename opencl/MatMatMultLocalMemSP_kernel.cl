/* Kernel for Matrix Multiplication local memory -single precision 
*/ 
#define BLOCK_SIZE 32 
__kernel void matMatMultKernel(__global float *output,__global float *d_MatA, __global float *d_MatB, __local float *As, __local float *Bs, __global int* d_rows, __global int* d_cols)
{
        int aBegin;
        int a;
        int b;
        int k;
        int c;
        float Csub = 0.0f;
        int bx = get_group_id(0);
        int by = get_group_id(1);
        int tx = get_local_id(0);
        int ty = get_local_id(1);
        aBegin  = ((*d_rows) * BLOCK_SIZE * by);
        int aEnd = aBegin + (*d_rows) - 1;
        int aStep  = BLOCK_SIZE;
        int bBegin  =  BLOCK_SIZE * bx;
        int bStep = BLOCK_SIZE * (*d_rows);
        for(a = aBegin, b = bBegin; a <= aEnd ; a  += aStep, b += bStep)
        {
        As[tx + ty * BLOCK_SIZE] = d_MatA[a + (*d_rows) * ty + tx];
        Bs[tx + ty * BLOCK_SIZE] = d_MatB[b+ (*d_cols) * ty + tx];
        barrier(CLK_LOCAL_MEM_FENCE);
        for( k= 0; k< BLOCK_SIZE; ++k)
        Csub += As[k + ty * BLOCK_SIZE] * Bs[tx + k *BLOCK_SIZE];
        barrier(CLK_LOCAL_MEM_FENCE);
        }
        output[get_global_id(1) * get_global_size(0) + get_global_id(0)] = Csub;

}

