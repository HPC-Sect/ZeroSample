  
  /*
  **************************************************************************                        

   Example 1.1  : mpicpp-hello-world.C
   
   Objective  	: Simple MPI C++ program(SPMD)to print "Hello World"
    
   Input	: Message = "Hello World"
   
   Output	: Message and Rank of the process.
    
   Condition	: Number of processes should be
                  less than or equal to 8.
 
  Created     	    : Aug 2011    

   E-mail      	: RarchK                                          

  ****************************************************************************
  */

 /*........Standard Includes........*/
     
  #include<iostream>
  #include<string.h>
  #include<unistd.h>
  #include"mpi.h"

  using namespace std;
  #define buffer_size 12 
 
  int main(int argc,char *argv[]) 
   {
    int root = 0,myrank,numprocs,source,destination,iproc;
    int destination_tag,source_tag;
    char message[buffer_size];
    MPI::Status status;
     
     /*.......MPI Intialization........*/
   
     MPI::Init(argc,argv);
     numprocs = MPI::COMM_WORLD.Get_size();
     myrank = MPI::COMM_WORLD.Get_rank();

     if(myrank == 0)
     {
      for(iproc = 1 ;iproc < numprocs ; iproc++)
       {
           source = iproc;
           source_tag = 0;
          MPI::COMM_WORLD.Recv(message,buffer_size,MPI::CHAR,source,source_tag);
          cout<<"\n";
          cout<<message<<"from Process "<<iproc;
          cout<<"\n";
        }                   /* End of for loop */  
      }                     /* End of if Block*/
    else
     {
      strcpy(message,"Hello World");
       destination = root;
       destination_tag = 0;
 MPI::COMM_WORLD.Send(message,buffer_size,MPI_CHAR,destination,destination_tag);
       }                    /*end of else block*/
  
       /* ...... Finalizing MPI ....*/
  
          MPI::Finalize();
          return 0;
      }                    /* End of main...*/
                





 
