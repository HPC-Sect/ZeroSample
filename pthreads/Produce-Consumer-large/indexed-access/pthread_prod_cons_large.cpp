/*
 * 	
 *
 * Date	  : Aug 2011
 * File	  : pthread_prod_cons_large.cpp
 * Desc   : Producer-Consumer problem with large no. of threads and large no. of resources
 * 			(as many as user specifies) using mutex
 * Input  : NumThreads, NumResources & ThreadAffMask (all 3, first 2 or first 1 only are
 * 			also accepted) 
 * Output : Thread-affinity set (if specified) , time taken to execute in sec & microsec.
 * E-mail : RarchK      
 */

#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sched.h>//for thread affinity

using namespace std;

#define NUM_LOOP 10000 //Number of loops for each thread.
int *ResourceQueue;//queue for the resources produced.
int QueueIndex;
double ResourceCount;
int NumThreads;//equal for producer/consumer
int NumResource;//maximum num., of resource on shared memory.

pthread_mutex_t QueueMutex;//mutex object for accessing queue.

void *producer(void *data);
void *consumer(void *data);

int main(int argc, char *argv[])
{
	int temp;
	int rc; //return code
	
	double timetaken;
	struct timeval tv_start, tv_end;//for gettimeofday()
	struct timezone tz_start, tz_end;//for gettimeofday()
	
	unsigned long ThreadAffMask;//to obtain Thread Affinity (no. of processors to which thread is bound)
	unsigned int ThreadAffMaskLen = sizeof(ThreadAffMask);//length of AffinityMask
	pid_t ProgPid = getpid();//PID of the program for thread affinity 
	
	ResourceCount = 0;//initially no resources in the queue.
	QueueIndex = 0;
	if(argc == 2)
	{
		NumThreads = atoi(argv[1]);
		cout << "Number of resources not provided.\n"
				"Assuming 1000 resources in the shared queue."
				"\n";
		NumResource = 1000;
	}
	else if(argc == 3)
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
	long ThreadIds[NumThreads*2];//for passing unique ID nos. to each thread.
	pthread_t  tid[NumThreads*2];
	pthread_mutex_init(&QueueMutex, NULL);
	
	//start time
	gettimeofday(&tv_start, &tz_start);
	
	//Thread Create operation - producer
	for(int i=0; i<NumThreads; i++)
	{
		ThreadIds[i] = i;
		rc = pthread_create(&tid[i], NULL, producer, (void *)&ThreadIds[i]);
		if(rc==-1)
			perror("pthread_create() ERROR\n");
	}
	//Thread Create operation - consumer
	for(int i=NumThreads; i<NumThreads*2; i++)
	{
		temp = i%NumThreads;
		ThreadIds[i] = temp;
		rc = pthread_create(&tid[i], NULL, consumer, (void *)&ThreadIds[i]);
		if (rc==-1)
			perror("pthread_create() ERROR\n");
	}
	//Thread Join operation
	for(int i=0; i<NumThreads*2; i+=2)
	{
		pthread_join(tid[i], NULL);
		pthread_join(tid[i+1], NULL);
	}
	
	//end time
	gettimeofday(&tv_end, &tz_end);
	//retrieve no. of cores to which thread binded
	if(argc == 4)
	{
		rc = sched_getaffinity(ProgPid, ThreadAffMaskLen, (cpu_set_t *)&ThreadAffMask);
		printf("pid %d's executed with affinity: %08lx\n\n", ProgPid, ThreadAffMask);
	}
	//time taken
	timetaken = tv_end.tv_sec*1000000 + tv_end.tv_usec - (tv_start.tv_sec*1000000 + tv_start.tv_usec);
	cout << "Execution time (micro sec) : " << timetaken << endl;
	cout << "Execution time (sec)       : " << timetaken/1000000  << endl;
	
	return 0;
	
}

void *producer(void *data)
{
	int inserted = 0;
	double loopCnt = NUM_LOOP;
	int *id = (int *)data;
	int seed = *id;
	int random;
	while(loopCnt)
	{
		inserted = 0;
		while( inserted == 0 )
		{
			pthread_mutex_lock(&QueueMutex);//obtain mutex to the Queue
			if(ResourceCount < NumResource-1)//if Queue is not full
			{
				ResourceQueue[QueueIndex++] = 100;//resource produced by producer
				ResourceCount++;//increment no. of resources available at Queue
				inserted = 1;
				loopCnt--;
			}
			pthread_mutex_unlock(&QueueMutex);//unlock mutex to Queue
			
		}
		cout << "Producer : "<< *id << " inserted.\n";
		//random = rand_r((unsigned int*)&seed) % 3 ;
		//sleep(random+1);
	}
}

void *consumer(void *data)
{
	int extracted = 0;
	double loopCnt = NUM_LOOP;
	int *id = (int *)data;
	int seed = *id;
	int random;
	while(loopCnt)
	{
		extracted = 0;
		while(extracted == 0)
		{
			pthread_mutex_lock(&QueueMutex);
			if(ResourceCount > 0)
			{
				ResourceQueue[--QueueIndex] = -1;//remove the resource
				ResourceCount--;//reduce no. of resources available by one.
				extracted = 1;
				loopCnt--;
			}
			pthread_mutex_unlock(&QueueMutex);
			
		}
		cout << "Consumer : "<< *id << " extracted.\n";
		//random = rand_r((unsigned int*)&seed) % 3 ;
		//sleep(random+1);
	}
}

