/*
***************************************************************************

    Example 2.8  : mpicpp-diag-comm.C	

    Objective	 : Mpi program to create a communicator consisting of  
                   processors that lie on the diagonal in the  square 
                   grid of processors, arranged in the form of a matrix

    Input        : Communicator consisting of all  the processors 

    Condition	 : Number of Processes should be perfect square 
                   and  less than 8. 		

  Created     	    : Aug 2011    

    E-mail       : RarchK                                          	

*****************************************************************************
*/
 
#include<iostream>
#include<math.h>
#include<unistd.h>
//#include<iomanip>
#include<cstdlib>
#include "mpi.h"

using namespace std;

int main(int argc,char *argv[])
{
        int index;
        int numprocs,myrank;
        int newnumprocs,newmyrank;
        int *process_rank,diag;

        MPI::Group   groupworld,new_group;
        MPI::Intracomm    group_diag;

  /* ....MPI Initialisation....*/

        MPI::Init(argc, argv);
        numprocs = MPI::COMM_WORLD.Get_size();
        myrank = MPI::COMM_WORLD.Get_rank();

 /* Calculating no. of procs in new Comm..*/

        diag = (int)sqrt((float)numprocs);

/* Validation if no of procs in old comm is perfect square...*/  

       if(diag*diag != numprocs) 
         {
           if(myrank == 0)
             cout<<"\n There should be  square number of processors."<<endl;
           MPI::Finalize();
           exit(-1);
         }

     process_rank = new int[diag];
  
   /* Finding the ranks  for  procs in new comm...*/ 

     for(index = 0 ; index < diag ; index++)
      { 
           process_rank[index] = index * diag + index;

         }  
         if(myrank == 0)
         {
           cout<<"\nthe Processors forming the diagonal group are "<<endl;
            for(index = 0 ; index < diag ; index++)
            cout<<"Processor"<<process_rank[index]<<endl;
           }
  
           groupworld = MPI::COMM_WORLD.Get_group();
           new_group = groupworld.Incl(diag,process_rank);
           group_diag = MPI::COMM_WORLD.Create(new_group);   
     
          if(myrank % (diag + 1) == 0)
           {
             newmyrank = group_diag.Get_rank();
             newnumprocs = group_diag.Get_size();   
             cout<<"\n Numberof Procs in New Comm = "<<newnumprocs<<endl;
             cout<<"\n NewRank in the diagonal group formed ="<<newmyrank<<endl;
             }
	delete(process_rank);
     /*........Finalizing MPI.......*/
        MPI_Finalize();
        return 0;        
}





