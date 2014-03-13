/* kernel for Matrix infinity norm
*/
__kernel void infinityNorm_kernel(__global float* input, __global int* rowCol, __global float* infiNorm)
{
int threadGid = get_global_id(0);
unsigned int sum;
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
