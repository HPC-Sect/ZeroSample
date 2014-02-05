/***************************************************************************

 Example               : pthread-join.c


 Objective             : To illustrate thread joining operation.

 Input                 : None.

 Output                : Displays Sequence of thread creation and joining.
	                                            
                                                                        
 Created               :Aug 2011
 
       
 E-mail                : RarchK                                          


*******************************************************************************/




#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 4

void *BusyWork(void *null)
{
   int i;
   double result=0.0;
   for (i=0; i<1000000; i++)
   {
     result = result + (double)random();
   }
   printf("result = %e\n",result);
   pthread_exit((void *) 0);
}

int main (int argc, char *argv[])
{
   pthread_t thread[NUM_THREADS];
   pthread_attr_t attr;
   int ret_count, t, status;



  printf("\n\t\t---------------------------------------------------------------------------");
  printf("\n\t\t Centre for Development of Advanced Computing (C-DAC)");
  printf("\n\t\t Email : RarchK");
  printf("\n\t\t---------------------------------------------------------------------------");
  printf("\n\t\t Objective : To illustrate thread join operation\n ");
  printf("\n\t\t..........................................................................\n");


   /* Initialize and set thread detached attribute */
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

   for(t=0; t<NUM_THREADS; t++)
   {
      printf("Creating thread %d\n", t);
      ret_count = pthread_create(&thread[t], &attr, BusyWork, NULL); 
      if (ret_count)
      {
         printf("ERROR; return code from pthread_create() is %d\n", ret_count);
         exit(-1);
      }
   }

   /* Free attribute and wait for the other threads */
   ret_count=pthread_attr_destroy(&attr);
   if (ret_count)
   {
   	printf("ERROR; return code from pthread_attr_destroy() is %d\n", ret_count);
        exit(-1);
   }
   for(t=0; t<NUM_THREADS; t++)
   {
      pthread_join(thread[t], (void **)&status);
      printf("Completed join with thread %d status= %d\n",t, status);
   }

   pthread_exit(NULL);
}
