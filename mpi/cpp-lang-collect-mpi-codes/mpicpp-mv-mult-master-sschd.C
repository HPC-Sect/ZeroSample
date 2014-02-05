/*
***************************************************************************

   Example 2.7      : mpicpp-mv-mult-master-sschd.C
   
   Objective        : MPI program to compute dot product of matrix-vetor using 
                      self-scheduling algritham 
   
   Input            : Simple square matrix input file
   
   Output	    : Process with rank 0 prints the final matrix vector product
   
  Created     	    : Aug 2011    

   E-mail      	    : RarchK                                          	

*****************************************************************************
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
        int destination,destination_tag,source_tag;
        int **matrix,*vector;
        int *buffer,*finalvector;
        int sum=0,finalvalue=0;      
        int row_size,col_size,vsize;
        int rowtag=0; 
        FILE *fp;   
        MPI::Status status; 
 
         MPI::Init(argc,argv);
         numprocs=MPI::COMM_WORLD.Get_size();
         myrank=MPI::COMM_WORLD.Get_rank();
        
	cout<<numprocs<<endl; 
       /* Validations...*/ 
           if(numprocs<1)
           {
             cout<<" \n Less than 2 procs...So aborting ........";
             MPI::Finalize();
             exit(-1);
            }/* end of if...*/
          
         /* Opening input file and reading the input data...*/
 
           fp=fopen("mvdata.inp","r");
           if(fp==NULL)
            {
             cout<<"\nInput Data File not Found. So terminating......";
             MPI::Finalize();
             exit(-1);
            } /*end of  validation of file opening..*/ 
            fscanf(fp,"%d",&row_size);      
            fscanf(fp,"%d",&col_size);      
            fscanf(fp,"%d\n",&vsize);      
          /* Allocation of Memory to hold input....*/  
          
           matrix= new int*[row_size];
           for(i=0;i<row_size;i++)     
           matrix[i]= new int[col_size];


           vector= new int[vsize];
          if(matrix==NULL)
           {
             cout<<" \nUnable to Allocate memory to hold Data..";
             MPI::Finalize();
             exit(-1);
           }
          if(vector==NULL)
           {
             cout<<" \nUnable to Allocate memory to hold Data..";
             MPI::Finalize();
             exit(-1);
           }

      /* reading data frm file....*/   
            /* Reading Matrix values form File...*/
          for(i=0;i<row_size;i++)
           for(j=0;j<col_size;j++)  
             fscanf(fp,"%d",&matrix[i][j]);
         for(i=0;i<vsize;i++)
             fscanf(fp,"%d",&vector[i]);
            fclose(fp); 
      /* Validations....*/  
          if(vsize!=col_size)
           {
           cout<<"\nMatrix-Vector multiplication not possible... so terminating....."; 
             MPI::Finalize();
             exit(-1);
           }
           
          if(vsize%numprocs!=0)
           {
             cout<<"\nLeads to Unequal Distribution of Data...";
             MPI::Finalize();
             exit(-1);
           }

       MPI::COMM_WORLD.Barrier();  
       MPI::COMM_WORLD.Bcast(&vsize,1,MPI::INT,0);
       MPI::COMM_WORLD.Bcast(vector,vsize,MPI::INT,0);
                
       buffer=new int[col_size];
       finalvector= new int[row_size];
       
      /* Send each row to slave......*/
     
         for(i=1;i<numprocs;i++) /* rows....*/
         {  
          for(j=0;j<col_size;j++) /* Column wise..*/
             buffer[j]=matrix[i-1][j];
           MPI::COMM_WORLD.Send(buffer,col_size,MPI::INT,i,rowtag+1);
           rowtag++;
          }         
                 
         /* get the partial sums form the slaves..*/
         for(i=0;i<row_size;i++)
          {
  MPI::COMM_WORLD.Recv(&sum,1,MPI::INT,MPI::ANY_SOURCE,MPI::ANY_TAG,status);
           destination=status.Get_source();
           source_tag=status.Get_tag(); 
           finalvector[source_tag]=sum;
            if(rowtag<row_size)
               { destination_tag=rowtag+1;
                 for(i=0;i<col_size;i++)
                  buffer[i]=matrix[rowtag][i];
 MPI::COMM_WORLD.Send(buffer,col_size,MPI::INT,destination,destination_tag);           
            rowtag++;      }
          }
        for(i=1;i<numprocs;i++)
          MPI::COMM_WORLD.Send(buffer,col_size,MPI::INT,i,0);
          
        for(i=0;i<row_size;i++)
           cout<<"\n Final answer["<<i<<"]= "<<finalvector[i];
         
        delete(finalvector);
        delete(matrix);
        delete(vector);            
        delete(buffer);  
       MPI::Finalize();
          return 0;
  }          
