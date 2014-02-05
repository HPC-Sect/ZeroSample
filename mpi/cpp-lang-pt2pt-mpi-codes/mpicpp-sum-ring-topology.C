/*
*******************************************************************

    Example 1.5    : mpicpp-sum-ring-topology.C

    Objective      : To find sum of 'n' integers on 'p' processors using
                     point-to-point communication library calls and
                     ring topology.

    Input          : Automatic input generation 
                     the rank of each process is input on each process

    Output         : Process with Rank 0 should print the sum of 'n' values

    Condition      : Number of processes should be less than or equal to 8.
	
  Created     	    : Aug 2011    

    E-mail         : RarchK                                          

***********************************************************************
*/

#include<iostream>
#include<string.h>
#include<unistd.h>
#include "mpi.h"
using namespace std;

int main(int argc,char *argv[])
{
    int root = 0,myrank,numprocs,source,destination;
    int dest_tag ,source_tag ;
    int sum = 0,value;
   MPI::Status status;

    /*....Intializing MPI..... */
    MPI::Init(argc,argv);
    numprocs=MPI::COMM_WORLD.Get_size();
    myrank=MPI::COMM_WORLD.Get_rank();

    if(myrank == root)
    {
        destination = myrank + 1;
        dest_tag    = 0;
       
        MPI::COMM_WORLD.Send(&myrank,1,MPI::INT,destination,dest_tag);
        }
    else
      {
         if(myrank < numprocs - 1)
          {
            source = myrank - 1;
            source_tag = 0;

           MPI::COMM_WORLD.Recv(&value,1,MPI::INT,source,source_tag,status);
           
           sum  =  myrank + value;
           destination = myrank + 1;
                             
            MPI::COMM_WORLD.Send(&sum,1,MPI::INT,destination,dest_tag);
           }
          else
           {
             source = myrank - 1;
             source_tag = 0;
 
             MPI::COMM_WORLD.Recv(&value,1,MPI::INT,source,source_tag,status);
 
             sum = myrank + value;
             }
          }
      if(myrank == root)
      {
         source = numprocs - 1;
         source_tag = 0;
       
        MPI::COMM_WORLD.Recv(&sum,1,MPI::INT,source,source_tag,status);

         cout<<"\n my rank : "<<myrank<<" Sum of "<<numprocs<< " Integers is "<<sum;  
         cout<<"\n";
        }
    if(myrank == (numprocs - 1))
    {
       destination = 0;
       dest_tag = 0;
     
       MPI::COMM_WORLD.Send(&sum,1,MPI::INT,destination,dest_tag);
      }

    /*..... Finalizing MPI......*/

      MPI::Finalize();
      return 0;
    }
