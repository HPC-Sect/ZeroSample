/*******************************************************************
                     
           
           OpenMP-3.0 Example  Codes Beta-v1.0      
        
File          :  icv-threads-openmp3x.c  

Description   : Simple example program to demonstrate the use of 
                OpenMP Library calls to change the default values  
                of the internal control variables. 

		- omp_set_nested() change ICV nest-vars : which enable 
		  of disable the nested parallelism 
                - omp_set_max_active_levels() change ICV max_active-levels-var 
                  :which limits the number of nested active parallel regions
                - omp_set_dynamic() change ICV dyn-vars : which enable or disable 
		  the dynamic adjustment of the nuber of threads vailable for the 
		  execution of subsequent parallel region.
                - omp_set_num_threads() change ICV nthread-vars : which sets the 
		  number of threads for the next parallel region.  

OpenMP Pragma /
Function Used :  
		- omp_set_nested()
   		- omp_set_max_active_levels()
   		- omp_set_dynamic()
   		- omp_set_num_threads()
		- omp_get_max_active_levels(), 
		- omp_get_num_threads(),
		- omp_get_max_threads()

Input         : None 

Output        : Values of Internal Control Variables                          
**********************************************************************/

/* Header file inclusion */
#include <stdio.h>
#include <omp.h>

/* main function */
int main (void)
{
  
	/* OpenMP library functions to change the default values
      	of the internal control variable */
   
	omp_set_nested(1);  /* Enable the Nested Parallel region */
   	omp_set_max_active_levels(8); /* Enable the maximum active levels */
   	omp_set_dynamic(0); /* Disable the dynamic thread creation */
   	omp_set_num_threads(2); /* Set the no. of threads to 2 */

   	/* Outer : Create the parallel region */
   	#pragma omp parallel
      	{
         	omp_set_num_threads(3);
         
   		/* Inner: Create the parallel region inside the outer parallel region */
         	#pragma omp parallel
           	{
               		omp_set_num_threads(4);
               		#pragma omp single /* Restricting the one thread to do the work */
                  	{
                    		/*
                     		* The following should print:
                     		* Inner: max_act_lev=8, num_thds=3, max_thds=4
                     		* Inner: max_act_lev=8, num_thds=3, max_thds=4
                     		*/
                     		printf ("\n\t\t Inner: max_act_lev=%d, num_thds=%d,max_thds=%d\n",omp_get_max_active_levels(), omp_get_num_threads(),omp_get_max_threads());
                   	}
                 } /* End of inner parallel region */
             	#pragma omp barrier /* Syncronization point */
               	#pragma omp single /* Outer: Restricting the one thread to do the work */
                 {
                 	/*
                  	* The following should print:
                  	* Outer: max_act_lev=8, num_ thds=2, max_thds=3
                  	*/
                 	printf ("\n\t\t Outer: max_act_lev=%d, num_thds=%d,max_thds=%d\n",omp_get_max_active_levels(), omp_get_num_threads(),omp_get_max_threads());
                 }
            } /* End of outer parallel region */


      } /* End of main function */

