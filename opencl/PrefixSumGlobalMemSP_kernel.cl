/* Kernel for Prefix sum global memory single precision
*/

__kernel void prefixSum_kernel(__global float *inArray, __global float *outArray, __global int* arrayLength)
 {
    	unsigned int gid = get_global_id(0);
    	float prefixSum = 0.0;
	int count;
	for( int count = 0; count < gid; count++)
	{
		int value=gid;
		prefixSum += inArray[count];
	}
    	outArray[gid] = prefixSum ;
 } 
