/* Kernel for Matrix transpose local memory double precsion 
*/
#define BLOCK_SIZE 32

#if defined(cl_khr_fp64)  // Khronos extension available?
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#elif defined(cl_amd_fp64)  // AMD extension available?
#pragma OPENCL EXTENSION cl_amd_fp64 : enable
#endif
__kernel void transMatrixDP(__global double* d_MatA, __global double* output, __global int* d_rows,__local double *As)
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

