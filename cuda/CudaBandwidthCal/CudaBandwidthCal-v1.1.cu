/*********************************************************************************************	
	
	Example : CudaBandwidthCal-v1.1.cu

	Objective: To calculate the bandwidth for pagebale/pinned memory from Host-to Device 
		   and Device-to-Host for a given range in MB/s.

	Input : Memory Mode; 0 - pageable , 1 - pinned
		Start data size in bytes
		End data size in bytes
		Increment value in bytes

	Output : Bandwidth in MB/s for a given data size

	Created : Aug 2011 

	Email :  RarchK
***********************************************************************************************/


#include<stdio.h>
#include<cuda.h>
#include<stdlib.h>				// header file inclusion
#include<assert.h>
#include<string.h>


#define DEFAULT_START 1024
#define DEFAULT_END 1324                        // Default values for data size
#define DEFAULT_INCR 100

unsigned long long  start_datasize_bytes,end_datasize_bytes,increment_by_bytes;
int memMode;
char *memModeStr ;

float bandwidth_calc( long int ,float );		// function declaration 
void check_cmdline_arg(int ,char **);


int main(int argc, char* argv[])
{
	float *a_h, *b_h; 	 // host data
        float *a_d , *b_d;		 // device data

	unsigned long long  N,nBytes ,memlimit; 
	int *arr_data;

	unsigned long long  i = 0;

	cudaDeviceProp prop;		// device related variables
	
	cudaEvent_t  start_htod, stop_htod;               // event variables for timings
	cudaEvent_t  start_dtoh, stop_dtoh;
	cudaEvent_t  start_dtod ,stop_dtod;
 	
	float elapsedTimeInMilliSeconds_htod;            // timing related variables
        float executionTimeInSeconds_htod;
	float elapsedTimeInMilliSeconds_dtoh;
        float executionTimeInSeconds_dtoh;
	float elapsedTimeInMilliSeconds_dtod;
        float executionTimeInSeconds_dtod;

	int  totalDevice;

	float *HtD_BW,*DtH_BW ,*DtD_BW;		// bandwidth related variabls

	int dev = 0;
	int j,k,count,check;


	check_cmdline_arg(argc,argv);				// function to check command line arguments


	if((cudaGetDeviceCount(&totalDevice)))			
        {
                printf("\n\t Error : There is no device available on the system...Aborting \n");	// Get number of available devices on system
                exit(-1);
        }

	 cudaGetDeviceProperties(&prop,dev);					// Get device properties

	printf("\n Running on device %s\n",prop.name);
	printf("\n Total available global memory on device = %llu bytes",prop.totalGlobalMem);
	printf("\n Start = %llu bytes , End = %llu bytes \n",start_datasize_bytes , end_datasize_bytes);


	/***************************************** Memory Check starts******************************************************/

	if(prop.totalGlobalMem >= 1073741824)
		memlimit = prop.totalGlobalMem-536870912 ;
	else
		 memlimit = prop.totalGlobalMem-104857600 ;

	//printf("\n Memory Limit = %llu bytes\n",memlimit);

	if(start_datasize_bytes > end_datasize_bytes)
	{
		printf("\n Invalid range value...start is greater than end......aborting...\n\n");
		exit(-1);
	}

	if((start_datasize_bytes*2) >= memlimit)
	{
		 printf("\n Entered start data size is exceeding global memory limit on device...aborting....\n");
                 exit(-1);
	}

	if((end_datasize_bytes*2) >= memlimit)
        {
                 printf("\n Entered end data size is exceeding global memory limit on device...aborting....\n");
                 exit(-1);
        }

	
	if(increment_by_bytes>= end_datasize_bytes)
	{
		printf("\n************************************Important Note**********************************************************\n");
		printf("\n Increment value is exceeding the end value limit...so increment value = start value\n");
		increment_by_bytes = start_datasize_bytes ;
		printf("\n*************************************************************************************************************\n");
	}


	if(((end_datasize_bytes - start_datasize_bytes)%increment_by_bytes)!=0)
	{
		printf("\n************************************Important Note**********************************************************\n");
		printf("\n The range values you have entered are not exactly divisible so end value will be choosen accordingly within a limit\n");
		printf("\n*************************************************************************************************************\n");
	}

       
	/*******************************************Memory Checks ends*******************************************************************/
	
	//printf("\n CUDA Bandwidth Test Running For memMode = %s \n",memModeStr);
	printf("\nRange(in bytes)-> From = %llu bytes\tTo = %llu bytes\t Increment by = %llu bytes\t\n",start_datasize_bytes,end_datasize_bytes,increment_by_bytes);
	
	check = 1 + ((end_datasize_bytes - start_datasize_bytes)/increment_by_bytes );


	assert(HtD_BW = (float *)malloc(check*sizeof(float)));        // arrays to hold bandwidths
	assert(DtH_BW = (float *)malloc(check*sizeof(float)));                    
	assert(DtD_BW = (float *)malloc(check*sizeof(float)));                    
	assert(arr_data = (int *)malloc(check * sizeof(int)));

	for(i=0;i<check;i++)
	{
		HtD_BW[i]= 0.0;
		DtH_BW[i]= 0.0;
		DtD_BW[i]= 0.0;
		arr_data[i]= 0;
	}
	

	count = 0;
	for(i=start_datasize_bytes;i<= end_datasize_bytes ; i=i+increment_by_bytes)
	{
		start_datasize_bytes = i;
		N = (start_datasize_bytes / sizeof(float));
		arr_data[count]= start_datasize_bytes;
	
		nBytes = N*sizeof(float);

		if(strcmp(memModeStr,"pageable")==0)
		{
			assert(a_h = (float *)malloc(nBytes));			// allocate pageable memory on host
			assert(b_h = (float *)malloc(nBytes));
		}

		else if(strcmp(memModeStr,"pinned") == 0)                   // allocate pinned memory
		{

			if(cudaMallocHost((void **) &a_h, nBytes) == cudaErrorMemoryAllocation)     
                	{
                        	printf(" \n\t Error :Memory Allocation Failed for pinned memory a_h \n");
                        	exit(-1);
                	}
                	if(cudaMallocHost((void **) &b_h, nBytes) == cudaErrorMemoryAllocation)     
                	{
                        	printf(" \n\t Error :Memory Allocation Failed for pinned memory b_h \n");
                        	exit(-1);
                	}


		}

		if(cudaMalloc((void **) &a_d, nBytes) == cudaErrorMemoryAllocation)	// allocate memory on device
 		{
                	printf(" \n\t Error :Memory Allocation on Device Failed \n");
                	exit(-1);
        	}
		if(cudaMalloc((void **) &b_d, nBytes) == cudaErrorMemoryAllocation)	// allocate memory on device
 		{
                	printf(" \n\t Error :Memory Allocation on Device Failed \n");
                	exit(-1);
        	}


		for (k=0; k<N; k++) 
			a_h[k] = 100.00 + k;
		
	
		/* Creating the Events */
        	cudaEventCreate (&start_htod);
        	cudaEventCreate (&stop_htod);

		elapsedTimeInMilliSeconds_htod=0.0f;
        	executionTimeInSeconds_htod=0.0f;

		
		/************* copy data from host to device***************************/
		
		cudaEventRecord (start_htod, 0);
		if(cudaMemcpy(a_d, a_h, nBytes, cudaMemcpyHostToDevice)!=cudaSuccess)
		{
			printf("\n Error in copying data from host to device !!\n");
			exit(-1);
		}
		cudaEventRecord (stop_htod, 0);
		cudaEventSynchronize (stop_htod);
		
		cudaEventElapsedTime ( &elapsedTimeInMilliSeconds_htod, start_htod, stop_htod);
                executionTimeInSeconds_htod = float (elapsedTimeInMilliSeconds_htod * 1.0e-3);	

		//printf("\nHost to device excution time in seconds = %f\n",executionTimeInSeconds_htod);	
		
		cudaEventDestroy(start_htod);
                cudaEventDestroy(stop_htod);

		/********************* copy data from device to device******************/		

		cudaEventCreate (&start_dtod);
                cudaEventCreate (&stop_dtod);
		
		elapsedTimeInMilliSeconds_dtod=0.0f;
                executionTimeInSeconds_dtod=0.0f;

		cudaEventRecord (start_dtod, 0);
                if(cudaMemcpy(b_d, a_d, nBytes, cudaMemcpyDeviceToDevice)!=cudaSuccess)
                {
                        printf("\n Error in copying data from device to device !!\n");
                        exit(-1);
                }
                cudaEventRecord (stop_dtod, 0);
                cudaEventSynchronize (stop_dtod);

		cudaEventElapsedTime ( &elapsedTimeInMilliSeconds_dtod, start_dtod, stop_dtod);
                executionTimeInSeconds_dtod = float (elapsedTimeInMilliSeconds_dtod * 1.0e-3);

		cudaEventDestroy(start_dtod);
                cudaEventDestroy(stop_dtod);

		/****************** copy data from device to host*********************/

		cudaEventCreate (&start_dtoh);
                cudaEventCreate (&stop_dtoh);

		elapsedTimeInMilliSeconds_dtoh=0.0f;
                executionTimeInSeconds_dtoh=0.0f;

		cudaEventRecord (start_dtoh, 0);
		//if(cudaMemcpy(b_h, b_d, nBytes, cudaMemcpyDeviceToHost)!= cudaSuccess)
		if(cudaMemcpy(b_h, a_d, nBytes, cudaMemcpyDeviceToHost)!= cudaSuccess)
		{
			printf("\n Error in copying data from  device to host !!\n");
                       exit(-1);
		}
		cudaEventRecord (stop_dtoh, 0);
		cudaEventSynchronize (stop_dtoh);

		cudaEventElapsedTime ( &elapsedTimeInMilliSeconds_dtoh, start_dtoh, stop_dtoh);
        	executionTimeInSeconds_dtoh = float (elapsedTimeInMilliSeconds_dtoh * 1.0e-3);
	
		//printf("\nDevice to Host excution time in seconds = %f\n",executionTimeInSeconds_dtoh);		

		cudaEventDestroy(start_dtoh);
                cudaEventDestroy(stop_dtoh);

		for (j=0; j< N; j++) 			// check for correctness of result
			assert( a_h[j] == b_h[j] );
	
		

		// call function to calculate bandwidth
		HtD_BW[count] = bandwidth_calc(start_datasize_bytes,executionTimeInSeconds_htod);
		DtD_BW[count] = 2.0 * bandwidth_calc(start_datasize_bytes,executionTimeInSeconds_dtod);
		DtH_BW[count] = bandwidth_calc(start_datasize_bytes,executionTimeInSeconds_dtoh);
		count++;
	
		
		if(strcmp(memModeStr,"pageable")==0)		
		{
			free(a_h);
                	free(b_h);
		}
		else if(strcmp(memModeStr,"pinned") == 0)
		{
			cudaFreeHost(a_h);
			cudaFreeHost(b_h);
		}
		
                cudaFree(a_d);
 		cudaFree(b_d);
		

	}

		printf("\n\n------------------------------------------------------------------------------------------------------------\n");
		printf("\n \t\t\t BandWidth Calculation Table for %s Memory Allocation in MB/s \n",memModeStr);

		
		printf("\n Data Size(bytes) \t  Bandwidth(Host-to-Device) \t Bandwidth (Device-to-Device)   \t  Bandwidht(Device-to-Host)  \t\n");
		for(k=0;k<check;k++)
                        printf("%d\t\t\t\t%.2f             \t\t%.2f        \t\t\t %.2f  \n",arr_data[k],HtD_BW[k], DtD_BW[k],DtH_BW[k]);


		printf("\n\n------------------------------------------------------------------------------------------------------------\n");


		free(arr_data);
		free(HtD_BW);
		free(DtD_BW);
		free(DtH_BW);

		return 0;
}


// function to check command line arguments

void check_cmdline_arg(int argc,char* argv[])			
{
	switch(argc)
	{
		case 1:
			start_datasize_bytes = DEFAULT_START;
                	end_datasize_bytes = DEFAULT_END;
                	increment_by_bytes = DEFAULT_INCR;
			memModeStr = "pageable";
			break;
		case 2 :
			memMode = atoi(argv[1]);
                        if(memMode == 0)
                                 memModeStr = "pageable";
                        else if(memMode == 1)
                                memModeStr = "pinned";
                        else
                        {
                                printf("\n Invalid Memory Mode.Press 0 for pageable and 1 for pinned memory usage..Aborting\n");
                                exit(0);
                        }
			start_datasize_bytes = DEFAULT_START;
                        end_datasize_bytes = DEFAULT_END;
                        increment_by_bytes = DEFAULT_INCR;
			break;

		case 3 :
			memMode = atoi(argv[1]);
			if(memMode == 0)
				 memModeStr = "pageable";
			else if(memMode == 1)
				memModeStr = "pinned";
			else
			{
				printf("\n Invalid Memory Mode.Press 0 for pageable and 1 for pinned memory usage..Aborting\n");
				exit(0);
			}
			start_datasize_bytes =  atol(argv[2]);
			end_datasize_bytes = start_datasize_bytes + 512;
                        increment_by_bytes = DEFAULT_INCR;
                        break;
		case 4 :
			memMode = atoi(argv[1]);
                        if(memMode == 0)
                                 memModeStr = "pageable";
                        else if(memMode == 1)
                                memModeStr = "pinned";
                        else
                        {
                                printf("\n Invalid Memory Mode.Press 0 for pageable and 1 for pinned memory usage..Aborting\n");
                                exit(0);
                        }

			start_datasize_bytes = atol(argv[2]);
                        end_datasize_bytes =  atol(argv[3]);
			increment_by_bytes = DEFAULT_INCR;
                        break;
		case 5 :
			memMode = atoi(argv[1]);
                        if(memMode == 0)
                                 memModeStr = "pageable";
                        else if(memMode == 1)
                                memModeStr = "pinned";
                        else
                        {
                                printf("\n Invalid Memory Mode.Press 0 for pageable and 1 for pinned memory usage..Aborting\n");
                                exit(0);
                        }

			 start_datasize_bytes = atol( argv[2]);
            		 end_datasize_bytes = atol( argv[3]);
            		 increment_by_bytes =  atol( argv[4]);
			 break;
		default :
			 printf("\n Invalid options...\n");
			 printf("\n Usage : <./exe> [memMode : 0:pageable , 1:pinned ] [start-bytes] [end-bytes] [increment-bytes]\n");
			 exit(-1);
			
	}
}


// function to calculate bandwidth in MB/s.

float bandwidth_calc(long int data_size_in_bytes, float time)
{
	
	float BW,datasize_in_MB;
	datasize_in_MB = (float)(data_size_in_bytes / 1048576.00) ;   	//1024 *1024 = 1048576
	BW = datasize_in_MB / time;
	return BW;
}
