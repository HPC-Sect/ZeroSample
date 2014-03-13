/*  Kernel for matrix-vector multiplication local memory single precision 
*/
#define BLOCK_SIZE 16
__kernel void matrixvectorMult(__global float* d_Mat, __global float* d_Vect,__global float* result_vec, __global int* vecLen , __global int* numRows, __local float *As)        
{
	int y;
        int x;
        int a;
        for ( y = get_group_id(0); y < (*vecLen); y += get_num_groups(0)) 
        {
                float sum = 0.0f;
                for (x = get_local_id(0); x < (*vecLen); x += get_local_size(0))
                        sum += d_Mat[y * (*vecLen) + x] * d_Vect[x];
                As[get_local_id(0)] = sum;
                barrier(CLK_LOCAL_MEM_FENCE);
                if (get_local_id(0) == 0)
                {
                        float dotProduct = 0.0f;
                        for (a = 0; a < get_local_size(0); ++a)
                        dotProduct += As[a];
                        result_vec[y] = dotProduct;
                }
                barrier(CLK_LOCAL_MEM_FENCE);
        }

}          
