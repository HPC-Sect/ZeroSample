/********************************************************************

 Example                : pthread-findmin.c

 Objective  		: To find the min val of a large array in a parallel manner.
                    	  Demonstrates use of:
                    		pthread_create()
                    		pthread_join()
          	    		pthread_mutex_init()
		    		pthread_mutex_destroy()
                               	pthread_mutex_lock()  
                               	pthread_mutex_unlock()  
 
 Input                 : None.

 Output                : Minimum value of the array and the timetaken	                                            
                                                                        
 Created               : Aug 2011 
 
 E-mail                : RarchK                                          

*************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<math.h>
#include<limits.h>
#include<sys/time.h>
#define MAXTHREADS 8
#define ARRAYSIZE 32*32

int *partial_list_ptr; //defines to start of each threads part of the array
 int partial_list_size;	//the size of partial array used by each thread
int a[ARRAYSIZE];
pthread_mutex_t min_val_lock;
int min_val = INT_MAX;//global minimum val
int num_threads;


void *
find_min (void *list_ptr)
{
  int start, end, my_min, i, j;	
  my_min = INT_MAX;
  partial_list_ptr = (int *) list_ptr;
  start = (*partial_list_ptr * partial_list_size);

  end = start + partial_list_size - 1;
 
  printf ("\nthread%d doing  iterations %d to %d  \n", *partial_list_ptr,
	  start, end);
 
//computing the min val of the partial array
  for (i = start; i < end; i++)
    if (a[i] < my_min)		//my_min
      my_min = a[i];


  printf ("(Thread%d  says....) my_min is %d\n", *partial_list_ptr, my_min);
 //obtaining lock to update the global min 
 pthread_mutex_lock (&min_val_lock);
  //updating global min.
  if (my_min < min_val)
    min_val = my_min;
 
  printf ("Thread%d says:global minimum after updation is..%d\n", *partial_list_ptr, min_val);
  //releasing the mutex	
  pthread_mutex_unlock (&min_val_lock);

  pthread_exit (NULL);
}



int
main (int argc, char *argv[])
{
  pthread_t *threads;
  int i, start, ret_count;
  int *tids;
  struct timeval tv_start, tv_end;	//timeval - one for the start and other for the end
  struct timezone tz_start, tz_end;	//timezone - one for start and other for the end
  double time;

  if (argc != 2)
    printf ("Syntax:./a.out <num of threads>\n");

  num_threads = atoi (argv[1]);
 
   if (num_threads > MAXTHREADS)
     {
      printf ("num threads shouldn't exceed 8\n");
      printf ("Sorry..... Aborting.........\n");
      return;
    }
  printf ("\n\nFinding the minimum value in a list.......\n\n");
  /* initialization of array ................................ */

  for (i = 0; i < ARRAYSIZE; i++)
    {
      a[i] = i + 10;		//offset 10
    }
 
  partial_list_size = ARRAYSIZE / num_threads;
  
  threads = (pthread_t *) malloc (sizeof (pthread_t) * num_threads);
  tids = (int *) malloc (sizeof (int) * num_threads);

  /*Mutex Initialization ......................................... */
  if (ret_count = pthread_mutex_init (&min_val_lock, NULL))
    {
      printf ("Error:return code from pthread_mutex_init() is\n %d",
	      ret_count);
      exit (-1);
    }

 //start time
  gettimeofday (&tv_start, &tz_start);

  /* creation of threads ....................................... */
  for (i = 0; i < num_threads; i++)
    {
      tids[i] = i;
      if (ret_count = pthread_create (&threads[i], NULL, find_min, &tids[i]))
	{
	  printf ("Error:return code form pthread_create()  is %d\n",
		  ret_count);
	  exit (-1);
	}
    }

  /*.............................................................. */
  for (i = 0; i < num_threads; i++)
    {
      if (ret_count = pthread_join (threads[i], NULL))

	{
	  printf ("Error:pthread_join error,return code is %d\n", ret_count);
	  exit (-1);
	}
    }

 if (ret_count = pthread_mutex_destroy (&min_val_lock))

    {
      printf ("Error:return code is %d \n", ret_count);
      exit (-1);
    }

  printf ("\nMinimum value in the list is %d\n \n", min_val);


  //end time
  gettimeofday (&tv_end, &tz_end);
  //time taken
  time =   tv_end.tv_sec * 1000000 + tv_end.tv_usec - (tv_start.tv_sec * 1000000 +
						tv_start.tv_usec);
  time /= 1000000;
  printf ("Total time required to complete the task is %g seconds\n", time);
}

