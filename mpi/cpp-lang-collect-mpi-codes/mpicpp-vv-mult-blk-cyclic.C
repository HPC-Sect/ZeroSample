/*
*******************************************************************************

   Example 2.6      : mpicpp-vv-mult-blk-cyclic.C
   
   Objective        : MPI program to compute dot product of two vectors using 
                      block-striped cyclic distribution of data
   
   Input            : Process with rank 0 reads a real vectors of size n. 
   
   Output	    : Process with rank 0 prints the final dot product of two vectors.
   
   Necessary 
   Conditions       : Number of processors should be divisible by vector size. 

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
     
        int root=0,myrank,numprocs,i,j,k=0;
        int vsize;
        int scatsize;
        int *vector_a,*vector_b,*buffer_a,*buffer_b;
        int sum=0,finalvalue=0;      
        int *temp_a,*temp_b;
        FILE *fp;   
        
 
         MPI::Init(argc,argv);
         numprocs=MPI::COMM_WORLD.Get_size();
         myrank=MPI::COMM_WORLD.Get_rank();
         
        if(myrank==root)
         {
           fp=fopen("mvdata.inp","r");
           if(fp==NULL)
            {
             cout<<"\nInput Data File not Found. So terminating......";
             MPI::Finalize();
             exit(-1);
            }
            fscanf(fp,"%d\n",&vsize);      
          /* Allocation of Memory to hold input....*/  
           vector_a= new int[vsize];
           vector_b= new int[vsize];
       
      /* reading data frm file....*/   
         for(i=0;i<vsize;i++)
             fscanf(fp,"%d",&vector_a[i]);
         for(i=0;i<vsize;i++)
             fscanf(fp,"%d",&vector_b[i]);
            fclose(fp); 
          }
           
       MPI::COMM_WORLD.Barrier();  
       MPI::COMM_WORLD.Bcast(&vsize,1,MPI::INT,0);
      /* Validations....*/  
          if(vsize<numprocs)
           {
	    if(myrank == 0)
             	cout<<"\nVector size is less than No. of Procs.."<<endl; 
             MPI::Finalize();
             exit(-1);
           }
           
          if(vsize%numprocs!=0)
           {
	     if(myrank == 0)
             	cout<<"\nLeads to Unequal Distribution of Data..."<<endl;
             MPI::Finalize();
             exit(-1);
           }
          /* Rearanging the Data i.e for yclic distribution...*/
           if(myrank==0)
           {
             temp_a = new int[vsize];      
             temp_b = new int[vsize];   
                    
             for(i=0;i<numprocs;i++)
              for(j=i;j<vsize;j=j+numprocs)
                   {
                    temp_a[k]=vector_a[j];     
                    temp_b[k]=vector_b[j];    
                    k++; 
                   }
            }   
  
         /* Scattering the Data....*/  
          scatsize=vsize/numprocs;
           buffer_a = new int[scatsize];
           buffer_b = new int[scatsize];
    MPI::COMM_WORLD.Scatter(temp_a,scatsize,MPI::INT,buffer_a,scatsize,MPI::INT,0);
    MPI::COMM_WORLD.Scatter(temp_b,scatsize,MPI::INT,buffer_b,scatsize,MPI::INT,0);
         for(i=0;i<scatsize;i++)      
           sum=sum+(buffer_a[i]*buffer_b[i]);
      MPI::COMM_WORLD.Reduce(&sum,&finalvalue,1,MPI::INT,MPI_SUM,0);
        if(myrank==0)
	{
        	cout<<"\nProduct of vector A and Vector B ::>" <<finalvalue<<"\n";
         	delete(vector_a);   
         	delete(vector_b);
         	delete(temp_a); 
         	delete(temp_b);
	}
         delete(buffer_a);   
         delete(buffer_b);   
         MPI::Finalize();
          return 0;
  }          
