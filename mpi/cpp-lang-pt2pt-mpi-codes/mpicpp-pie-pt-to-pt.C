   
/*
**********************************************************************    

     Example 1.7  : mpicpp-pie-pt-to-pt.C 
           
     Objective    : To compute the value of PI by numerical integration
                    using MPI point to point communication library call

     Input        : Number of intervals
      
     Output       : calculated value of PI
  
     Condition    : Number of processes should be less
                    than or equal to 8.
	     
  Created     	    : Aug 2011    

     E-mail       : RarchK                                          

**********************************************************************
*/
 
   #include<iostream>
   #include<stdio.h>
   #include<unistd.h>
   #include<cstdlib>
   #include"mpi.h"
   
   using namespace std;
   
   int main(int argc,char *argv[])
   {
     int root = 0,myrank,numprocs,index;
     int no_interval;
     int destination,source;
     int dest_tag,source_tag;
     int iproc,interval;
     double my_pi,pi = 0.0,sum = 0.0,x = 0.0,h;
    
     MPI::Status status;
     
   /* ..... MPI Intializing ......*/
 
     MPI::Init(argc,argv);
     numprocs=MPI::COMM_WORLD.Get_size();
     myrank=MPI::COMM_WORLD.Get_rank();
     
     if(myrank == root)
      {
       cout<<" \n Enter number of Intervals ::> ";
       cin>>no_interval;
  
          //if(no_interval <= 0)
          //{
            //c/out<<" \n Invalid Intervals :: so Terminating process .....\n"; 
            //MPI::Finalize();
            //exit(-1);
           // }

        /* Send no. of intervals to all...  */
           for(iproc = 1 ; iproc < numprocs ; iproc++)
           {
            destination = iproc;
            dest_tag = 0;
            MPI::COMM_WORLD.Send(&no_interval,1,MPI::INT,destination,dest_tag); 
             }
        }
     else
       {
           source = root;
           source_tag = 0; 
         MPI::COMM_WORLD.Recv(&no_interval,1,MPI::INT,source,source_tag,status);          
        }          
          
       h = 1.0/(double)no_interval;
       sum = 0.0;
       for(interval = myrank + 1; interval < no_interval;interval += numprocs)
        {
          x=h * ((double)interval - 0.5);
          sum = sum + (4.0 / (1.0 + x * x));
        }
        my_pi = h * sum;            
   
      /* Collect the pi values .....*/
 
       if(myrank == root)
       {
         pi = pi + my_pi;
         for(iproc = 1;iproc < numprocs;iproc++)
          {
           source     = iproc;
           source_tag = 0;
           MPI::COMM_WORLD.Recv(&my_pi,1,MPI::DOUBLE,source,source_tag,status);
           pi = pi + my_pi;
           }
         }  
       else
        {
           destination   = root;
           dest_tag      = 0;
           MPI::COMM_WORLD.Send(&my_pi,1,MPI::DOUBLE,destination,dest_tag);
           }
 
      if(myrank == root)
   cout<<" \nValue of Pi with intervals " <<no_interval <<" is ::> " <<pi<<"\n";


    /*.......Fianlizing MPI.......*/ 
     MPI::Finalize();
     return 0;
  }
 
