/*
**************************************************************************

   Example 2.5      : mpicpp-vv-mult-blkstp-nonunf.C
   
   Objective        : MPI program to compute dot product of two vectors using 
                      block-striped partitioning with non-uniform data distribution 
   
   Input            : Process with rank 0 reads a real vectors of size n. 
   
   Output	    : Process with rank 0 prints the final dot product of 
                      two vectors.
   
  Created     	    : Aug 2011    

   E-mail      	    : RarchK                                          	

**************************************************************************
*/  
#include<iostream>
#include<cstdlib>
#include<iomanip>
#include<unistd.h>
#include"mpi.h"
using namespace std;
   
int main(int argc,char *argv[]) 
{     
        int root=0,myrank,numprocs;
        int i,j,x,recv_cnt;
        int vsize=90;
        int *vector_a,*vector_b,*buffer_a,*buffer_b;
        int *displ_vectr,*cnt_status;
        int sum=0,finalvalue=0;      
        int distribute_col,left_cols;   
        int destn_tag=0,source_tag=0;   
         
        MPI::Status status;
        MPI::Init(argc,argv);
        numprocs=MPI::COMM_WORLD.Get_size();
        myrank=MPI::COMM_WORLD.Get_rank();
	
	if(vsize<numprocs)
           {
             if(myrank == root)
                cout<<"\nVector size is less than No. of Procs.."<<endl;
             MPI::Finalize();
             exit(-1);
           }
	
        /* Creation of space to hold data...*/   
        if(myrank==root)
        {
           vector_a = new int[vsize];
           vector_b = new int[vsize];
           for(i=0;i<vsize;i++)
           {
              vector_a[i]=i+1;
              vector_b[i]=vsize+(i+1);
            }
        }
         /* Broadcast the datasize */  
         MPI::COMM_WORLD.Bcast(&vsize,1,MPI::INT,0);
          
          /* Calculate the distributed colums and left out columns */ 
           if(myrank==0)
             {
               displ_vectr = new int[numprocs];
               cnt_status= new int[numprocs];
               
               distribute_col=vsize/numprocs;
               left_cols=vsize%numprocs;
               
               for(i=0;i<numprocs;i++)
                 cnt_status[i]=distribute_col;
               for(i=0;i<left_cols;i++)
                 cnt_status[i]=cnt_status[i]+1;
              
     /* Caluclating the addresses for scattering i.e displacement values....*/
             displ_vectr[0]=0;
              for(i=1;i<numprocs;i++)
               { 
                x=0;
                for(j=0;j<i;j++)
                   {
                     x=x+cnt_status[j];
                     }
              displ_vectr[i]=x;
               }  
           /* send the count of data to each proc......*/
                                 
            for(i=1;i<numprocs;i++)
             {
             recv_cnt=cnt_status[i];
             MPI::COMM_WORLD.Send(&recv_cnt,1,MPI::INT,i,destn_tag);
             }
                recv_cnt=cnt_status[0];
            }
     if(myrank!=root)
         MPI::COMM_WORLD.Recv(&recv_cnt,1,MPI::INT,0,source_tag,status);   
          
          /* create buffer to hold elements at each proc....*/
          buffer_a=(int*)malloc(recv_cnt*sizeof(int));
          buffer_b=(int*)malloc(recv_cnt*sizeof(int));

             /* Distribute the data using Scatterv call....*/ 
           
            MPI::COMM_WORLD.Scatterv(vector_a,cnt_status,displ_vectr,MPI::INT,buffer_a,recv_cnt,MPI::INT,0);       
            MPI::COMM_WORLD.Scatterv(vector_b,cnt_status,displ_vectr,MPI::INT,buffer_b,recv_cnt,MPI::INT,0);       
         
          /* calculation part.....*/
       for(i=0;i<recv_cnt;i++)
     sum=sum+(buffer_a[i]*buffer_b[i]);
     
    /* Gather the output......*/
          
          MPI::COMM_WORLD.Reduce(&sum,&finalvalue,1,MPI::INT,MPI_SUM,0);
    /* Display the Result...*/
       if(myrank==0)
       {	
           cout<<"\nProduct of vector A and Vector B ::>" <<finalvalue<<"\n";
   	   /* Free the memory .......*/
           free(cnt_status);
           free(displ_vectr);
           free(vector_a);   
          free(vector_b);
	}
        free(buffer_a);   
        free(buffer_b);   
 
         MPI::Finalize();
          return 0;
  }          
