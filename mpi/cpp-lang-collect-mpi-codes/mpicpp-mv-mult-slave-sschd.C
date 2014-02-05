/*
*******************************************************************************

   Example 2.7      : mpicpp-mv-mult-slave-sschd.C
   
   Objective        : MPI program to compute dot product of matrix-vetor using 
                      self-scheduling algritham 
   
   Input            : Simple square matrix input file
   
   Output	    : Process with rank 0 prints the final matrix vector product
   
  Created     	    : Aug 2011    

   E-mail      	    : RarchK                                          	

*******************************************************************************
*/

     #include<iostream>
     #include<cstdlib>
     #include<iomanip>
     #include<unistd.h>
     #include"mpi.h"
     using namespace std;
   
    int main(int argc,char *argv[]) 
    {
     
        int root=0,myrank,numprocs,i;
        int flag;
        int vsize;
        int *vector,*buffer;
        int sum=0,finalvalue=0;     
        MPI::Status status;  
        int destination,destination_tag;  
         MPI::Init(argc,argv);
         numprocs=MPI::COMM_WORLD.Get_size();
         myrank=MPI::COMM_WORLD.Get_rank();
         
         
         if(numprocs<1)
          {
           cout<<"\n Cannont run the program....";
           MPI::Finalize();
           exit(-1);
          }
          MPI::COMM_WORLD.Barrier();
          MPI::COMM_WORLD.Bcast(&vsize,1,MPI::INT,0);
          vector=(int *)malloc(vsize*sizeof(int));
          MPI::COMM_WORLD.Bcast(vector,vsize,MPI::INT,0);
          
          buffer=(int *)malloc(vsize*sizeof(int));
          for(;;)
         {
          MPI::COMM_WORLD.Recv(buffer,vsize,MPI::INT,0,MPI::ANY_TAG,status);     
         flag=MPI::ANY_TAG-1;
         if(flag==-1)
         break;
         destination=0;
         destination_tag=flag;
         sum=0;
         for(i=0;i<vsize;i++)
           sum=sum+(buffer[i]*vector[i]);
                     
         MPI::COMM_WORLD.Send(&sum,1,MPI::INT,0,flag);
         }
        free(vector);  
        free(buffer);         
  
         MPI::Finalize();
          return 0;
  }          
