
/******************************************************************************

    Example 2.2 	: mpicpp-scatter.C

    Objective           : To Scatter an integer array of size "n by 1"        
                          using MPI Collective communication library call 
                          (MPI_Scatter)

    Input               : Input Data file "sdata.inp".

    Output              : Print the scattered array on all processes.


    Necessary Condition : Number of processes should be
                          less than or equal to 8.

  Created     	    : Aug 2011    
    
    E-mail      	: RarchK                                          	

**************************************************************************/

     #include<iostream>
     #include<cstdlib>
     #include<iomanip>
     #include<unistd.h>
     #include"mpi.h"
     using namespace std;
   
    int main(int argc,char *argv[]) 
    {
     
        int root = 0,myrank,numprocs,source,destination,index;
        int dest_tag,source_tag;
        int sum = 0,value = 0;
        int dsize,scat_size;
        int *input_buffer,*recev_buffer;
        MPI::Status status;

      /*......MPI intializing .....*/
 
        MPI::Init(argc,argv);
        numprocs=MPI::COMM_WORLD.Get_size();
        myrank=MPI::COMM_WORLD.Get_rank();

	 /* checking that data can be distributed uniformly...*/
	 if(dsize % numprocs != 0)
         {
                if(myrank == 0)
                    cout<<"\nInput Cannot be Distributed Evenly....."<<endl;
                MPI::Finalize();
                exit(-1);
         }/* End of if(dsize%numprocs!=0) */

        if(myrank == root)
          {
            dsize = 100;

           /* Allocating Memory to hold values...*/
               input_buffer = new int[dsize];

           /* Verifying whether memory allocated or not..*/  
               if(input_buffer == NULL)
               {
                  cout<<"\n Memory Cannot be Allocated to hold Values...."; 
                  MPI::Finalize();
                  exit(-1);
                }           

               /* inserting values to array to scatter...*/ 
            for(index = 0;index < dsize;index ++)
            {
              input_buffer[index] = index;
             }
           }   /* End of if(myrank == root)   */
 
           /*... Broadcasting size ..*/
           MPI::COMM_WORLD.Bcast(&dsize,1,MPI::INT,root);

          /* Calculating scatter size  */ 
           int scatsize;
           scatsize = dsize/numprocs;
           /* Allocating memory to hold values..*/
              recev_buffer = new int[scatsize];
              if(recev_buffer == NULL)
               {
                  cout<<"\n Memory Cannot be Allocated to hold Values...."; 
                  MPI::Finalize();
                  exit(-1);
                }  
 
         /* Scattering values using Scatterv....*/
           MPI::COMM_WORLD.Scatter(input_buffer,scatsize,MPI::INT,recev_buffer,scatsize,MPI::INT,root);

    	    cout<<" \n My Rank ::> "<<" "<<myrank<<"\n";
            for(index = 0;index < scatsize;index++)
            {
             cout<<recev_buffer[index];
             cout<<" ";
            } 
             cout<<"\n\n";
	if(myrank == 0)
		delete(input_buffer);
	delete(recev_buffer);
        /*..... Finalizing MPI ....*/
         MPI::Finalize();
         return 0;
} 
