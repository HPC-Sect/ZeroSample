/*
*******************************************************************************

   Example 2.4      : mpicpp-vv-mult-blkstp-unf.C
   
   Objective        : MPI program to compute dot product of two vectors using 
                      block-striped partitioning with uniform data distribution 
   
   Input            : Process with rank 0 reads a real vectors of size n. 
   
   Output	    : Process with rank 0 prints the final dot product of two vectors.
   
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
     
        int root = 0,myrank,numprocs,index;
        int no_interval;
        int vsize = 100;
        int scatsize;
        int *vector_a,*vector_b,*buffer_a,*buffer_b;
        int sum = 0,finalvalue = 0,i;      
 
         MPI::Init(argc,argv);
         numprocs = MPI::COMM_WORLD.Get_size();
         myrank = MPI::COMM_WORLD.Get_rank();
         
        if(myrank == root)
         {
           vector_a = new int[vsize];
           vector_b = new int[vsize];
           for(i = 0;i < vsize;i++)
           {
              vector_a[i] = i+1;
              vector_b[i] = vsize+(i+1);
            }
          }
          
         MPI::COMM_WORLD.Bcast(&vsize,1,MPI::INT,0);
          
          if(vsize < numprocs)
           {
	     if(myrank == 0)
             	cout<<"\nVector size is less than No. of Procs.."<<endl; 
             MPI::Finalize();
             exit(-1);
           }
           
          if(vsize % numprocs != 0)
           {
	     if(myrank == 0)
             	cout<<"\nLeads to Unequal Distribution of Data..."<<endl;
             MPI::Finalize();
             exit(-1);
           }
           scatsize = vsize/numprocs;
           buffer_a = new int[scatsize];
           buffer_b = new int[scatsize];
    MPI::COMM_WORLD.Scatter(vector_a,scatsize,MPI::INT,buffer_a,scatsize,MPI::INT,0);
    MPI::COMM_WORLD.Scatter(vector_b,scatsize,MPI::INT,buffer_b,scatsize,MPI::INT,0);
         for(i = 0;i<scatsize;i++)      
           sum = sum+(buffer_a[i] * buffer_b[i]);
      MPI::COMM_WORLD.Reduce(&sum,&finalvalue,1,MPI::INT,MPI_SUM,0);
        if(myrank == 0)
	{
           cout<<"\nProduct of vector A and Vector B ::>" <<finalvalue<<"\n";
           delete(vector_a);   
           delete(vector_b);
	}
         delete(buffer_a);   
         delete(buffer_b);   
    /* Finalizing MPI */
         MPI::Finalize();
          return 0;
  }          
