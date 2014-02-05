
/***************************************************************************


  Example     : pthread-loop-carried.c
 
  Objective   : Restructuring Loop-carried dependence to Loop-
                independent dependence using pthread APIs

  Input       : Nothing.

  Output      : Time in Second for Serial And Parallel Execution.
	                                                                                                                    
  Created     : Aug 2011
  

  E-mail      : RarchK      
                                    
****************************************************************************/

/*Header files inclusion */

#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

/* define size of thread and number of thread */ 
#define N 100000000
#define numThread 4 
	/* defining global variable */
	const double up = 1.0;
	double Sn = 1.0,origSn=1.0,Snp;
	double opts[N+1];
	double optp[N+1];
	int dist;
	pthread_mutex_t  mutex = PTHREAD_MUTEX_INITIALIZER;

/*call back function for thread */

void * dowork(int  threadid)
{
	int n;
	double Snpl;
	/* logic for parallized the code using pow() function */
	for(n=(threadid * dist) + 1 ; n<=((threadid +1) * dist); n++)
	{
			if(n==((threadid * dist)+1))
			{
				Snpl = origSn * pow(up,n);
			}
			else
			{
				Snpl = Snpl * up;
			}
			optp[n] = Snpl; 
	}
	
	if(threadid == (numThread - 1))
	{
		Snp=Snpl;
	}

 
}
/* main () starts */
int main(int  argc, char * argv [])
{

	/*variable declaration */
	int n,t,status;
	pthread_t threads[numThread];
	pthread_attr_t pta;
	int ret_count;
	double time_start, time_end;
	struct timeval tv;
	printf("\n\t\t---------------------------------------------------------------------------");
        printf("\n\t\t Centre for Development of Advanced Computing (C-DAC)");
        printf("\n\t\t Email : RarchK");
        printf("\n\t\t---------------------------------------------------------------------------");
	printf("\n\t\t Objective : Write pthread code to illustrate restructuring Loop-carried dependence to Loop-independent 	dependence using pthread APIs.  \n ");
        printf("\n\t\t..........................................................................\n");


	if ( numThread > 8 )
	{
		printf("\n Number of thread should be less than or equal to 8 \n");
		return;
	}

	/*Taking start time for serial calculation */
	gettimeofday(&tv,NULL);
	time_start = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
	
	/* serial loop with loop carried Dependence */
	for (n=0 ; n<=N; ++n)
	{
		opts[n] = Sn;
		Sn = Sn * up;
	}
	/*Taking end time for serial calculation */
	gettimeofday(&tv,NULL);
	time_end = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
	printf("\n\t\t Time in serial Execution in  Seconds (T)  :  %lf",(time_end - time_start));

	dist = N/numThread;
	optp[0]= origSn;
	ret_count=pthread_attr_init(&pta);

	/*Taking start time for parallel calculation */
	gettimeofday(&tv,NULL);
        time_start = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
	
	/* Loop for Ceating Thread's */
	for(t=0;t<numThread;t++)
	{
		ret_count = pthread_create(&threads[t], &pta,(void *(*) (void *))  dowork, (void *)t);
            if (ret_count)
            {
               printf("ERROR; return code from pthread_create() is %d\n", ret_count);
               exit(-1);
            }

	}
	/* Loop For Join Thread's */
	for(t=0;t<numThread;t++)
        {
                pthread_join(threads[t],(void**) & status);

        }

	
	Snp = Snp * up;	
	/*Taking end time for parallel calculation */
	gettimeofday(&tv,NULL);
        time_end = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
	printf("\n\t\t Time in parallel Execution in  Seconds (T) : %lf",(time_end - time_start));
	printf("\n");
	
	/*Loop For Comparning The Result */
	for (n=0 ; n<=N; ++n)
        {
               
		if((float)opts[n] !=(float) optp[n])
                {
			printf("Result is not Same ");
			return;
		}
        }


		printf("\nserial Sn= %lf parallel Snp= %lf\n",Sn,Snp);
		if((float) Sn!=(float)Snp)
		{
			printf("Both Serial(Sn) and Parallel (Snp) Variable are  Not Same ");
		}
	
}



