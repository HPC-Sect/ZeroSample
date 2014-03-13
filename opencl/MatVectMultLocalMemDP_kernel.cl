/* Kernel for matrix-vector multiplication local memory double precision 
*/

#if defined(cl_khr_fp64)  // Khronos extension available?
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#elif defined(cl_amd_fp64)  // AMD extension available?
#pragma OPENCL EXTENSION cl_amd_fp64 : enable
#endif
__kernel void matrixvectorMult(__global double* d_Mat, __global double* d_Vect,__global double* result_vec, __global int* vecLen , __global int* numRows, __local double *As)
{
	int y;
        int x;
        int a;
        for ( y = get_group_id(0); y < (*vecLen); y += get_num_groups(0)) 
        {
                double sum = 0.0;
                for (x = get_local_id(0); x < (*vecLen); x += get_local_size(0))
                        sum += d_Mat[y * (*vecLen) + x] * d_Vect[x];
                As[get_local_id(0)] = sum;
                barrier(CLK_LOCAL_MEM_FENCE);
                if (get_local_id(0) == 0)
                {
                        double dotProduct = 0.0;
                        for (a = 0; a < get_local_size(0); ++a)
                        dotProduct += As[a];
                        result_vec[y] = dotProduct;
                }
                barrier(CLK_LOCAL_MEM_FENCE);
        }
} 
