/*
 * Date	  :Aug 2011
 * File	  : prod_consumer_mutex_multi_access.c
 * Desc   : Producer-Consumer problem with large no. of threads and large no. of resources
 * 	    (as many as user specifies) using pthread_mutex
 * Input  : num_threads, queue_size & ThreadAffMask (all 3, first 2 or first 1 only are
 * 	    also accepted) 
 * Output : Thread-affinity set (if specified) , time taken to execute in sec & microsec.
 * E-mail : RarchK   
 */
#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include <errno.h>
#include <sys/time.h>//to record the start and end time
#include <sched.h>//to set thread-affinity

#define maxsize 1000
//node of linked list
struct node
{
  int data;
  struct node *next;
};
//defination of the queue datastructure
typedef struct
{
  int contains;  // no. of elements currently in the queue
  struct node *front; // the front ptr
  struct node *rear;  // the rear ptr
  pthread_mutex_t *mutex; //mutex of queue
} queue;

queue *q;//queue object

//queue operations
//***************************************************
//queue initialization
void
queue_init ()
{
  q = (queue *) malloc (sizeof (queue));
  q->front = (struct node *) malloc (sizeof (struct node));
  q->front->next = NULL;
  q->rear = q->front;
  q->mutex = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
  q->contains = 0;
  return;
}

//enqueue operation
void
enqueue (double val)
{
  struct node *tmp = (struct node *) malloc (sizeof (struct node));
  tmp->data = val;
  tmp->next = NULL;
  q->rear->next = tmp;
  q->rear = tmp;
  q->contains++;
  return;
}

//dequeue operation
double
dequeue ()
{
  struct node *tmp = q->front;
  q->front = q->front->next;
  double retval = tmp->data;
  q->contains--;
  free (tmp);
  return retval;
}

//****************************************

int num_iter = 10000;  //default no. of iterations
int num_threads = 64;  //default no. of threads
int queue_size=maxsize;//default queue size
//the producer thread fn
void *
producer (void *arg)
{
  int inserted=0;
  int loopcnt = num_iter;
  int *id = (int *) arg;
  while (loopcnt)
    {
      inserted=0;
     while(inserted==0)
     {
      pthread_mutex_lock (q->mutex); //to obtain mutex on the queue
      if (q->contains < queue_size-1)//if the queue is not full
	{
	  enqueue (*id);
          inserted=1;	
	  loopcnt--;
	}
      pthread_mutex_unlock (q->mutex);//to release the mutex on the queue
     }
     printf("Producer : %d inserted.\n",*id);
     
    }


}

//the consumer thread fn
void *
consumer (void *arg)
{
  int extracted=0;
  int loopcnt = num_iter;
  int *id = (int *) arg;
  while (loopcnt)
    {
      extracted=0;
     while(extracted==0)
     {
      pthread_mutex_lock (q->mutex);//to obtain mutex on the queue
      if (q->contains > 0)
	{
	  dequeue ();
          extracted=1;
	  loopcnt--;
	}
      pthread_mutex_unlock (q->mutex);//to release the mutex on the queue
    }
      printf("Consumer : %d extracted.\n",*id);
  }

}


int
main (int argc, char *argv[])
{
  int i;
  double timetaken;
  struct timeval tv_start,tv_end; //for gettimeofday()
  struct timezone tz_start,tz_end;//for gettimeofday()
  
  unsigned long ThreadAffMask;//to obtain Thread Affinity (no. of processors to which thread is bound)
  unsigned int ThreadAffMaskLen = sizeof(ThreadAffMask);//length of AffinityMask
  pid_t ProgPid = getpid();//PID of the program for thread affinity 
  queue_init ();
  
  if (argc == 2) //user provided num_threads
    {
      num_threads = atoi (argv[1]);
    }
  else if (argc == 3) //user provided num_threads and  queue_size
    {
      num_threads = atoi (argv[1]);
      queue_size= atoi (argv[2]);
    }
  else if(argc == 4)//user provided num_threads, queue_size & ThreadAffMask (i.e., no. of cores to bind)
    {
	num_threads = atoi (argv[1]);
        queue_size= atoi (argv[2]);
	ThreadAffMask = atoi(argv[3]);
	if(sched_setaffinity(ProgPid, ThreadAffMaskLen, (cpu_set_t *)&ThreadAffMask)==-1)
	 perror("ERROR: Couldn't set thread affinity mask\n");
    }
    else
    {
	printf("usage: %s <num_threads> <queuesize> <ThreadAffMask>\n"
		"Assuming num_threads = 64 , queue_size = 1000 & default thread-affinity\n",argv[0]);
    }
  pthread_t prod_tids[num_threads], cons_tids[num_threads]; //declaration of tids of the producer and consumer threads resp.
  pthread_mutex_init (q->mutex, NULL);//to the initialize the queue mutex
  int tid_id[num_threads];//ids to be passed to the thread fns
  
  //**************************************************************************
  gettimeofday(&tv_start, &tz_start); //to record the start time
  //**************************************************************************
  //loop to create producer threads
  for (i = 0; i < num_threads; i++)
    {
      tid_id[i]=i;
      if (pthread_create (&prod_tids[i], NULL, producer, (void *) &tid_id[i]))
	{
	  perror ("pthread_create error\n");
	}
    }
  //loop to create consumer threads
  for (i = 0; i < num_threads; i++)
    {
      if (pthread_create (&cons_tids[i], NULL, consumer, (void *) &tid_id[i]))
	{
	  perror ("pthread_create error\n");
	}
    }
  //thread join only then main thread continues
  for (i = 0; i < num_threads; i++)
    {
      pthread_join (prod_tids[i], NULL);
      pthread_join (cons_tids[i], NULL);
    }
   //**************************************************************************   
   gettimeofday(&tv_end, &tz_end); //to record the end time
   //**************************************************************************  
   timetaken = tv_end.tv_sec*1000000 + tv_end.tv_usec - (tv_start.tv_sec*1000000 + tv_start.tv_usec); //to calculate the timetaken in microseconds
 
   if(argc == 4)
   {
       sched_getaffinity(ProgPid, ThreadAffMaskLen, (cpu_set_t *)&ThreadAffMask); //to get the affinity with which executed
       printf("pid %d's executed with affinity: %08lx\n\n", ProgPid, ThreadAffMask);
   }
      
    printf("Execution time (micro sec) : %d",timetaken);
    printf("Execution time (sec)       : %d",timetaken/1000000);
	
  return 0;
}
