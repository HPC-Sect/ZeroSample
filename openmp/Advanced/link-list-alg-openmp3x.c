/*********************************************************************************

                    OpenMP-3.0  Example Codes Beta-v1.0      
        
File          : link-list-alg-openmp3x.c  
Date          : March 2010

Description   : The program perform the link list traversal(irregular parallelism)
		in parallel using the openmp-3.0 feature task construct and
		openMP-2.5 approach and measure the time taken in both the 
		approches.

		a) incrementList_Wtake (OpenMP-2.5) : When a thread encounter the 
	           parallel construct it creates the team of threads. The single 
		   construct inside a parallel region restrict that only one thread
		   at a time can process the node. Its an unintuitive and inefficient 
		   because only one thread at time is involved in processing which 
                   incured relatively high cost of the single construct.

		b) incrementList_Task(OpenMP-3.0) : This approach uses 
	           the openMP-3.0 task construct. Whenever a thread encounters
		   a task construct, a new explicit task, An explicit task may be
		   executed by any thread in the current team, in parallel with
		   other tasks.In this approach the several task can be executed in
		   parallel.
				 
OpenMP pragma/
Directive used : #pragma omp parallel
		 #pragma omp single
		 #pragma omp task 

Input         : - Number of Nodes in the link list
		- Number of threads to be used  
Output        : Time taken in both approach. 
 
*********************************************************************************/

/* Header file inclusion */
#include<stdio.h>
#include<stdlib.h>
#include <omp.h>

/* Global variable declaration */
long int totalNodes;
int   	 numThreads;


typedef struct node node;

/* Structure for link list node */
struct node
{ 	
	int data;
	struct node *link;
};

/* Function declaration to perform operation on Link List */
node *createList(node ** );
void incrementList_Task(node *);
void incrementListItem_Wtask(node *);
void processList(node *);
//void traverseList(node *);
 
/* Main Function */
int main(int argc,char** argv)
{ 

	node 	*start; 
	start = NULL;

 	/* Checking for command line arguments */
        if( argc != 3 ){

           printf("\t\t Very Few Arguments\n ");
           printf("\t\t Syntax : exec <total-nodes>  <No. of Threads>\n");
           exit(-1);
        }

	/* Initalizing Number of Nodes in the List and 
	   Number of threads */
        totalNodes =atol(argv[1]);
        numThreads =atoi(argv[2]);
	
	if(totalNodes<=0)
	{
		printf("\n\t Error : Number of nodes should be greater then 0\n");
		exit(-1);
	}
	
	
	/* Function Calling to create the link list */
	start=createList(&start);

	printf("\n\t\t Total Nodes in the List : %ld ",totalNodes);
	printf("\n\t\t Number of threads       : %d  ",numThreads);

         /* Check for the Empty link list condition */
	if ( start != NULL){
		//traverseList(start);
		
		/* Function Calling to process the list using Task Construct */
		incrementList_Task(start);	
		
		/* Function Calling to process the list using Parallel Construct */
		incrementListItem_Wtask(start);	
	
		//traverseList(start);
	}
	else
		printf("\n List is Empty \n");



	return 0;		
} /* End of main */

/* 	Description 	: Function to create the link list 
   	@param [ **q] 	: Start pointer to link list
	@return 	: Start pointer to the list 
	
*/ 
struct node *createList(node **q)
{ 
	node *temp,*r,*head;
	temp = *q;
	int count=1;
	
	while(count <= totalNodes ) { 
	/* If the node is the first node of the list */
	if(*q==NULL)
	{ 
		/* Create the node */
		if((temp = (node *)malloc(sizeof(struct node)))==NULL){
                	perror("\n\t Memory allocation for newnode ");
			printf(" \n\t Creating the singly Link List.................Failed. \n");
                	exit(-1);
        	}

		temp->data= rand();
		temp->link=NULL;
		*q=temp;
		head=temp;
	}
	else
	{ 
		temp = *q;
		while(temp->link !=NULL)
		{ 
			temp=temp->link;
		}
		/* Create the node */
		if((r = (node *)malloc(sizeof(struct node)))==NULL){
			perror("\n\t Memory allocation for newnode ");
			printf(" \n\t Creating the singly Link List.................Failed. \n");
                        exit(-1);
		}
		r->data=rand() ;
		r->link=NULL;
		temp->link=r;
	}
		count++;
	}
	
	return head ;
} /* End of Create List function */

/*	Description : 	Function to increment the List items using TASK CONSTRUCT (openmp-3.0) 
                	Whenever a thread encounters a task construct, a new explicit task, 
			An explicit task may be executed by any thread in the current team, 
			in parallel with other tasks.In this approach the several task can 
			be executed in parallel.

  	@param [node] : start pointer to link list
	@return	      : None
	
*/

 void incrementList_Task(node *head)
{

 	double start_time,end_time; 

	/* Set the number of threads in parallel region */
	omp_set_num_threads(numThreads);

	/* Get the start time */
	start_time=omp_get_wtime();
 		 
	/* Create the parallel region */ 
	#pragma omp parallel
    	{
        	#pragma omp single /*Restrict single thread create the task */
		{
			node * q = head;
               		while (q) {
        			#pragma omp task /* Create the task */
                       			processList(q);
                		q = q->link;
            		}
		}
      } /* end of the parllel region */
	
	end_time=omp_get_wtime();

	printf("\n\t\t Time taken ( Task Construct : openMP-3.0)    :  %lf sec", (end_time -start_time));
	
} /* End of the function */

/* Description : Function to increment the List items  WITHOUT using TASK CONSTRUCT (openmp-2.5) 
                 When a thread encounter the parallel construct it creates the team of threads. 
		 The single construct inside a parallel region restrict that only one thread
                 t a time can process the node. Its an unintuitive and inefficient because only 
		 one thread at time is involved in processing which incured relatively high cost of
		 the single construct.

   @param [head]: start pointer to link list    
   @return 	: None	
 
*/

 void incrementListItem_Wtask(node *head)
{

        double 	start_time,end_time;
	int 	list_Node[totalNodes],total_elements=0,i;
        
	/* Set the number of threads */
	omp_set_num_threads(numThreads);

	/* get the start time */
        start_time=omp_get_wtime();

        /* Create the team of thread */
        #pragma omp parallel
        {
                node * q = head;
                while (q) {
                	#pragma omp single /* Restrict single thread process the list */ 
                             processList(q);
                          q = q->link;
                        }
      } 

        end_time=omp_get_wtime();

        printf("\n\t\t Time taken ( Parallel / Single Directive : OpenMP-2.5) :  %lf sec\n", (end_time -start_time));

}/* End of the function */

/* 
 Description 	: Function to increment the node item.
 @Param[q] 	: pointer to the current node
 @return 	: None

*/
void processList(struct node *q)
{
	//printf("\n\t My thread ID %d\n", omp_get_thread_num());
	q->data = (q->data + q->data);
}

/* Function to travese the list */
/*void traverseList(node *q)
{
        if(q==NULL)
        {
                printf("\n\nEmpty Link List.Can't Display The Data");
                exit(-1);
        }
	printf("\t");
        while(q!=NULL)
        {
                printf(" %d--->",q->data);
                q=q->link;
        }
}*/

