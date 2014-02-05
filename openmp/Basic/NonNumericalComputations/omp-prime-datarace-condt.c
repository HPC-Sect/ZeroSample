


/*****************************************************************************



				

 Example 1.7           : omp-prime-datarace-condt.c


 Objective             : Write an OpenMP program to find the total prime numbers
			 between a given range of numbers .This program demonstrates
			 how to avoid Data Race Condition using OpenMP Critical Section.

 Input                 : Number of threads.
			 
			 Upper bound to find the Prime Number. By default Lower bound
			 is 1.The program finds the number of primes without Critical section
			 with Critical Section and also checks with the serial computation.

 Output                : No of Primes found between 1 to Upper-bound without Critical Section
   			 No of Primes found between 1 to Upper-bound with Critical Section
			 No of Primes found between 1 to Upper-bound using serial computation.	                                            
                                                                        
 Created               : Aug 2011 . 
       
 
 Author                : RarchK                                          


*********************************************************************************/


#include<stdio.h>
#include<omp.h>
#include<math.h>
#include<stdlib.h>

int is_prime(int number ) {

        	int factor;
                int maxlimit ;

                maxlimit = sqrt(number);

                for ( factor = 3;factor <= maxlimit ; factor++) {
             
                	if( number % factor != 0) 
                    		continue; 
                 	else 
                   		return 0; 
                } 
              
          return 1;  
}

/* Main Program */
main(int argc , char **argv)
{
        
	int             *Array,*Primearray,*Check,Noofthreads,i;
	int             Countdatarace=0,Countparallel=0,Count=0,number,Maxnumber; 


        printf("\n\t\t---------------------------------------------------------------------------");
        printf("\n\t\t Email : RarchK");
        printf("\n\t\t---------------------------------------------------------------------------");
        printf("\n\t\t Objective : Data Race condition  ");
        printf("\n\t\t OpenMP program to find the total prime numbers between a given range of numbers ");
        printf("\n\t\t using OpenMP Parallel for directive and Critical Section ");
        printf("\n\t\t..........................................................................\n");

	 /* Checking for command line arguments */
        if( argc != 3 ){

           printf("\t\t Very Few Arguments\n ");
           printf("\t\t Syntax : exec <Threads> <upper bound to fine prime nos.>\n");
           exit(-1);
        }

        Noofthreads=atoi(argv[1]);
        if ((Noofthreads!=1) && (Noofthreads!=2) && (Noofthreads!=4) && (Noofthreads!=8) && (Noofthreads!= 16) ) {
               printf("\n Number of threads should be 1,2,4,8 or 16 for the execution of program. \n\n");
               exit(-1);
         }

        Maxnumber=atoi(argv[2]);
          
	/*printf("\n\t\t Enter the upper bound  to find the prime no. \n");
	scanf("%d", &Maxnumber);*/

       /* Upper bound to find the prime no. */
	if ( Maxnumber <= 0) {
		printf("\n\t\t To find the Prime number the upper bound should be greater than 2 \n ");
		exit(-1);
	}

        printf("\n\t\t Threads                        :     %d ",Noofthreads); 
        printf("\n\t\t Range to find Prime No. is     : 1 - %d ",Maxnumber);

	/* Dynamic Memory Allocation */
	Array = (int *) malloc(sizeof(int) * Maxnumber);
	Primearray = (int *) malloc(sizeof(int) * Maxnumber);
	Check = (int *) malloc(sizeof(int) * Maxnumber);

	/* Array Elements Initialization */
	for (i = 0; i < Maxnumber ; i++) {
	 	Array[i] = 0  ;
	 	Primearray[i] = 0  ;
                Check[i] = 0; 
         }

        /* set the number of threads */
	omp_set_num_threads(Noofthreads);
 
	/* OpenMP Parallel For Directive    */
	#pragma omp parallel for 
   	 for (number =3 ; number < Maxnumber  ; number += 2 )
    	{
                 if (is_prime(number))   
		{
           		  Array[ Countdatarace ] = number;
            		  Countdatarace++;  	 /* Data Race condition */
        	}	
   	 }


	/* OpenMP Parallel For Directive And Critical Section */
        #pragma omp parallel for
         for (number =3 ; number < Maxnumber  ; number += 2 )
        {
                 if (is_prime(number))
                {
                        #pragma omp critical
                        {
                          Primearray[ Countparallel ] = number;
                          Countparallel++;
                        }
                }
         }



        /* Serial computation */
	 for (number =3 ; number < Maxnumber  ; number += 2 )
        {
                 if (is_prime(number))
                {
                          Check[ Count ] = number;
                          Count++;
                }
         }


          printf("\n\n\t\t Prime number calculation between [ 1 - %d ] ...........Done \n\n ",Maxnumber); 


	 printf("\n\t\t The Prime No. found range [ 1 - %d ] by parallel calculation ( without critical section ) is : %d ",Maxnumber,Countdatarace); 
	 printf("\n\t\t The Prime No. found range [ 1 - %d ] by parallel calculation (with critical section ) is     : %d ",Maxnumber,Countparallel); 
         printf("\n\t\t The Prime No. found range [ 1 - %d ] by serial calculation is                                : %d ",Maxnumber,Count); 



	/* Freeing Memory */
	free(Array);
	free(Primearray);
	free(Check);

        printf("\n\n\t\t.............................................................................................\n");
}
