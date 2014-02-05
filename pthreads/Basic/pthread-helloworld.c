
/********************************************************************
                       

 Example                : pthread-helloworld.c


 Objective  		: To print Hello World using threads.
                    	  Demonstrates use of:
                    		pthread_create()
                    		pthread_join()
          	    		pthread_attr_init()
		    		pthread_attr_destroy()
 Input                 : None.

 Output                : Hello World.	                                            
                                                                        
 Created               : Aug 2011
 
 
 E-mail                : RarchK                                          

*************************************************************************/





#include<pthread.h>
#include<stdio.h>

void * printmsg(char *s)
{
	
	printf(" %s", s);
	
	return;
	
} 

main(int argc, char *argv[])
{
	
		int ret_count;
		pthread_t thread1, thread2;
		pthread_attr_t pta;


   		printf("\n\t\t---------------------------------------------------------------------------");
  		printf("\n\t\t Centre for Development of Advanced Computing (C-DAC)");
		printf("\n\t\t Email : RarchK");
  		printf("\n\t\t---------------------------------------------------------------------------");
  		printf("\n\t\t Objective : Pthread code for printing Hello World!! \n ");
  		printf("\n\t\t..........................................................................\n");

	
		ret_count=pthread_attr_init(&pta);
		if(ret_count)
		{
			printf("ERROR; return code from pthread_attr_init() is %d\n", ret_count);
         		exit(-1);
		}


		ret_count=pthread_create(&thread1, &pta, (void *(*) (void *)) printmsg, (void *) "Hello ");
		if (ret_count)
		{
			printf("ERROR; return code from pthread_create() is %d\n", ret_count);
         		exit(-1);
      		}
	
		ret_count=pthread_create(&thread2, &pta, (void *(*) (void *)) printmsg, (void *) "World!");
		if (ret_count)
                {
                        printf("ERROR; return code from pthread_create() is %d\n", ret_count);
                        exit(-1);
                }

	

	/*
	 * Join the threads to inform the main thread to wait till they are
	 * done.
		 */ 
		ret_count=pthread_join(thread1, NULL);
		if (ret_count)
      		{		
         		printf("ERROR; return code from pthread_join() is %d\n", ret_count);
         		exit(-1);
      		}
	
		ret_count=pthread_join(thread2, NULL);
      		if (ret_count)
      		{
         		printf("ERROR; return code from pthread_join() is %d\n", ret_count);
         		exit(-1);
      		}
		
		ret_count=pthread_attr_destroy(&pta);
	      	if (ret_count)
      		{
         		printf("ERROR; return code from pthread_attr_destroy() is %d\n", ret_count);
         		exit(-1);
      		}		
		printf("\n");
	
} 
