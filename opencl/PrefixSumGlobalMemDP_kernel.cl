/* Kernel for Prefix sum global memory double precision
*/

#if defined(cl_khr_fp64)  // Khronos extension available?
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#elif defined(cl_amd_fp64)  // AMD extension available?
#pragma OPENCL EXTENSION cl_amd_fp64 : enable
#endif
__kernel void prefixSum_kernel(__global double *inArray, __global double *outArray, __global int* arrayLength)
 {
    	unsigned int gid = get_global_id(0);
    	unsigned int numDim = get_work_dim(); 
    	double prefixSum = 0;
	int count;
	for( int count = 0; count < gid; count++)
	{
		int value=gid;
		prefixSum += inArray[count];
	}
    	outArray[gid] = prefixSum ;
 } 
