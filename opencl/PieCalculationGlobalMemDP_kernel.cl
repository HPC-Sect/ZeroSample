
#if defined(cl_khr_fp64)  // Khronos extension available?
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#elif defined(cl_amd_fp64)  // AMD extension available?
#pragma OPENCL EXTENSION cl_amd_fp64 : enable
#endif

__kernel void pieCalculation(__global int *numOfIntervals, __global double* area)
{
	int globalId = get_global_id(0);
	int intervalCount = globalId;
       	double intervalMidPoint = 0.0;
       	double distance=0.5;
	int noOfint = *numOfIntervals;
       	double intervalWidth = 1.0 / (*numOfIntervals);
       	double sum = 0.0;
       	double tempResult = 0.0;
       	int icnt;
	if(globalId < (*numOfIntervals))
       	{
               intervalMidPoint = intervalWidth*((intervalCount+1)-distance);
               sum=(4.0/(1.0 + intervalMidPoint*intervalMidPoint));
       	}
       	area[intervalCount] = intervalWidth * sum;
       	barrier(CLK_GLOBAL_MEM_FENCE);
	if(globalId==0)
       	{
		tempResult = 0.0;
                for(icnt=0;icnt < noOfint;icnt++)
                {
                       tempResult += area[icnt];
                }
       	*area = tempResult;
       	}
}
