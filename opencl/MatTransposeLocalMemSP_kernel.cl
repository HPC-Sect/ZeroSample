/* Kernel for Matrix transpose local memory single precsion 
*/
#define BLOCK_SIZE 16
__kernel void transMatrix(__global float* d_MatA, __global float* output, __global int* d_rows,__local float *As)
{
	unsigned int xIndex = get_global_id(0);
        unsigned int yIndex = get_global_id(1);

        if((xIndex  < (*d_rows)) && (yIndex < (*d_rows)))
        {
                unsigned int index_in = yIndex * (*d_rows) + xIndex ;
                As[get_local_id(1)*(BLOCK_SIZE)+get_local_id(0)] = d_MatA[index_in];
        }

        barrier(CLK_LOCAL_MEM_FENCE);

        xIndex = get_group_id(1) * BLOCK_SIZE + get_local_id(0);
        yIndex = get_group_id(0) * BLOCK_SIZE + get_local_id(1);
        if((xIndex < (*d_rows)) && (yIndex  < (*d_rows)))
    	{
                unsigned int index_out = yIndex * (*d_rows) + xIndex;
                output[index_out] = As[get_local_id(0)*(BLOCK_SIZE)+get_local_id(1)];
        }

}

