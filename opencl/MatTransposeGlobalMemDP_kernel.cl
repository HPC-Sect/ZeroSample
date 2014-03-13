/* Kernel for Matrix transpose operation double precision global memory 
*/
#if defined(cl_khr_fp64)  // Khronos extension available?
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#elif defined(cl_amd_fp64)  // AMD extension available?
#pragma OPENCL EXTENSION cl_amd_fp64 : enable
#endif
__kernel void transMatrix(__global double* input, __global double* output, __global int* d_rows)
{
	unsigned int xIndex = get_global_id(0);
    	unsigned int yIndex = get_global_id(1);

    	if (xIndex  < (*d_rows) && yIndex < (*d_rows))
    	{
        	unsigned int index_in  = xIndex  + (*d_rows) * yIndex;
        	unsigned int index_out = yIndex + (*d_rows) * xIndex;
        	output[index_out] = input[index_in]; 
    	}

}

