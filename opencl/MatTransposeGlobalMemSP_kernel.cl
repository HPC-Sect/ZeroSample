/* Kernel for Matrix transpose operation single precision global memory 
*/
__kernel void transMatrix(__global float* input, __global float* output, __global int* d_rows)
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

