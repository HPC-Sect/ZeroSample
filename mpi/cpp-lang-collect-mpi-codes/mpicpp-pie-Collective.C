/*
*****************************************************************************

     Example 2.3        : mpicpp-pie-Collective.C      

     Objective          : To compute the value of PI by numerical integration
                            MPI collective communication library calls are used
    
     Input              : Number of intervals

     Output             : Calculated PI value.
 
     Condition          : Number of processes should be less or than equal 
                          to 8

     Created     	    : Aug 2011    

         E-mail      	: RarchK                                          	

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
     
        int root = 0,myrank,numprocs;
        int no_interval,interval;
        double x,h,sum,pi;
        double my_pi; /* Pi value at every processor */

     /*........Intializing MPI .......*/ 
        MPI::Init(argc,argv);
        numprocs  = MPI::COMM_WORLD.Get_size();
        myrank    = MPI::COMM_WORLD.Get_rank();

	if(numprocs > 8)
	{
		if(myrank == 0)
			cout<<"Number of processors should be less than or equal to 8"<<endl;
		MPI::Finalize();
		exit(-1);
	}
         if(myrank == root)
         {
           cout<<"enter the intervals"<<"\t";
           cin>>no_interval;
           }
      /*.. Broad casting the number of sub-intervals to each processor...*/

        MPI::COMM_WORLD.Bcast(&no_interval,1,MPI::INT,root);

        if(no_interval == 0)
         {
               cout<<"cannot compute as there are no intervals";
               MPI::Finalize();
                     exit(-1);
            }

           h = 1.0 / (double)no_interval;
          sum = 0.0;
         for(interval = myrank + 1;interval <= no_interval;interval += numprocs)
         {
            x = h * ((double)interval - 0.5);
            sum = sum + (4.0 / (1.0 + x * x));
            }
            my_pi = h * sum;

      /*.... collecting the calculated pi's at root ie,P0.......*/
          MPI::COMM_WORLD.Reduce(&my_pi,&pi,1,MPI::DOUBLE,MPI_SUM,root);
       
      if(myrank == root)
          cout<<"\n pi value with " <<no_interval <<" intervals :"<<pi<<"\n";

     /*..... Finalizing MPI .......*/
          MPI::Finalize();
          return 0;
  }          
