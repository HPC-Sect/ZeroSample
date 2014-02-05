




/***************************************************************************** 


                

 Example               : omp-loop-invert.c


 Objective             : Write an OpenMP Program to demonstrate  Performance improvement
                  	 by doing the loop inverting.

 Input                 : a) Number of threads
         
                  	 b) Size of matrices (numofrows and noofcols )


 Output                : Time taken for the computation.	                                            
                                                                        
 Created               : Aug 2011. 
 
 

 Author                : RarchK                                          


*********************************************************************************/



#include <stdio.h>
#include <sys/time.h>
#include <omp.h>
#include<math.h>
#include <stdlib.h>


/* Main Program */
main(int argc,char **argv)
{
	int            i,j,Noofthreads,Matsize;
	float         **Matrix;
 	struct 		timeval  TimeValue_Start, TimeValue_Start1;
        struct 		timezone TimeZone_Start, TimeZone_Start1;

        struct 		timeval  TimeValue_Final, TimeValue_Final1;
        struct 		timezone TimeZone_Final, TimeZone_Final1;
        long            time_start, time_end;
        double          time_overhead1,time_overhead2;



	printf("\n\t\t---------------------------------------------------------------------------");
        printf("\n\t\t Email : RarchK");
        printf("\n\t\t---------------------------------------------------------------------------");
        printf("\n\t\t Objective :  OpenMP Program to demonstrate  Performance improvement ");
        printf("\n\t\t by doing the loop inverting. .");
        printf("\n\t\t..........................................................................\n");


	/*i Checking for command line arguments */
        if( argc != 3 ){

           printf("\t\t Very Few Arguments\n ");
           printf("\t\t Syntax : exec <Threads> <matrix-size>\n");
           exit(-1);
        }


        Noofthreads=atoi(argv[1]);
        Matsize=atoi(argv[2]);          

        if ((Noofthreads!=1) && (Noofthreads!=2) && (Noofthreads!=4) && (Noofthreads!=8) && (Noofthreads!= 16) ) {
               printf("\n Number of threads should be 1,2,4,8 or 16 for the execution of program. \n\n");
               exit(-1);
         }
 

       /* printf("\n\t\t Enter the size of the Matrix \n");
        scanf("%d",&Matsize);*/     

         printf("\n\t\t Threads               : %d",Noofthreads); 
         printf("\n\t\t Matrix Size           : %d",Matsize); 

         /* Read the input */
	 Matrix = (float **)malloc( Matsize *sizeof(float *));
         for(i=0 ; i<Matsize ; i++)
                        {
                                Matrix[i] = (float *)malloc(Matsize * sizeof(float));
                                for(j=0; j<Matsize; j++) {
                                        Matrix[i][j] = cos(i*1.0) ;
                                }
                        }

       /* .......................................................................................
          This section parallelizes the sequential for loop into Parallel for loop.
          Because of the data dependency between the two rows, the two rows cannot 
          be updated simultaneously. That is the loop index j can be parallelized
          but index i cannot.
          If the Parallel for inserted before the inner loop, the resulting Parallel program execute 
          correctly, but it may not exhibits good performance. (Matsize-1) fork/join steps required.
         ........................................................................................*/

  	gettimeofday(&TimeValue_Start, &TimeZone_Start);

	omp_set_num_threads(Noofthreads);

        for(i = 1; i < Matsize ; i++){
      	#pragma omp parallel for  
       	for(j = 1 ;j  < Matsize;j++)
         {
             		Matrix[i][j] =2 * Matrix[i-1][j]; 
        	} 
              	
       	}
               

	gettimeofday(&TimeValue_Final, &TimeZone_Final);

        time_start = TimeValue_Start.tv_sec * 1000000 + TimeValue_Start.tv_usec;
        time_end = TimeValue_Final.tv_sec * 1000000 + TimeValue_Final.tv_usec;
        time_overhead1 = (time_end - time_start)/1000000.0;

        /* .......................................................................................
          This section parallelizes the sequential for loop into Parallel for loop.
          Because of the data dependency between the two rows the two rows cannot
          be updated simultaniously. That means the loop index j can be parallelized
          but i cannot.
          if the loop is invert and then apply parallel for before the outer loop.
          only single fork/join step required.   
	........................................................................................*/
   

	time_start=0;
	time_end=0;

	gettimeofday(&TimeValue_Start1, &TimeZone_Start1);

       #pragma omp parallel for private(i)
       for(j = 1 ;j  < Matsize;j++) {
           for(i = 1; i < Matsize ; i++){
                        Matrix[i][j] =2 * Matrix[i-1][j];
           }

        }


        gettimeofday(&TimeValue_Final1, &TimeZone_Final1);
        
        time_start = TimeValue_Start1.tv_sec * 1000000 + TimeValue_Start1.tv_usec;
        time_end = TimeValue_Final1.tv_sec * 1000000 + TimeValue_Final1.tv_usec;
        time_overhead2 = (time_end - time_start)/1000000.0;



	printf("\n\n\t\t Time Taken in Seconds (By Parallel Computation )           :%lf  ", time_overhead1);
	printf("\n\t\t Time Taken in Seconds (By Inverting loop Computation )     :%lf \n ", time_overhead2);
       
        printf("\n\t\t..........................................................................\n");


}
