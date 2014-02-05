/*****************************************************************************



 Example               : omp-loop-carried-depend.c


 Objective             : Write in OpenMP progam for Loop-carried dependence using OpenMP parallel Directive .
 
 Input                 : a) Number of threads

                  	 b) Size of Array 


 Output                : Status of the computation i.e the comparitive results of the serial
                  	 and parallel computation.	                                            
                                                                        
 
 Author                : RarchK                                          


*********************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <sys/time.h>

int main(int argc,char **argv){

      const double up = 1.1 ;
      double Sn, origSn=1000.0;
      double *opt,*dependency_opt,*no_dependency_opt;
      int n,Noofthreads,N;

      printf("\n\t\t---------------------------------------------------------------------------");
      printf("\n\t\t Email : RarchK");
      printf("\n\t\t---------------------------------------------------------------------------");
      printf("\n\t\t Objective :  OpenMP Program to demonstrate Loop-Carried Dependency  ");
      printf("\n\t\t..........................................................................\n");
	
      if( argc != 3 ){

           printf("\t\t Very Few Arguments\n ");
           printf("\t\t Syntax : exec <Threads> <array-size>\n");
           exit(-1);
      }

      N = atoi(argv[2]); 
      Noofthreads=atoi(argv[1]);
      if ((Noofthreads!=1) && (Noofthreads!=2) && (Noofthreads!=4) && (Noofthreads!=8) && (Noofthreads!= 16) ) {
               printf("\n Number of threads should be 1,2,4,8 or 16 for the execution of program. \n\n");
               exit(-1);
      }

     /* printf("\n\t\t Read The Array Size \n");
      scanf("%d",&N);*/
 
      printf("\n\t\t Threads : %d ",Noofthreads);
      printf("\n\t\t Array Size  : %d  \n  ",N);

      opt = (double *)malloc( (N+1) *sizeof(double));
      dependency_opt = (double *)malloc( (N+1) *sizeof(double));
      no_dependency_opt = (double *)malloc( (N+1) *sizeof(double));
	
      /* Serial Computation in which the for loop contains
	 Loop-Carried Depedency.These dependencies are so named because 
         variables depend on previous iterations within a loop. To parallelize 
	 the loop, the dependency must be resolved .	
	  */
	
       Sn = 1000.0;
      for (n=0; n<=N; ++n) {
        opt[n] = Sn;
        Sn *= up;
      }
    
      //for (n=0; n<=N; ++n) 
//	printf("opt[%d]= %d\n",n,opt[n]);

      Sn = 1000.0;

     /* The above for Loop that is existing with Loop-Carried Dependency is
	parallelised */
      omp_set_num_threads(Noofthreads);

      #pragma omp parallel for private(n) 
      for (n=0; n<=N; ++n) {
        dependency_opt[n] = Sn;
        Sn *= up;
      }
 
      for (n=0; n<=N; ++n) {
	if ( opt[n] == dependency_opt[n]){}
	else{
	printf("\n\t\tIncorrect results found when Serial computation results & Parallel computation\n\t\twith Dependency results are compared\n");
        break;
	}
      }

      Sn = 1000.0;
      no_dependency_opt[0] = origSn;

      /*The For Loop is parallelised after resolving the dependency by pushing 
	out the update variable Sn*/
      omp_set_num_threads(Noofthreads);

      #pragma omp parallel for private(n) lastprivate(Sn)
      for (n=1; n<=N; ++n) {
          Sn = origSn * pow(up, n);
          no_dependency_opt[n] = Sn;
      }

      Sn *= up;

      for (n=0; n<=N; ++n) {
	if ( (int)(opt[n]) == (int)(no_dependency_opt[n])){}
	else{
	printf("%lf != %lf\n",opt[n],no_dependency_opt[n]);
	printf("\n\t\t Incorrect results found when Serial computation results & Parallel computation \n\t\twithout Dependency results are compared\n");
        break;
	}
      }
      //for (n=0; n<=N; ++n) 
	//printf("no_opt[%d]= %d\n",n,no_dependency_opt[n]);
     printf("\n\t\t..........................................................................\n");
 
     free(opt);
     free(dependency_opt);
     free(no_dependency_opt);
}
	
