/*
*******************************************************************

   Example 1.3  : mpicpp-sum-pt-to-pt.C

   Objective    : To find sum of 'n' integers on 'p' processors using
                    point-to-point communication library calls

   Input        : Input is automatically generated
                    The rank of each process is input on each Process.

   Output       : Process with rank 0 should print the sum of 'n' values

   Condition    : Number of processes should be less than or equal to 8

  Created     	    : Aug 2011    

   E-mail      	: RarchK                                          

************************************************************************
*/

     #include<iostream>
     #include<unistd.h>
     #include"mpi.h"
     using namespace std;
   
    int main(int argc,char *argv[]) 
    {
     
        int root=0,myrank,numprocs,source,destination,iproc;
        int dest_tag,source_tag;
        int sum=0,value=0;
        MPI::Status status;
 
   /*.... Intializing MPI .....*/
 
        MPI::Init(argc,argv);
        numprocs=MPI::COMM_WORLD.Get_size();
        myrank=MPI::COMM_WORLD.Get_rank();

        if(myrank != root)
         { 
            destination = 0;
            dest_tag    = 0;

           MPI::COMM_WORLD.Send(&myrank,1,MPI::INT,destination,dest_tag);
           }
      else
         { 
              for(iproc = 1;iproc < numprocs;iproc++)
                {
                 source     = iproc;
                 source_tag = 0;
             MPI::COMM_WORLD.Recv(&value,1,MPI::INT,source,source_tag,status);
              sum =sum + value;
                }
           cout<<" \n MyRank :: "<<myrank <<" Sum of first "  << numprocs<< " Integers is  "<<sum <<"\n"; 

          }

   /*.....Finalizing MPI ....*/
         MPI::Finalize();
        return 0;
     }
                





 
