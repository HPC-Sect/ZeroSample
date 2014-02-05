/*
*******************************************************************

     Example 1.6    : mpicpp-sum-associative-fanin-blocking-tree.C   

     Objective      : To find sum of 'n' integers on 'p' processors using
                      'Associative Fan-in ' rule.
		       MPI Blocking communication library calls are used.
                        
     Input          : Automatic input generation.
                         The rank of each process is input on each process.

     Output         : Process with rank 0 should print the sum of 'n' values

     Condition      : Number of processes should be less than or equal
                      to 8. 

  Created     	    : Aug 2011    

     E-mail         : RarchK                                          
	
***************************************************************************
*/

#include<iostream>
#include<string.h>
#include<math.h>
#include<cstdlib>
#include<iomanip> 
#include<unistd.h>
#include "mpi.h"
using namespace std;

int main(int argc,char *argv[])
{
   int root = 0,myrank,numprocs,source,destination;
   int dest_tag ,source_tag ;
   int sum = 0,value;
   int level,next_level,ilevel;
   int no_levels;

   MPI::Status status;
  
  /* .....Intitializing MPI.....*/

    MPI::Init(argc,argv);
    numprocs = MPI::COMM_WORLD.Get_size();
    myrank   = MPI::COMM_WORLD.Get_rank(); 

   /* checking for the validations  */

     no_levels = (int)(log10((double)(numprocs)) / log10((double) 2));

     if(!(no_levels == ((int)(no_levels)) ? 1 : 0))
     {
       if(myrank == root)	
       	cout<<"\n number of processors is not power of  2\n";
       MPI::Finalize();
       exit(-1);
     }
     
     if(numprocs != 1 && numprocs != 2 && numprocs != 4 && numprocs != 8)
     {
	if(myrank == root)
	  cout<<"The number of processor should be 1 or 2 or 4 or 8"<<endl;
	MPI::Finalize();
	exit(-1);
     }
    
 
      sum = myrank;
      source_tag = 0;
      dest_tag = 0;

       for(ilevel = 0 ; ilevel < no_levels ; ilevel++)
       {
          level = (int)( pow((double)2 , (double) (ilevel)));
          
           if((myrank % level) == 0)
            {
              
             next_level = (int) (pow((double)2, (double) (ilevel+1)));

               if((myrank % next_level) == 0)
                 {
                   source = myrank + level;

               MPI::COMM_WORLD.Recv(&value,1,MPI::INT,source,source_tag,status); 
                   sum = sum + value;
                  
                  }
              
               else
                {
 
                 destination = myrank - level;
                
                 MPI::COMM_WORLD.Send(&sum,1,MPI::INT,destination,dest_tag);
               
                 }
             
                }    
            
              }            
 
          if(myrank == root)
           {

            cout<<"\n my rank : "<<myrank<<" final sum :  "<<sum;  
            cout<<"\n";
             
             }

     /*.....Finalizing MPI ....*/

      MPI::Finalize();
      return 0;
    }
