
/* ***************************************************************************

 Example               : pthread-conditionvariable.c


 Objective 	       : To illustrate pthread condition variables.

 
 Input                 : None

 Output                : Display value of global variable with respect to mutex state.	                                            
                                                                        

 Created               : Aug 2011 
       

 E-mail                : RarchK                                          


*********************************************************************************/



#include <pthread.h>
#include <stdio.h>

#define NUM_THREADS  3
#define TCOUNT 10
#define COUNT_LIMIT 12

int     count = 0;
int     thread_ids[3] = {0,1,2};
pthread_mutex_t count_mutex;
pthread_cond_t count_threshold_cv;

void *inc_count(void *idp) 
{
  int j,i;
  double result=0.0;
  int *my_id = idp;

  for (i=0; i<TCOUNT; i++) {
    pthread_mutex_lock(&count_mutex);
    count++;

    /* 
    Check the value of count and signal waiting thread when condition is
    reached.  Note that this occurs while mutex is locked. 
    */
    if (count == COUNT_LIMIT) {
      pthread_cond_signal(&count_threshold_cv);
      printf("inc_count(): thread %d, count = %d  Threshold reached.\n", 
             *my_id, count);
      }
    printf("inc_count(): thread %d, count = %d, unlocking mutex\n", 
	   *my_id, count);
    pthread_mutex_unlock(&count_mutex);

    /* Do some work so threads can alternate on mutex lock */
    for (j=0; j<1000; j++)
      result = result + (double)random();
    }
  pthread_exit(NULL);
}

void *watch_count(void *idp) 
{
  int *my_id = idp;

  printf("Starting watch_count(): thread %d\n", *my_id);

  /*
  Lock mutex and wait for signal.  Note that the pthread_cond_wait 
  routine will automatically and atomically unlock mutex while it waits. 
  Also, note that if COUNT_LIMIT is reached before this routine is run by
  the waiting thread, the loop will be skipped to prevent pthread_cond_wait
  from never returning. 
  */
  pthread_mutex_lock(&count_mutex);
  if (count<COUNT_LIMIT) {
    pthread_cond_wait(&count_threshold_cv, &count_mutex);
    printf("watch_count(): thread %d Condition signal received.\n", *my_id);
    }
  pthread_mutex_unlock(&count_mutex);
  pthread_exit(NULL);
}

int main (int argc, char *argv[])
{
  int i, ret_count;
  pthread_t threads[3];
  pthread_attr_t attr;

  /* Initialize mutex and condition variable objects */
  ret_count=pthread_mutex_init(&count_mutex, NULL);
  if (ret_count)
  {
         printf("ERROR; return code from pthread_mutex_init() is %d\n", ret_count);
         exit(-1);
  }
  pthread_cond_init (&count_threshold_cv, NULL);

  /* For portability, explicitly create threads in a joinable state */
  ret_count=pthread_attr_init(&attr);
  if (ret_count)
  {
       printf("ERROR; return code from pthread_attr_init() is %d\n", ret_count);
       exit(-1);
  }
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  ret_count=pthread_create(&threads[0], &attr, inc_count, (void *)&thread_ids[0]);
  if (ret_count)
  {
         printf("ERROR; return code from pthread_create() is %d\n", ret_count);
         exit(-1);
  }
  ret_count=pthread_create(&threads[1], &attr, inc_count, (void *)&thread_ids[1]);
  if (ret_count)
  {
         printf("ERROR; return code from pthread_create() is %d\n", ret_count);
         exit(-1);
  }
  ret_count=pthread_create(&threads[2], &attr, watch_count, (void *)&thread_ids[2]);
  if (ret_count)
  {
         printf("ERROR; return code from pthread_create() is %d\n", ret_count);
         exit(-1);
  }

  /* Wait for all threads to complete */
  for (i=0; i<NUM_THREADS; i++) 
  {
        ret_count=pthread_join(threads[i], NULL);
        if (ret_count)
        {
            printf("ERROR; return code from pthread_join() is %d\n", ret_count);
            exit(-1);
        }
  }
  printf ("Main(): Waited on %d  threads. Done.\n", NUM_THREADS);

  /* Clean up and exit */
  ret_count=pthread_attr_destroy(&attr);
  if (ret_count)
  {
         printf("ERROR; return code from pthread_attr_destroy() is %d\n", ret_count);
         exit(-1);
  }
  ret_count=pthread_mutex_destroy(&count_mutex);
  if (ret_count)
  {
         printf("ERROR; return code from pthread_mutex_destroy() is %d\n", ret_count);
         exit(-1);
  }
  pthread_cond_destroy(&count_threshold_cv);
  pthread_exit(NULL);

}
