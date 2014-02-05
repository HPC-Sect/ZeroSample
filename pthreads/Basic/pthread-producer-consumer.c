
/*****************************************************************************

  Example     : pthread-producer-consumer.c
 
  Objective   : To illustrate producer-consumer problem using pthreads.

  Input       : Nothing.

  Output      : Sequence of Produced and Consumed items.
	                                                                                                                    
  Created     : Aug 2011 

  E-mail      : RarchK           
                               
****************************************************************************/

#include <pthread.h> 
#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h> 

/*defining QUEUE sise */
#define QUEUESIZE 10 
#define LOOP 20 

/*global variable declaration */ 
int ret_count;
/*Thread callback function prototype*/
void *producer (void *args); 
void *consumer (void *args); 

typedef struct { 
		
 			int buf[QUEUESIZE]; 
			long head, tail; 
			int full, empty; 
			pthread_mutex_t *mut; 
			pthread_cond_t *notFull, *notEmpty; 
	       } queue; 



queue *queueInit (void); 
void queueDelete (queue *q); 
void queueAdd (queue *q, int in); 
void queueDel (queue *q, int *out); 

/*Main function */
int main () 
{ 
	queue *fifo; 
	pthread_t pro, con;
	/*QUEUE initialization */ 
	fifo = queueInit (); 
	if (fifo == NULL)
        { 
		fprintf (stderr, "main: Queue Init failed.\n"); 
		exit (1); 
	} 
	/*producer thread creation */
	ret_count=pthread_create (&pro, NULL, producer, fifo); 
    	if(ret_count)
    	{
		printf("\n ERROR : Return code from pthread_create() is %d ",ret_count);
		exit(-1);
    	}
	/*consumer thread creation */
	ret_count=pthread_create (&con, NULL, consumer, fifo); 
	if(ret_count)
    	{
		printf("\n ERROR : Return code from pthread_create() is %d ",ret_count);
		exit(-1);
    	}
	/*joining producer thread to main thread */
	ret_count=pthread_join (pro, NULL); 
    	if(ret_count)
    	{
		printf("\n ERROR : Return code from pthread_join() is %d ",ret_count);
		exit(-1);
    	}


	/*joining consumer thread to main thread */
	ret_count=pthread_join (con, NULL); 
	if(ret_count)
    	{
		printf("\n ERROR : Return code from pthread_join() is %d ",ret_count);
		exit(-1);
    	}
	queueDelete (fifo); 
	return 0; 
} 

/*producer thread callback function */
void *producer (void *q) 
{ 
	queue *fifo; 
	int i; 
	fifo = (queue *)q; 
	for (i = 0; i < LOOP; i++) 
	{ 
		pthread_mutex_lock (fifo->mut); 
		while (fifo->full) 
		{ 
			printf ("producer: queue FULL.\n"); 
			pthread_cond_wait (fifo->notFull, fifo->mut); 
		} 
		queueAdd (fifo, i); 
		pthread_mutex_unlock (fifo->mut); 
		pthread_cond_signal (fifo->notEmpty); 
		printf ("producer: add %d \n",i); 
		usleep (100000); 
        }

 return (NULL); 

} 



/*consumer thread callback function */
void *consumer (void *q) 
{ 
	queue *fifo; 
	int i, d; 

 
	fifo = (queue *)q; 
	for (i = 0; i < LOOP; i++) 
	{ 
		pthread_mutex_lock (fifo->mut); 
		while (fifo->empty) 
		{ 
			printf ("consumer: queue EMPTY.\n"); 
			pthread_cond_wait (fifo->notEmpty, fifo->mut); 
		} 
		queueDel (fifo, &d); 
		pthread_mutex_unlock (fifo->mut); 
		pthread_cond_signal (fifo->notFull); 
		printf ("consumer: received %d.\n", d); 
		usleep(500000); 
	} 
	 
	return (NULL); 

} 

#ifdef o 
typedef struct 
{ 
	int buf[QUEUESIZE]; 
	long head, tail; 
	int full, empty; 
	pthread_mutex_t *mut; 
	pthread_cond_t *notFull, *notEmpty; 
} queue; 
#endif 

 
queue *queueInit (void) 
{ 
	queue *q; 
	q = (queue *)malloc (sizeof (queue)); 
	if (q == NULL) return (NULL); 
	q->empty = 1; 
	q->full = 0; 
	q->head = 0; 
	q->tail = 0; 
	q->mut = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t)); 
	ret_count=pthread_mutex_init (q->mut, NULL); 
    	if(ret_count)
    	{
		printf("\n ERROR : Return code from pthread_mutex_init() is %d ",ret_count);
		exit(-1);
    	}

	q->notFull = (pthread_cond_t *) malloc (sizeof (pthread_cond_t)); 
	pthread_cond_init (q->notFull, NULL); 
	q->notEmpty = (pthread_cond_t *) malloc (sizeof (pthread_cond_t)); 
	pthread_cond_init (q->notEmpty, NULL); 
	return (q); 
} 


void queueDelete (queue *q) 
{ 
	ret_count=pthread_mutex_destroy (q->mut); 
    	if(ret_count)
    	{
		printf("\n ERROR : Return code from pthread_mutex_destroy() is %d ",ret_count);
		exit(-1);
    	}
	free (q->mut); 
	pthread_cond_destroy (q->notFull); 
	free (q->notFull); 
	pthread_cond_destroy (q->notEmpty); 
	free (q->notEmpty); 
	free (q); 
} 


void queueAdd (queue *q, int in) 
{ 
	q->buf[q->tail] = in; 
	q->tail++; 
	if (q->tail == QUEUESIZE) 
	q->tail = 0; 
	if (q->tail == q->head) 
		q->full = 1; 
	q->empty = 0; 
	return; 
} 

void queueDel (queue *q, int *out) 
{ 

 
	*out = q->buf[q->head]; 
	q->head++; 
	if (q->head == QUEUESIZE) 
	q->head = 0; 
	if (q->head == q->tail) 
	q->empty = 1; 
	q->full = 0; 
	return; 
} 

