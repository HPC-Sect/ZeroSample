/***********************************************************************************

  Example     : pthread-demo-datarace.c
 
  Objective   : Write Pthread code to illustrate Data Race Condition
  	        and its solution using MUTEX.

  Input       : Nothing.

  Output      : Value of Global variable with and without using Mutex.
	                                                                                                                    
  Created     :Aug 2011  
  
  E-mail      : RarchK         
                                 
****************************************************************************/

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int myglobal;                                        // declaration of global variable
pthread_mutex_t mymutex=PTHREAD_MUTEX_INITIALIZER;  // initialization of MUTEX variable


void *thread_function_datarace(void *arg)       // Function which operates on myglobal without using mutex
{
  	int i,j;                                        
  	for ( i=0; i<20; i++ ) 
	{
    		j=myglobal;
    		j=j+1;                                         
    		printf("\nIn thread_function_datarace..\t");    // Incrementing j and assign myglobal value of j
//    		fflush(stdout);                               
   		sleep(1);
    		myglobal=j;
  	}
  	return NULL;
}

void *thread_function_mutex(void *arg)   // Function which operates on myglobal using mutex
{
  	int i,j;
  	for ( i=0; i<20; i++ ) 
	{
    		pthread_mutex_lock(&mymutex);
		j=myglobal;
    		j=j+1;
    		printf("\nIn thread_function_mutex..\t");
  //  		fflush(stdout);
   		sleep(0.1);
    		myglobal=j;
		pthread_mutex_unlock(&mymutex);
  	}
  	return NULL;
}

int main(void) 
{

  	pthread_t mythread;
  	int i;

  	if ( pthread_create( &mythread, NULL, thread_function_datarace, NULL) )   // calling thread_function_datarace
	{
   	 	printf("error creating thread.");
    		abort();
  	}

	printf("\n\t\t---------------------------------------------------------------------------");
        printf("\n\t\t Centre for Development of Advanced Computing (C-DAC)");
        printf("\n\t\t Email : RarchK");
        printf("\n\t\t---------------------------------------------------------------------------");
        printf("\n\t\t Objective : Pthread code to illustrate data race condition and its solution \n ");
        printf("\n\t\t..........................................................................\n");



  	for ( i=0; i<20; i++) 
	{
    		myglobal=myglobal+1;
    		printf("\nIn main..\t");
    //		fflush(stdout);
    		sleep(1);
  	}

  	if ( pthread_join ( mythread, NULL ) ) 
	{
    		printf("error joining thread.");
    		abort();
  	}

  	printf("\nValue of myglobal in thread_function_datarace is :  %d\n",myglobal);
	
	printf("\n ----------------------------------------------------------------------------------------------------\n");	

	myglobal = 0;

	if ( pthread_create( &mythread, NULL, thread_function_mutex, NULL) ) // calling thread_function_mutex
        {
                printf("error creating thread.");
                abort();
        }

        for ( i=0; i<20; i++)
        {
                pthread_mutex_lock(&mymutex);
    		myglobal=myglobal+1;
    		pthread_mutex_unlock(&mymutex);
    		printf("\nIn main..\t");
    //		fflush(stdout);
    		sleep(1);
        }

        if ( pthread_join ( mythread, NULL ) )
        {
                printf("error joining thread.");
                abort();
        }
	
	printf("\n");
        printf("\nValue of myglobal in thread_function_mutex is :  %d\n",myglobal);


  	exit(0);

}

