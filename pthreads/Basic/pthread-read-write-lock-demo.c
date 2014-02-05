
/***********************************************************************



 Example      : pthread-read-write-lock-demo.c
   
 Objective    : Code to demonstrate read-write lock APIs for Demo Purpose.
  
 Description  : Demo code to illustrate different read-write locks APIs. 

 Input        : None
  
 Output       : Sequence of acquiring and releasing read-write locks.
         

 Created      : Aug 2011
 
               
 Email        : RarchK
                 
************************************************************************/


#define _MULTI_THREADED
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void checkResults(char *string, int rc);            // to check return code from thread function

pthread_rwlock_t       rwlock;

void *rdlockThread(void *arg)				// illustrate readlock APIs
{
  int rc;

  printf("\n\nEntered thread1, getting read lock\n\n");
  rc = pthread_rwlock_rdlock(&rwlock);
  checkResults("pthread_rwlock_rdlock()\n", rc);
  printf("\ngot the rwlock read lock\n");

  sleep(5);

  printf("\nunlock the read lock\n");
  rc = pthread_rwlock_unlock(&rwlock);
  checkResults("pthread_rwlock_unlock()\n", rc);
  return NULL;
}

void *wrlockThread(void *arg)                           // illustrate writelock APIs
{
  int rc;

  printf("\n\nEntered thread2, getting write lock\n\n");
  rc = pthread_rwlock_wrlock(&rwlock);
  checkResults("pthread_rwlock_wrlock()\n", rc);

  printf("\nGot the rwlock write lock, now unlock\n\n");
  rc = pthread_rwlock_unlock(&rwlock);
  checkResults("pthread_rwlock_unlock()\n", rc);
  return NULL;
}

int main(int argc, char **argv)
{
  int                   rc=0;
  pthread_t             thread2, thread1;


  printf("Main, initialize the read write lock\n\n\n");
  rc = pthread_rwlock_init(&rwlock, NULL);				// initialise rwlock
  checkResults("pthread_rwlock_init()\n", rc);

  printf("Main creating thread1\n\n");
  rc = pthread_create(&thread1, NULL, rdlockThread, NULL);              // creating thread1
  checkResults("pthread_create\n", rc);


  printf("Main creating thread2 \n\n\n");				// creating thread2
  rc = pthread_create(&thread2, NULL, wrlockThread, NULL);
  checkResults("pthread_create\n", rc);

  sleep(5);

  printf("Main waiting for the threads\n\n");
  rc = pthread_join(thread1, NULL);
  checkResults("pthread_join\n", rc);

  rc = pthread_join(thread2, NULL);
  checkResults("pthread_join\n", rc);

  rc = pthread_rwlock_destroy(&rwlock);
  checkResults("pthread_rwlock_destroy()\n", rc);

  printf("Main completed\n\n");


  return 0;
}

void checkResults(char *string, int rc)                  // check whether pthread API return some error
{
	if (rc) 
  {
    printf("Error on : %s, rc=%d",string, rc);
    exit(0);
  }
  return;
}
