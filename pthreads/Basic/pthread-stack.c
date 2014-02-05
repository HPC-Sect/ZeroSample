      
/*****************************************************************************

 Example               : pthread-stack.c


 Objective 	       : To illustrate basic stack management.

 
 Input                 :  None.


 Output                : Default Stack Size
			 Amount of stack needed per thread.
			 Sequence of thread creation with stack size.	                                            
                                                                        
 Created               : Aug 2011

 E-mail                : RarchK                                          

******************************************************************************/


#include <pthread.h>
#include <stdio.h>
#include<stdlib.h>
      
#define NTHREADS 4
#define N 1000
#define MEGEXTRA 1000000
       
pthread_attr_t attr;
       
void *dowork(void *threadid)
{
         double A[N][N];
         int i,j;
         size_t mystacksize;
       
         pthread_attr_getstacksize (&attr, &mystacksize);
         printf("Thread %d: stack size = %li bytes \n", threadid, mystacksize);
         for (i=0; i<N; i++)
           for (j=0; j<N; j++)
            A[i][j] = ((i*j)/3.452) + (N-i);
         pthread_exit(NULL);
}
       
int main(int argc, char *argv[])
{
         pthread_t threads[NTHREADS];
         size_t stacksize;
         int ret_count, t;
         printf("\n\t\t---------------------------------------------------------------------------");
	 printf("\n\t\t Centre for Development of Advanced Computing (C-DAC)");
  	 printf("\n\t\t Email : RarchK");
  	 printf("\n\t\t---------------------------------------------------------------------------");
  	 printf("\n\t\t Objective : To illustrate stack management using pthreads\n ");
  	 printf("\n\t\t..........................................................................\n");

        ret_count=pthread_attr_init(&attr);
      	if (ret_count)
      	{
         	printf("ERROR; return code from pthread_attr_init() is %d\n", ret_count);
         	exit(-1);
      	}
        ret_count= pthread_attr_getstacksize (&attr, &stacksize);
      	if (ret_count)
     	{
         	printf("ERROR; return code from pthread_getstacksize() is %d\n", ret_count);
         	exit(-1);
      	}
         printf("Default stack size = %li\n", stacksize);
         stacksize = sizeof(double)*N*N+MEGEXTRA;
         printf("Amount of stack needed per thread = %li\n",stacksize);

        ret_count= pthread_attr_setstacksize (&attr, stacksize);
      	if (ret_count)
      	{
         	printf("ERROR; return code from pthread_setstacksize() is %d\n", ret_count);
         	exit(-1);
      	}
         printf("Creating threads with stack size = %li bytes\n",stacksize);
         for(t=0; t<NTHREADS; t++)
         {
            ret_count = pthread_create(&threads[t], &attr, dowork, (void *)t);
            if (ret_count)
            {
               printf("ERROR; return code from pthread_create() is %d\n", ret_count);
               exit(-1);
            }
         }
         printf("Created %d threads.\n", t);
         pthread_exit(NULL);
}

