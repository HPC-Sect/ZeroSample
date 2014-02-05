

/*****************************************************************************




 Example 1.10           : omp-workshare-section.c





 Objective              : Write an OpenMP Program to illustrate Work-Sharing section.

		  	  In this , the OpenMP SECTION directive is used to assign

   		 	  different  operations to each thread that executes a SECTION.

 

 Input                  :a) Number of threads

         

                  	 b) Size of matrices (numofrows and noofcols )



 Output                 : Time taken by the parallel and serial computation                                            




 Author                 : RarchK                                          





*********************************************************************************/





#include <stdio.h>

#include <sys/time.h>

#include <omp.h>

#include <math.h>

#include <stdlib.h>



/* Main Program */

main(int argc,char **argv)

{

	int            NoofCols,NoofRows,index,VectorSize,icol,vec_col,irow,MatrixFileStatus=1,VectorFileStatus=1;

	int             matrowsize,matcolsize,vectorsize;

	float         **Matrix, **Matrix_one,*Vector, *Result_Vector;

        FILE           *fp1; 

 	struct 		timeval  TimeValue_Start, TimeValue1_Start;

        struct 		timezone TimeZone_Start, TimeZone1_Start;



        struct 		timeval  TimeValue_Final, TimeValue1_Final;

        struct 		timezone TimeZone_Final, TimeZone1_Final;

        long            time_start, time_end;

        double          time_overhead1,time_overhead2;


	int icol1 ,irow1;




	printf("\n\t\t---------------------------------------------------------------------------");


        printf("\n\t\t Email : RarchK");

        printf("\n\t\t---------------------------------------------------------------------------");

        printf("\n\t\t Objective : OpenMP Work-Sharing Construct .");

        printf("\n\t\t The OpenMP SECTION directive is used to assign different  operations to each ");

        printf("\n\t\t thread that executes a SECTION."); 

        printf("\n\t\t OpenMP Work-Shareing Construct : SECTION Directive . ");

        printf("\n\t\t..........................................................................\n");



         

       if(argc != 4){

	 	printf("\t\t Very Few Arguments\n ");

           	printf("\t\t Syntax : exec <NoOfRows> <NoOfColums> <Vector-size>  \n");

           	exit(-1);



       }



       /* printf("\n\t\t Enter the No. of Rows and Columns of Matrix and Vector Size \n");

        scanf("%d%d%d",&matrowsize,&matcolsize,&vectorsize); */

  matrowsize = atoi(argv[1]);
         matcolsize = atoi(argv[2]);
         vectorsize = atoi(argv[3]);

          if( matcolsize != vectorsize) {

                  

                printf("\n\n\t\t Matrix into vertor multiplication not possible ");

                printf("\n\t\t Vector Size != Matrix Col size \n");

                exit(-1);

          }  





       /* .......................................................................................

          The OpenMP SECTION directive is used to assign different  operations to each 

          thread that executes a SECTION. 

     

		- One thread will do the file operation

                - Second thread will do the matrix vector computation 

         ........................................................................................*/





  	gettimeofday(&TimeValue_Start, &TimeZone_Start);

	omp_set_num_threads(2);

      	#pragma omp parallel  
     	{
           #pragma omp sections 
           {
		
             #pragma omp section 
             {
                	printf("\n\n\t\t %d - Thread is doing section 1 ", omp_get_thread_num());  

  			if ((fp1 = fopen ("./data/mdata.inp", "r")) == NULL){
				printf("\n\t\t Failed to open the file\n "); 
       				MatrixFileStatus = 0;

  			}

/*
			if(MatrixFileStatus != 0) {
     				fscanf(fp1,"%d%d\n",&NoofRows,&NoofCols);

    		 		Matrix = (float **)malloc(NoofRows*sizeof(float *));
     				

				for(irow=0 ;irow<NoofRows; irow++){
 	 				Matrix[irow] = (float *)malloc(NoofCols*sizeof(float));
 	 				for(icol=0; icol<NoofCols; icol++) 
    	    					fscanf(fp1,"%f",&Matrix[irow][icol]);

                		}

    				fclose(fp1);
				free(Matrix);
              		} */
          	}

               #pragma omp section 
               {  	
	
       	       		printf("\n\n\t\t %d - Thread is doing section 2 ", omp_get_thread_num());  
    		 	Matrix_one = (float **)malloc( matrowsize *sizeof(float *));
    		 	Vector = (float *)malloc( vectorsize *sizeof(float));
    		 	Result_Vector = (float *)malloc(matrowsize  *sizeof(float));
			
		 	for(irow1=0 ; irow1<matrowsize ; irow1++)
                 	{

 	 			Matrix_one[irow1] = (float *)malloc(matcolsize * sizeof(float));
 	 			for(icol1=0; icol1<matcolsize; icol1++) {
    	    				Matrix_one[irow1][icol1] = (irow1*1.0) ;
                        		Vector[irow1] = (icol1*1.0) ;
                        		Result_Vector[irow1] = 0.0;

       	 			}

                	}
			
               

               		for(icol1 = 0; icol1 < matrowsize ; icol1++)
                    		for(irow1 = 0 ; irow1 < matcolsize; irow1++)
                       		{
                      			Result_Vector[icol1] = Result_Vector[icol1] + Vector[irow1] * Matrix_one[irow1][icol1]; 
                       		} 

              	
		
			free(Matrix_one);
              		free(Vector);
              		free(Result_Vector); 
			
             	}

               

        }  /* End of sections */

   } /* End of parallel section */


	gettimeofday(&TimeValue_Final, &TimeZone_Final);



        time_start = TimeValue_Start.tv_sec * 1000000 + TimeValue_Start.tv_usec;

        time_end = TimeValue_Final.tv_sec * 1000000 + TimeValue_Final.tv_usec;

        time_overhead1 = (time_end - time_start)/1000000.0;



	/*.................................................................................

          	This section implement the above section in serial way

           ...............................................................................*/



      	/* Serial computation */

  	gettimeofday(&TimeValue1_Start, &TimeZone1_Start);

    	

	 /* .......Read the Input file ......*/

         if ((fp1 = fopen ("./data/mdata.inp", "r")) == NULL){

         	printf("\n\t\t Failed to open the file\n ");

                MatrixFileStatus = 0;

         }



         if(MatrixFileStatus != 0) {

                 fscanf(fp1,"%d%d\n",&NoofRows,&NoofCols);



                /* ...Allocate memory and read Matrix from file .......*/

                 Matrix = (float **)malloc(NoofRows*sizeof(float *));



                for(irow=0 ;irow<NoofRows; irow++){

                        Matrix[irow] = (float *)malloc(NoofCols*sizeof(float));

                                for(icol=0; icol<NoofCols; icol++) {

                                        fscanf(fp1,"%f",&Matrix[irow][icol]);

                                }

                }

                fclose(fp1);

                free(Matrix);

          }

               





			Matrix_one = (float **)malloc( matrowsize *sizeof(float *));

                        Vector = (float *)malloc( vectorsize *sizeof(float));

                        Result_Vector = (float *)malloc(matrowsize  *sizeof(float));



                        for(irow=0 ; irow<matrowsize ; irow++)

                        {

                                Matrix_one[irow] = (float *)malloc(matcolsize * sizeof(float));

                                for(icol=0; icol<matcolsize; icol++) {

                                        Matrix_one[irow][icol] = cos(irow*1.0) ;

                                        Vector[irow] = cos(icol*1.0) ;

                                        Result_Vector[irow] = 0.0;

                                }

                        }



                        for(icol = 0; icol < matrowsize ; icol++)

                                for(irow = 0 ; irow < matcolsize; irow++)

                                {

                                        Result_Vector[icol] = Result_Vector[icol] + Vector[irow] * Matrix_one[irow][icol];

                                }



                        free(Matrix_one);

                        free(Vector);

                        free(Result_Vector);







	gettimeofday(&TimeValue1_Final, &TimeZone1_Final);



        time_start = 0;

        time_end = 0;



        time_start = TimeValue1_Start.tv_sec * 1000000 + TimeValue1_Start.tv_usec;

        time_end = TimeValue1_Final.tv_sec * 1000000 + TimeValue1_Final.tv_usec;

        time_overhead2 = (time_end - time_start)/1000000.0;





	printf("\n\n\t\t Time Taken (By Parallel Computation )    :%lf  ", time_overhead1);

	printf("\n\t\t Time Taken (By Serial Computation )      :%lf \n ", time_overhead2);

       

        printf("\n\t\t..........................................................................\n");





}

