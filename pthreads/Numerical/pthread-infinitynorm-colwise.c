/*************************************************************************************
                         

 Example 	: pthread-infinitynorm-colwise.c

 Objective  	: Inifinity Norm of Matrix using Column Wise splitting 

 Input 		: class size,
                  Number of Threads
            
 Output         : Infinity Norm of Matrix (Using Column-Wise splitting) 

 Created        : Aug 2011

 E-mail      	: RarchK	

***************************************************************************************/

#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include<stdlib.h>
#include<sys/time.h>

/* pthread mutex and condition variable declaration */

pthread_mutex_t  mutex_norm = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t  CurRow_norm = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_Row = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t count_threshold_cv = PTHREAD_COND_INITIALIZER;

pthread_mutex_t  *mutex_Res;
pthread_mutex_t  mutex_col;

/*global variable declaration */
double   NormCol = 0;
int      dist_col;
int      iteration;

int      rmajor,cmajor;
int      perfect_square;
int      row, col;
double   **InMat,*Res;
int      numberOfThreads;
double   infinitynorm_col;

/* Thread callback function */
void *doColWise(int myId)
{
    int iRow;
    int CurCol = 0;       /*Which Column to operate on.*/

    for (CurCol = ((myId - 1) * dist_col); CurCol <= ((myId * dist_col) - 1); CurCol++)
      for(iRow = 0 ;iRow < row; iRow++)
        Res[iRow] += InMat[iRow][CurCol];

    pthread_exit(NULL);
}

/* Main function starts*/
main(int argc, char *argv[])
{
/* variable declaration */
  int            i, j,p,q,CLASS_SIZE,THREADS,ret_count;
  int            first_value_row,diff,matrix_size;
  double         time_start, time_end,memoryused=0.0;
  struct         timeval tv;
  struct         timezone tz;
  int            counter, irow, icol, numberOfThreads,ithread;  
  FILE           *fp;
  char	         * CLASS;

  pthread_t      *threads_col;
  pthread_attr_t ptc;

  printf("\n\t\t---------------------------------------------------------------------------");
  printf("\n\t\t Centre for Development of Advanced Computing (C-DAC)");
  printf("\n\t\t Email : RarchK");
  printf("\n\t\t---------------------------------------------------------------------------");
  printf("\n\t\t Objective : Dense Matrix Computations (Floating-Point Operations)\n ");
  printf("\n\t\t Computation of Infinity Norm of a Square Matrix - Columnwise partitioning;");
  printf("\n\t\t..........................................................................\n");
  /*Argument validation check*/
  if( argc != 3 ){
       printf("\t\t Very Few Arguments\n ");
       printf("\t\t Syntax : exec <Class-Size(Give A/B/C_> <Threads>\n");
       exit(-1);
  }
  else {
       CLASS = argv[1];
       THREADS = atoi(argv[2]);
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


  numberOfThreads = THREADS;
   matrix_size = CLASS_SIZE;
   row = matrix_size;
   col = matrix_size;

   printf("\n\t\t Input Parameters :");
   printf("\n\t\t CLASS           :  %c",*CLASS);
   printf("\n\t\t Matrix Size     :  %d",matrix_size);
   printf("\n\t\t Threads         :  %d",numberOfThreads);
   printf("\n");

  if ((numberOfThreads != 1) && (numberOfThreads != 2) && (numberOfThreads != 4) && (numberOfThreads != 8))
   {
       printf("\n Number of Threads must be 1 or 2 or 4 or 8. Aborting ...\n");
      return; 
   }

  if(numberOfThreads > row)
   {
       printf("\nNumber of threads should be <= %d",row);
       return;
   }

  /*....Memory Allocation....*/
  
  InMat = (double **) malloc(sizeof(double) * row);
  for (i = 0; i < row; i++)
  InMat[i] = (double *) malloc(sizeof(double) * col);
  if(InMat == NULL)
  {
   	printf("\n Not sufficient memory to accomadate InMat");
	return;   
  } 
  memoryused = (row * col * sizeof(double));
 
  Res = (double *) malloc(sizeof(double) * row);
  if(Res == NULL)
  {
   	printf("\n Not sufficient memory to accomadate Res");
   	return;
  }
  memoryused += (row * sizeof(double));
  

  /* Matrix  and Vector  Initialization */
   for (i=0; i<row;i++) {
   	for (j = 0; j<col;j++) {
   		InMat[i][j] = (double)(i + j);
   		Res[i] = 0.0;
   	}
  }


  /* Column Wise Partitioning */

  dist_col = col / numberOfThreads;

  ret_count=pthread_attr_init(&ptc);
  if(ret_count)
  {
	printf("\n ERROR : Return code from pthread_attr_init() is %d ",ret_count);
	exit(-1);
  }
  mutex_Res = ( pthread_mutex_t * ) malloc(sizeof(pthread_mutex_t) * row);
  threads_col = ( pthread_t * ) malloc(sizeof(pthread_t) * numberOfThreads);

/*Taking start time*/
  gettimeofday(&tv, &tz);
  time_start = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;

 /*Thread creation */
  for (ithread = 0; ithread < numberOfThreads; ithread++)
  {
 	ret_count=pthread_create(&threads_col[ithread], &ptc, (void*(*)(void*)) doColWise, (void*) (ithread + 1));
	if(ret_count)
    	{
		printf("\n ERROR : Return code from pthread_create() is %d ",ret_count);
		exit(-1);
    	}
 }
/*Joining threads */
  for (counter=0; counter<numberOfThreads; counter++)
  {
  	ret_count=pthread_join(threads_col[counter], NULL);
    	if(ret_count)
    	{
		printf("\n ERROR : Return code from pthread_join() is %d ",ret_count);
		exit(-1);
    	}
  }
  for (irow = 0 ; irow < row ; irow++)
  if (Res[irow] > NormCol)
    NormCol = Res[irow];

  printf("\n\t\t Col Wise partitioning - Infinity Norm of a Square Matrix.....Done ");
  ret_count=pthread_attr_destroy(&ptc);
  if(ret_count)
  {
	printf("\n ERROR : Return code from pthread_attr_destroy() is %d ",ret_count);
	exit(-1);
  }

   free(InMat);
   free(threads_col);
   free(Res);
   free(mutex_Res);
   gettimeofday(&tv, &tz);
   time_end = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
   printf("\n");
   printf("\n\t\t Memory Utilized       :  %lf MB",(memoryused/(1024*1024)));
   printf("\n\t\t Time in  Seconds (T)  :  %lf",(time_end - time_start));
   printf("\n\t\t..........................................................................\n");

}



