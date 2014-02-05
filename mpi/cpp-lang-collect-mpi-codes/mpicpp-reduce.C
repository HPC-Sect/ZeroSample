/*
************************************************************************

   Example 2.1      : mpicpp-reduce.C

   Objective        : To find sum of 'n' integers on 'p'processors using 
                      MPI collective communication library 
                      call (MPI::COMM_WORLD.Reduce)

   Input            : Automatic input is generated
                      The rank of each process is input on each process 

   Output	    : Process with rank 0 should print the sum of 'n' values.

  Created     	    : Aug 2011    
   
   E-mail      	    : RarchK                                          	

*****************************************************************************
*/

     #include<iostream>
     #include<cstdlib>
     #include<iomanip>
     #include<unistd.h>
     #include"mpi.h"

     using namespace std;
   
    int main(int argc,char *argv[]) 
    {
        int root = 0,myrank,numprocs,source,destination,index;
        int destination_tag,source_tag;
        int sum = 0;
    /* ...... MPI Intializing .........*/
        MPI::Init(argc,argv);
        numprocs = MPI::COMM_WORLD.Get_size();
        myrank = MPI::COMM_WORLD.Get_rank();
   /* ....... reducing the ranks on all processors to a single value.....*/ 
        MPI::COMM_WORLD.Reduce(&myrank,&sum,1,MPI::INT,MPI_SUM,0);

         if(myrank==0)
           {
             cout<<"\n Sum of first " <<numprocs-1 <<" integers ::> " <<sum;
             cout<<"\n";
            }        
  /*....... Finalizing MPI.........*/
        MPI::Finalize();
        return 0;
} 
