__kernel void pieCalculation(__global int  *numOfIntervals, __global float* area)
{
	int globalId = get_global_id(0);
       int intervalCount = globalId;
       float intervalMidPoint = 0.0f;
       float distance=0.5f;
       int noOfint = *numOfIntervals;
       float intervalWidth = 1.0 / (*numOfIntervals);
       float sum = 0.0f;
       float tempResult = 0.0f;
       int icnt;
	if(globalId<(*numOfIntervals))
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
