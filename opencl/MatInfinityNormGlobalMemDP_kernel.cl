/* Kernel for matrix Infinity norm  double precision
*/

#if defined(cl_khr_fp64)  // Khronos extension available?
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#elif defined(cl_amd_fp64)  // AMD extension available?
#pragma OPENCL EXTENSION cl_amd_fp64 : enable
#endif
__kernel void infinityNorm_kernel(__global double* input, __global int* rowCol, __global double* infiNorm)
{
int threadGid = get_global_id(0);
double sum;
if( threadGid < rowCol[0])
{

       sum = 0;
        for(int colCount = 0; colCount < rowCol[1]; colCount++ )
	{
                        sum = sum + (input[ threadGid * rowCol[1] + colCount ]);
                }
                input[ threadGid * rowCol[1] ] = sum;
        }
        barrier(CLK_LOCAL_MEM_FENCE);
        if( threadGid == 0 )
{
        sum = 0;
        for( int rowCount = 0; rowCount< rowCol[0] ; rowCount++)
	{
        sum = ( sum > input[ rowCount * rowCol[1] ] ? sum : input[ rowCount * rowCol[1]] );
        }
                *infiNorm = sum;

        }
};
