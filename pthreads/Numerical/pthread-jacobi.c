
/**************************************************************************************
 Example 	: pthread-jacobi.c

 Objective      : Jacobi method to solve AX = b matrix system of linear equations. 

 Input          : Class Size
		  Number of Threads

 Output         : The solution of  Ax = b or 
                  The status of convergence for given bumber of iterations

 Created	: Aug 2011
 

 E-mail      	: RarchK                                          
 
*************************************************************************************/

#include <stdio.h>
#include<pthread.h>
#include<sys/time.h>
#include<stdlib.h>

#define  MAX_ITERATIONS 1000 
#define MAXTHREADS 8

double   Distance(double *X_Old, double *X_New, int matrix_size);
pthread_mutex_t  mutex1 = PTHREAD_MUTEX_INITIALIZER;

double   **Matrix_A, *RHS_Vector;
double   *X_New, *X_Old, *Bloc_X, rno,sum;

int Number; 
void jacobi(int);

main(int argc, char **argv)
{

        double diag_dominant_factor  = 4.0000;
        double tolerance  = 1.0E-5;
        /* .......Variables Initialisation ...... */
        int matrix_size,  NoofRows, NoofCols,CLASS_SIZE,THREADS;
        int NumThreads,ithread;
        double rowsum;
        double  sum;
        int irow, icol, index, Iteration,iteration,ret_count;
        double time_start, time_end,memoryused;
        struct timeval tv;
        struct timezone tz;
        char * CLASS;
        FILE *fp;

        pthread_attr_t pta;
        pthread_t *threads;
     
        memoryused =0.0;


        printf("\n\t\t---------------------------------------------------------------------------");
        printf("\n\t\t Centre for Development of Advanced Computing (C-DAC)");
        printf("\n\t\t Email : RarchK");
        printf("\n\t\t---------------------------------------------------------------------------");
        printf("\n\t\t Objective : To Solve AX=B Linear Equation (Jacobi Method)\n ");
        printf("\n\t\t Performance for solving AX=B Linear Equation using JACOBI METHOD");
        printf("\n\t\t..........................................................................\n");

        if( argc != 3 ){

            printf("\t\t Very Few Arguments\n ");
            printf("\t\t Syntax : exec <Class-Size (Give A/B/C)> <Threads>\n");
            exit(-1);
        }
        else {
            CLASS = argv[1];
            THREADS = atoi(argv[2]);
        }
	if (THREADS > MAXTHREADS ){
		printf("\n Number of Threads must be less than or equal to 8. Aborting ...\n");
		return; 
	}
        if( strcmp(CLASS, "A" )==0){
            CLASS_SIZE = 1024;
        }
        if( strcmp(CLASS, "B" )==0){
            CLASS_SIZE = 2048;
        }
        if( strcmp(CLASS, "C" )==0){
            CLASS_SIZE = 4096;
        }


         matrix_size = CLASS_SIZE;
         NumThreads = THREADS;
         printf("\n\t\t Matrix Size :  %d",matrix_size);
         printf("\n\t\t Threads     :  %d",NumThreads);
          
        NoofRows = matrix_size; 
        NoofCols = matrix_size;
        
       
        /* Allocate The Memory For Matrix_A and RHS_Vector */
        Matrix_A = (double **) malloc(matrix_size * sizeof(double *));
        RHS_Vector = (double *) malloc(matrix_size * sizeof(double));


        /* Populating the Matrix_A and RHS_Vector */
        rowsum = (double) (matrix_size *(matrix_size+1)/2.0);
        for (irow = 0; irow < matrix_size; irow++)
        { 
             Matrix_A[irow] = (double *) malloc(matrix_size * sizeof(double)); 
             sum = 0.0;
             for (icol = 0; icol < matrix_size; icol++)
             {
                 Matrix_A[irow][icol]= (double) (icol+1);
                 if(irow == icol )  Matrix_A[irow][icol] = (double)(rowsum);
                 sum = sum + Matrix_A[irow][icol];
             }
             RHS_Vector[irow] = (double)(2*rowsum) - (double)(irow+1);
         }
          
         memoryused+=(NoofRows * NoofCols * sizeof(double));
         memoryused+=(NoofRows * sizeof(double));  
         
         printf("\n");

         if (NoofRows != NoofCols) 
         {
                printf("Input Matrix Should Be Square Matrix ..... \n");
                exit(-1);
         }

        /* Dynamic Memory Allocation */
        X_New = (double *) malloc(matrix_size * sizeof(double));
        memoryused+=(NoofRows * sizeof(double));
        X_Old = (double *) malloc(matrix_size * sizeof(double));
        memoryused+=(NoofRows * sizeof(double));
        Bloc_X = (double *) malloc(matrix_size * sizeof(double));
        memoryused+=(NoofRows * sizeof(double));

        /* Calculating the time of Operation Start */
        gettimeofday(&tv, &tz);
        time_start= (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;

        /* Initailize X[i] = B[i] */

        for (irow = 0; irow < matrix_size; irow++)
        {
                Bloc_X[irow] = RHS_Vector[irow];
                X_New[irow] =  RHS_Vector[irow];
        }
  
        /* Allocating the memory for user specified number of threads */
        threads = (pthread_t *) malloc(sizeof(pthread_t) * NumThreads);  

        /* Initializating the thread attribute */
        pthread_attr_init(&pta);

        Iteration = 0;
        do {
              for(index = 0; index < matrix_size; index++) 
              X_Old[index] = X_New[index];
              for(ithread=0;ithread<NumThreads;ithread++)
              {

                 /* Creating The Threads */                 
                 ret_count=pthread_create(&threads[ithread],&pta,(void *(*) (void *))jacobi, (void *) (matrix_size/NumThreads));
    		 if(ret_count)
    		 {
			printf("\n ERROR : Return code from pthread_create() is %d ",ret_count);
			exit(-1);
    		 }

               }

                  Iteration++;
                  for (ithread=0; ithread<NumThreads; ithread++)
                  {
                  	ret_count=pthread_join(threads[ithread], NULL); 
			if(ret_count)
    			{
				printf("\n ERROR : Return code from pthread_join() is %d ",ret_count);
				exit(-1);
    			}
                  }                  
                  ret_count=pthread_attr_destroy(&pta);
    		  if(ret_count)
    		  {
			printf("\n ERROR : Return code from pthread_attr_destroy() is %d ",ret_count);
			exit(-1);
    		  }
             } while ((Iteration < MAX_ITERATIONS) && (Distance(X_Old, X_New, matrix_size) >= tolerance));
       
        /* Calculating the time at the end of Operation */
        gettimeofday(&tv, &tz);
        time_end= (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;

        printf("\n\t\t The Jacobi Method For AX=B .........DONE"); 
        printf("\n\t\t Total Number Of Iterations   :  %d",Iteration);
        printf("\n\t\t Memory Utilized              :  %lf MB",(memoryused/(1024*1024)));
        printf("\n\t\t Time in  Seconds (T)         :  %lf",(time_end - time_start));
        printf("\n\t\t..........................................................................\n");

        /* Freeing Allocated Memory */
        free(X_New);
        free(X_Old);
        free(Matrix_A);
        free(RHS_Vector);
        free(Bloc_X);
        free(threads);

}

double Distance(double *X_Old, double *X_New, int matrix_size)
{
        int             index;
        double          Sum;

        Sum = 0.0;
        for (index = 0; index < matrix_size; index++)
        Sum += (X_New[index] - X_Old[index]) * (X_New[index] - X_Old[index]);
        return (Sum);
}

void jacobi(int Number)
{
   int i,j;

   for(i = 0; i < Number; i++)
   {
     	Bloc_X[i] = RHS_Vector[i];

     	for (j = 0;j<i;j++) 
     	{
      		Bloc_X[i] -= X_Old[j] * Matrix_A[i][j];
     	}
     	for (j = i+1;j<Number;j++) 
     	{
     		Bloc_X[i] -= X_Old[j] * Matrix_A[i][j];
     	}
     	Bloc_X[i] = Bloc_X[i] / Matrix_A[i][i];
  }
  for(i = 0; i < Number; i++)
  { 
  	X_New[i] = Bloc_X[i];
  }
}  

