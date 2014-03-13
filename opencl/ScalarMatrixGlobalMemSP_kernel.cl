/* Kernel for Scalar-Matrix multiplication global memory single precison 
*/
__kernel void scalarMatrixMult(__global float *d_Mat, __global int* d_scalar,__global float *result_mat, __global int* numCols , __global int* numRows)        
{   
       int threadGid = get_global_id(0);    
       int numThread = get_global_size(0); 
       int colCount;
       if( threadGid < (numThread))         
       {
               for(colCount = 0; colCount < (*numCols); colCount++ )
               {
                       result_mat[threadGid * (*numCols) + colCount] =  d_Mat[threadGid * (*numCols) + colCount] *  (*d_scalar);
               }
        }
};          

