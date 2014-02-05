/********************************************************************************* 
  

               OpenMP-3.0 Example Codes Beta-v1.0      
        
File          : tree-traverse-alg-openmp3x.c 
Date          : Aug 2011    

Description   : The example program demonstrate the use of openmp new feature 
		task construct to perform the parallelization of hierarchical 
		algorithum (inorder tree traversal) and compare it with the
		openmp-2.5 approach using single construct & dispaly the 
		time taken in both approcahes

		a) inorderTraverse_Task() :Uses OpenMP-3.0 Task Construct which is an 
                   efficient approah. Whenever a thread encounters a task construct, 
		   a new explicit task, An explicit task may be executed by any thread 
		   in the current team, in parallel with other tasks.In this approach the 
		   several task can be executed in parallel.

                b) inorderTraverse_Wtask() : Uses OpenMP-2.5 parallel - section  directive.
	           whenever a thread encounter the parallel region it creates the team of 
		   threads. When the thread in a team encounter the section directive it 
		   execute the section region.This approach can be costly, however, because 
		   of the overhead of parallel region creation, the risk of oversubscribing 
		   system resources, difficulties in load balancing, and different behaviors 
		   of different implementations

OpenMP pragma/
Directive used : #pragma omp parallel
		 #pragma omp section
		 #pragma omp task
                                 
Input         : - Number of Nodes in the link list
                - Number of threads to be used  
Output        : Time taken in both approach. 
 
*********************************************************************************/

/* Header file inclusion */
# include<stdio.h>
# include<stdlib.h>
# include <omp.h>


/* Global variable declaration */
long int totalNodes,numThreads;
FILE *fp;


/* Structure define tree node */
struct node
{ 	
	int info ;
	struct node *left;
	struct node *right;

}*root,*p,*q;


/* Function declaration  */
struct 	node *createTree();
void 	leftNode(struct node * ,int ); 
void 	rightNode(struct node * ,int );
struct	node *createNewNode(int);
void 	inorderTraverse_Task(struct node *); 
void 	inorderTraverse_Wtask(struct node *); 


/* Main Function */
int main(int argc,char **argv)
{ 
	int 	index=0, value,count=1;
	double 	start_time,end_time; 

	/* Checking for command line arguments */
        if( argc != 3 ){

           	printf("\t\t Very Few Arguments\n ");
           	printf("\t\t Syntax : exec <total-nodes>  <No. of Threads>\n");
           	exit(-1);
        }

	/* Initalizing the Number of nodes in the tree 
	   and the Number of threads*/
        totalNodes =atoi(argv[1]);
        numThreads =atoi(argv[2]);

        if(totalNodes<=0)
        {
                printf("\n\t Error : Number of nodes should be greater then 0\n");
                exit(-1);
        }

	
	/* Creating the data input file */ 
	if((fp = fopen("tree-input.dat","w"))==NULL)
	{
		printf("\n\t Failed to open the file \n");
		exit(-1);
	}

	/* Writing the input data to the file */ 
	for (index=0; index<totalNodes ; index++)
		fprintf(fp,"%d ",rand());

	fclose(fp);
	
	/* Function call to create the tree */
	root = createTree();

	start_time=omp_get_wtime();

	/* Creating the parallel region */
	#pragma omp parallel num_threads(numThreads)
	{
		/* Restricting the one thread to do the work */
		#pragma omp single	
		inorderTraverse_Task(root); /* Function call to perform the tree 
						* traversal using task construct */
	} /* end of parallel region */

 	end_time=omp_get_wtime();
	
	printf("\n\t\t Total Nodes       :  %ld ",totalNodes);
	printf("\n\t\t Number of Threads :  %ld ",numThreads);
	printf("\n\t\t Time Taken ( Task Construct : OpenMp-3.0 )          : %lf sec",(end_time-start_time));

	
	start_time=omp_get_wtime();
	 /* function call to perform the tree
	    * traversal without task construct */
		inorderTraverse_Wtask(root);

 	end_time=omp_get_wtime();

	printf("\n\t\t Time Taken (Parallel Section Directive : OpenMP-2.5) : %lf sec ",(end_time-start_time));
 	printf("\n\n\t Inorder tree traversal using Task Construct .................Done \n");


	
	return 0;		
}/* End of main */

/* Description : Function to create the Binary Search tree 
   @return : Retun the pointer to the root node 	
*/
struct node *createTree()
{
	int index,value;

	/* Open the file to read the input data */
	if((fp = fopen("tree-input.dat","r"))==NULL)
        {
                printf("\n\t Failed to open the file \n");
                exit(-1);
        }

        index=totalNodes;

	/* reading the first element */
        fscanf(fp,"%d",&value);

        /* Function call to create the first Node of the tree */
        root=createNewNode(value);

	/* Iterate over the loop to create the tree */
        while (index >1  ) {

		/* Reading the data from the input file */
                fscanf(fp,"%d",&value);
                p=root;
                q=root;

		if( value == p->info)
			continue;
		else
		{
			/* Check the condition for node insertion in right or left  */
                	while(value!=p->info && q!=NULL)
                	{
                        	p=q;
                        	if(value < p->info )
                                	q = p->left;
                        	else
                                	q = p->right;
                	}
                	if( value < p->info)
                	{
				/* If the value is less then the node value
				   then insert the value in left  
				 */
                       	 	leftNode(p,value);
                	}
                	else
                	{
				/* If the value is greater then the node value
				   then insert the value in right   
				 */
                        	rightNode(p,value);
                	}
                	index--;
        	}
	} /* End of while loop */
	
	fclose(fp);
	
	return root;
} /* End of Function */ 

/* Description 	: Helper function to create the new node    
   @param[value] : Data value of the node  	 
*/ 
struct node *createNewNode(int value )
{ 
	struct node *newnode;

	/* Allocating the memory to create the new node */
	if((newnode=(struct node *)malloc(sizeof(struct node)))==NULL){
		perror("\n\t Memory allocation for newnode ");
                exit(-1);
	}

	newnode->info=value;
	newnode->right=newnode->left=NULL;
	return(newnode);

}
/* Description 	: Function to create the left node of the tree 
   @param [*r]  : Position to insert the node  
   @param[value : data value in the node	
    
*/
void leftNode(struct node *r,int value )
{
	if(r->left!=NULL)
		printf("\n Invalid !");
	else
		r->left=createNewNode(value);
}
/* Description : Function to create the right node of the tree
   @param [*r]  : Position to insert the node  
   @param[value : data value in the node	
*/
void rightNode(struct node *r,int value)
{
	if(r->right!=NULL)
		printf("\n Invalid !");
	else
		r->right=createNewNode(value);
}
/*
Description :Uses OpenMP-3.0 Task Construct which is an efficient approah. 
Whenever a thread encounters a task construct,a new explicit task, An explicit 
task may be executed by any thread in the current team, in parallel with other 
tasks.In this approach the several task can be executed in parallel.

@param : starting pointer of the tree
*/
void inorderTraverse_Task(struct node *r)
{
		/* Generating task to traverse Left Subtree 
		   by the team of threads*/
 		if(r->left!=NULL){
		#pragma omp task
                        inorderTraverse_Task(r->left);
		}
		/* Generating task to traverse Right Subtree by the 
		   team of thread*/
 		if(r->right!=NULL){
		#pragma omp task
                                inorderTraverse_Task(r->right);
                } 

}

/*
Description :Uses OpenMP-2.5 parallel - section  directive.
whenever a thread encounter the parallel region it creates the team of 
threads. When the thread in a team encounter the section directive it 
execute the section region. Then the sections will be executed by different
threads parallely.This approach can be costly, however, because 
of the overhead of parallel region creation, the risk of oversubscribing 
system resources, difficulties in load balancing, and different behaviors 
of different implementations

@param : starting pointer of the tree
*/
void inorderTraverse_Wtask(struct node *r)
{
	omp_set_num_threads(numThreads);

       /* Create the parallel region */
	//#pragma omp parallel sections num_threads(2)
	#pragma omp parallel sections
	{
	 	/* Creating section to traverse the
		   Left subtree by a thread 1 */	
                #pragma omp section 
                if(r->left!=NULL){
                        inorderTraverse_Wtask(r->left);
                }
	 	/* Creating section to traverse the
		   Right subtree by a thread 2 */	
                #pragma omp section 
                if(r->right!=NULL){
                                inorderTraverse_Wtask(r->right);
                }
	}

}





