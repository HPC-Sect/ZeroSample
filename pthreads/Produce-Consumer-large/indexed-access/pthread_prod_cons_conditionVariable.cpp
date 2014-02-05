/*
 *  	
 * Date	  : Aug 2011
 * File	  : pthread_prod_cons_conditionVariable.cpp
 * Desc   : Producer-Consumer problem with large no. of threads and large no. of resources
 * 			(as many as user specifies)
 * Input  : NumThreads, NumResources & ThreadAffMask (all 3, first 2 or first 1 only are
 * 			also accepted) 
 * Output : Thread-affinity set (if specified) , time taken to execute in sec & microsec.
 * E-mail : RarchK      
 */

#include <iostream>
#include<stdio.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sched.h>//for setting thread-affinity

using namespace std;

#define NUM_LOOP 10000 //Number of loops for each thread.
int *ResourceQueue;//queue for the resources produced.
int QueueIndex;
double ResourceCount;
int NumThreads;//equal for producer/consumer
int NumResource;//maximum num., of resource on shared memory.

pthread_cond_t QueueFullCond, QueueEmptyCond;//condition variable for Queue full/empty.
pthread_mutex_t ConditionMutex;//mutex object for wait/signal the condition variable.

void *producer(void *data);
void *consumer(void *data);

int main(int argc, char *argv[])
{
	int temp;
	int rc; //return code
	
	double timetaken;//to calculate time taken
	struct timeval tv_start, tv_end;//for gettimeofday()
	struct timezone tz_start, tz_end;//for gettimeofday()
	
	unsigned long ThreadAffMask;//to obtain Thread Affinity (no. of processors to which thread is bound)
        unsigned int ThreadAffMaskLen = sizeof(ThreadAffMask);//length of AffinityMask
        pid_t ProgPid = getpid();//PID of the program for thread affinity 

	ResourceCount = 0;//initially no resources in the queue.
	QueueIndex = 0;
	if(argc == 2)//user provided NumThreads
	{
		NumThreads = atoi(argv[1]);
		cout << "Number of resources not provided.\n"
				"Assuming 1000 resources in the shared queue."
				"\n";
		NumResource = 1000;
	}
	else if(argc == 3)//user provided NumThreads & NumResource
	{
		NumThreads = atoi(argv[1]);
		NumResource = atoi(argv[2]);
	}
	else if(argc == 4)//user provided NumThreads, NumResources & ThreadAffMask (i.e., no. of cores to bind)
	{
		NumThreads = atoi(argv[1]);
		NumResource = atoi(argv[2]);
		ThreadAffMask = atoi(argv[3]);
		rc = sched_setaffinity(ProgPid, ThreadAffMaskLen, (cpu_set_t *)&ThreadAffMask);
		if(rc==-1)
			cout << "ERROR: Couldn't set thread affinity mask\n";
	}
	else
	{
		cout << "Correct usage: " << argv[0] << " <NumThreads> <NumResource> <ThreadAffMask>\n"
				"Assuming NumThreads = 64 , NumResource = 1000 & default thread-affinity\n";
		NumThreads = 64;
		NumResource = 1000;		
	}
	
	//initialize Resource Queue based on NumResource
	ResourceQueue = (int *)malloc(sizeof(int)*NumResource);
	
	pthread_t  tid[NumThreads*2];
	
	//initialize condition & mutex objects
	pthread_mutex_init(&ConditionMutex, NULL);
	pthread_cond_init(&QueueFullCond, NULL);
	pthread_cond_init(&QueueEmptyCond, NULL);
	
	//Start time
	gettimeofday(&tv_start, &tz_start);
	
	long *ThreadId[NumThreads*2];//ID allocated to each thread
	
	//create threads - producer
	for(int i=0; i<NumThreads; i++)
	{
		ThreadId[i] = new long;
		*ThreadId[i] = i;
		rc = pthread_create(&tid[i], NULL, producer, (void *)ThreadId[i]);
		if(rc==-1)
			perror("pthread_create() ERROR\n");
	}
	//create threads - consumer
	for(int i=NumThreads; i<NumThreads*2; i++)
	{
		temp = i%NumThreads;
		ThreadId[i] = new long;
		*ThreadId[i] = temp;
		rc = pthread_create(&tid[i], NULL, consumer, (void *)ThreadId[i]);
		if (rc==-1)
			perror("pthread_create() ERROR\n");
	}
	//Thread Join operation
	for(int i=0; i<NumThreads*2; i+=2)
	{
		pthread_join(tid[i], NULL);
		pthread_join(tid[i+1], NULL);
	}
	
	//End time
	gettimeofday(&tv_end, &tz_end);
	//time taken for the operation
	if(argc == 4)
	{
		rc = sched_getaffinity(ProgPid, ThreadAffMaskLen, (cpu_set_t *)&ThreadAffMask);
		printf("pid %d's executed with affinity: %08lx\n\n", ProgPid, ThreadAffMask);
	}
	timetaken = tv_end.tv_sec*1000000 + tv_end.tv_usec - (tv_start.tv_sec*1000000 + tv_start.tv_usec);
	cout << "Execution time (micro sec) : " << timetaken << endl;
	cout << "Execution time (sec)       : " << timetaken/1000000  << endl;
	
	return 0;
	
}

void *producer(void *data)
{
	double loopCnt = NUM_LOOP;
	int *id = (int *)data;
	int seed = *id;
	int random;
	while(loopCnt)
	{
		
		pthread_mutex_lock(&ConditionMutex);//obtain mutex to the Queue
		while(ResourceCount == NumResource-1)//if Queue is full
			pthread_cond_wait(&QueueEmptyCond, &ConditionMutex);
		
		ResourceQueue[QueueIndex++] = 100;//resource produced by producer
		ResourceCount++;//increment no. of resources available at Queue
		loopCnt--;
		
		pthread_cond_signal(&QueueFullCond);
		pthread_mutex_unlock(&ConditionMutex);//unlock mutex to Queue
		
		cout << "Producer : "<< *id << " inserted.\n";
		//random = rand_r((unsigned int*)&seed) % 3 ;
		//sleep(random+1);
	}
}

void *consumer(void *data)
{
	double loopCnt = NUM_LOOP;
	int *id = (int *)data;
	int seed = *id;
	int random;
	while(loopCnt)
	{
		pthread_mutex_lock(&ConditionMutex);//obtain mutex to the Queue
		while(ResourceCount == 0)//if Queue is empty
			pthread_cond_wait(&QueueFullCond, &ConditionMutex);
		
		ResourceQueue[--QueueIndex] = -1;//resource consumed by consumer
		ResourceCount--;//decrement no. of resources available at Queue
		loopCnt--;
		
		pthread_cond_signal(&QueueEmptyCond);
		pthread_mutex_unlock(&ConditionMutex);//unlock mutex to Queue
		
		cout << "Consumer : "<< *id << " extracted.\n";
		//random = rand_r((unsigned int*)&seed) % 3 ;
		//sleep(random+1);
	}
}

