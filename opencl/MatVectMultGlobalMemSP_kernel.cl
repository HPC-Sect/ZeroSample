/* Kernel for matrix-vector multiplication global memory single precision 
*/
__kernel void matrixvectorMult(__global float* d_Mat, __global float* d_Vect,__global float* result_vec, __global int* vecLen , __global int* numRows)        
{   
       int threadGid = get_global_id(0);    
       int numThread = get_global_size(0); 
       float tempResult = 0; 
       if( threadGid < (*numRows))         
       {
               for(int colCount = 0; colCount < (*vecLen); colCount++ )
               {
                       tempResult = tempResult + d_Vect[colCount] * d_Mat[threadGid * (*vecLen) + colCount];
               }
               result_vec[threadGid] = tempResult; 
        }
}          

