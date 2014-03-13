/*****************************************************************************************

                 C-DAC Tech Workshop : HEMPA-2011
                         Oct 17 - 21, 2011

Exaple 	     : clMultiGPU-VectVectAdd.c

Descritption : The Simple example program for beginners to demonstart how to use the
		multiple GPU with OpenCL. OpenCL Events is used to syncronize the CPU
		and GPU devices. Vector Vector Addition is used in kernel. 

		Host reads the two vectors A & B . The vector is divided among the 
		available number of devices and that chunk of data will be copied 
		from host to each device . Each GPU device will do the computation on 
		its chuck of data. The Host reads the result from each device and 
		accumulate.   
	
*****************************************************************************************/

/* Header file inclusion */
#include<CL/cl.h>
#include<stdio.h>
#include<math.h>
#include<assert.h>
#include<string.h>

#define LINE "\n____________________________________________________________________________________\n\n"

cl_int status = CL_SUCCESS;    // integer value used to check whether operation is successfull or not

cl_platform_id 		*platforms=NULL; 	// OpenCL Platform IDs
cl_uint			num_platforms=0;	// Number of OpenCL Platforms
cl_int			platform_id = -1; // NVIDIA Platform ID
cl_uint 		num_devices=0;		// Number of OpenCL Devices
cl_device_id 		*devices;		// OpenCL Device IDs
char 		**device_name;		// Holds available devices name
cl_context_properties contextProp[3];

// Function Declaration
void checkStatus(const char *,cl_int ); 
int getPlatform();
int getDevice();
void checkResult(float *,float  *,float *, int );


/**** define the  OpenCL Kernel source string ****/
const char *kernel_src_str=
" __kernel void vectVectAdd(__global  float *d_a, __global float *d_b, __global float *d_output) \n"
"{\n"

	"int output;\n" 
	
       "int tid = get_global_id(0);\n"
       "d_output[tid] =  d_a[tid] + d_b[tid];\n"

"}\n"
;

///////////////////////////////////////////////////////////////////////////////////
// main() function
/////////////////////////////////////////////////////////////////////////////////
int  main(int argc, char **argv)
{


	cl_context		context; 	// OpenCL Context
	cl_command_queue 	*cmd_queue;	// OpenCL Command Queue	
	cl_mem 			*d_a, *d_b; 	// OpenCL device input buffer
	cl_mem			*d_output;	// OpenCL device output buffer
	cl_program		program;	// OpenCL Program
	cl_kernel		*kernel;	// OpenCL kernel
	size_t 			global_work_size;	// ND Range Size
 	size_t			kernel_src_length;	// Holds the length of the kernel source
	cl_int 			err;			// Holds the status of API call
	cl_event		*events;
	
	long int 		vector_size; 	// vector size
	float 			*h_a,*h_b;	// Host input buffer
	float 			*h_output;	// Host Output buffer
	int			partition_size,start_itr,end_itr; // Data partition Size
	int			iCount; 	// loop control variable 


	/********** 1. Reading Input & Initializing Input Buffer on Host **********/	
		/* Checking for Command line Arguments */
		if( argc != 2) {

			printf("\n Usage : ./<executable> <vector-size> \n");
			exit(-1);
		}

		/* Reading vector Size */
		vector_size = atol(argv[1]);
		printf(LINE);
		printf("\n >> Vector Size : %ld \n ",vector_size);		
		printf("\n >> Allocating Memory & Initializing the input vectors on host with random values  \n ");		
	
		/* Allocating Memory & Initializing Host Input Buffer with random values */
		assert( (h_a=(float *) malloc (sizeof(float) * vector_size )) != NULL) ;
		assert( (h_b =(float *) malloc (sizeof(float) * vector_size )) != NULL) ;
		assert( (h_output=(float *) malloc (sizeof(float) * vector_size )) != NULL) ;
	
        	for ( iCount = 0 ; iCount < vector_size  ; iCount++)
        	{
                	h_a[iCount]=h_b[iCount] = rand() + 1.10f ;
        	}
	


	/******** 2. Get OpenCL platform Info****************/
	printf("\n >> Query Platforms ....");
	if( getPlatform()) {
		printf(" \n\t\t Failed to get platform Info \n");
		exit(-1);
	}
	
	/****** 3. Get OpenCL Device Info***********/
	printf("\n >> Query devices ....");
	if( getDevice()) {
		printf(" \n\t\t Failed to get Device  Info \n");
		exit(-1);
	}

	contextProp[0] = CL_CONTEXT_PLATFORM;                          // prop name
        contextProp[1] = (cl_context_properties)platforms[platform_id];          // prop value
        contextProp[2] = 0;                                            // must be terminated with 0


	/****** 4. Create Context for GPU Device ***********/
        context = clCreateContext( contextProp, num_devices, devices, NULL, NULL, &err);
         if ( err != CL_SUCCESS || context == 0)
        {
                printf("\n\t No GPU detected ");
                printf("\n\t Context : %d , Err : %d",context, err);
                exit(-1);
        }
	printf("\n >> Creating Context........ done \n");
	printf("\nnum_devices = %d\n",num_devices);


	/******** 5. Create the command queue for requested devices *********/
	assert( (cmd_queue=(cl_command_queue *) malloc (sizeof(cl_command_queue) * num_devices)) != NULL) ;
	for ( iCount = 0 ; iCount < num_devices ; iCount++)
	{
		//printf("\n >> Creating Command Queue for device : %s  ",device_name[iCount]);
		cmd_queue[iCount] = clCreateCommandQueue( context, devices[iCount], 0, &err);
		if( err != CL_SUCCESS || cmd_queue[iCount] == 0)
        	{
		 	printf("\n\t Failed to create command queue for device : %d " , iCount );
			exit (-1);
		}	
	}

	printf("\n >> Creating Command Queue...... done \n ");
	//exit(-1);

	 /******* 6. Allocate and initialize memory buffer on each device  ****************/ 
		
		printf("\n >> Partitioning input vector data among the avaialble devices ");
		/* Partition Data among available  devices */	
		if ( vector_size % num_devices != 0 )
		{
			printf("\n\t Error : Vector Size should be perfectly divisible by number of devices \n");
			exit(-1);
		}
		else
			partition_size = vector_size / num_devices;
		
		printf("\n >> Partition size  for each device : %d ", partition_size);
	

		assert((d_a = (cl_mem *) malloc (sizeof(cl_mem) * num_devices)) != NULL);
		assert((d_b = (cl_mem *) malloc (sizeof(cl_mem) * num_devices)) != NULL);
		assert((d_output = (cl_mem *) malloc (sizeof(cl_mem) * num_devices)) != NULL);

		/* Allocate & initalize memory buffer on each device */
		for ( iCount = 0 ; iCount < num_devices ; iCount++ ) { 
		
			start_itr = iCount * partition_size ;
			end_itr  = start_itr + (partition_size-1);
			printf(" \n\n >> Partition size for device %s is [%d - %d]", device_name[iCount],start_itr,end_itr);  
		
			printf("\n >> Allocate and initialize memory buffer on device : %s ",device_name[iCount]);
		
			/** Allocate memory for input buffer A on device **/	
			d_a[iCount] = clCreateBuffer ( context, CL_MEM_READ_ONLY , partition_size  * sizeof(float),NULL, &err);
        		checkStatus("Failed to create device input buffer A  ",err);

			/** Allocate memory for input buffer B on device **/	
			d_b[iCount] = clCreateBuffer ( context, CL_MEM_READ_ONLY , partition_size  * sizeof(float),NULL, &err);
        		checkStatus("Failed to create device input buffer B  ",err);
        
			/** Allocate memory for output buffer on device **/	
			d_output[iCount] = clCreateBuffer ( context, CL_MEM_WRITE_ONLY ,partition_size * sizeof(float), NULL, &err);
        		checkStatus( "Failed to create device output  buffer   ",err);

			/** Copy data from host to device input buffer A **/	
			err = clEnqueueWriteBuffer ( cmd_queue[iCount], d_a[iCount], CL_TRUE, 0, partition_size * sizeof(float), &h_a[iCount * partition_size], 0, 0 , 0);
       			checkStatus("Failed to copy data from host to device ",err);
		
			/** Copy data from host to device input buffer B **/	
			err = clEnqueueWriteBuffer ( cmd_queue[iCount], d_b[iCount], CL_TRUE, 0, partition_size * sizeof(float), &h_b[iCount * partition_size], 0, 0 , 0);
       			checkStatus("Failed to copy data from host to device ",err);
		
		}

			printf("\n >> Allocate and initialize memory buffer on devices .........done \n");

//	        exit(-1);
	 /*******7 . Create Program object from the kernel source  *****************/
        kernel_src_length = strlen(kernel_src_str);
        program = clCreateProgramWithSource( context, 1, &kernel_src_str, &kernel_src_length, &err);
        checkStatus( " clCreateProgramWithSource Failed ",err);
	printf("\n >> Creating Program Object .... done \n ");

	 /******* 8. Build (compile & Link ) Program ***************/
        err = clBuildProgram( program,  num_devices , devices, NULL, NULL, NULL);
        checkStatus( " Build Program Failed ",err);
	printf("\n >> Build Program  .... done \n");


	assert((kernel = (cl_kernel *) malloc (sizeof(cl_kernel) * num_devices)) != NULL);
	for ( iCount = 0 ; iCount < num_devices ; iCount++ ) { 
        	
		printf("\n >> Creating Kernel %d ",iCount);
	
		/******* 9. Create the kernel **************/
        	kernel[iCount] = clCreateKernel ( program, "vectVectAdd", &err);
        	checkStatus(" Create kernel failed ",err);
        
		/******* 10. Set the kernel arguments ********/
        	err = clSetKernelArg( kernel[iCount], 0, sizeof(cl_mem), (void *) &d_a[iCount]);
      		checkStatus( "Set  kernel arguments failed ",err); 
       		err = clSetKernelArg( kernel[iCount], 1, sizeof(cl_mem), (void *) &d_b[iCount]);
      		checkStatus( "Set  kernel arguments failed ",err); 
       		err = clSetKernelArg( kernel[iCount], 2, sizeof(cl_mem), (void *) &d_output[iCount]);
      		checkStatus( "Set  kernel arguments failed ",err); 
	}
	printf("\n >> Create Kernel  .... done \n");


	global_work_size = partition_size; // ND Range Size for each kernel launch
	
	for ( iCount = 0 ; iCount < num_devices ; iCount++ ) { 
		
	printf("\n >> Launching Kernel %d on device %s ",iCount,device_name[iCount]);
	/****** 10. Enqueue / launch the kernel *******/
       	err = clEnqueueNDRangeKernel( cmd_queue[iCount], kernel[iCount], 1, NULL, &global_work_size , NULL, 0, NULL, NULL);
       	checkStatus( "  Kernel launch failed ",err);

	}
	printf("\n >> Launch Kernel  .... done \n ");


	assert((events = (cl_event *) malloc (sizeof(cl_event) * num_devices)) != NULL);
	for ( iCount = 0 ; iCount < num_devices ; iCount++ ) { 
	
	printf("\n >> Reading output from device %s ",device_name[iCount]);
 	/****** 11. Read Results from the device ******/
       	err = clEnqueueReadBuffer(cmd_queue[iCount], d_output[iCount], CL_TRUE, 0, partition_size * sizeof(cl_float), &h_output[iCount * partition_size] , 0, 0 ,&events[iCount]);
       	checkStatus(" Read output faied ",err);
	}

	clWaitForEvents(num_devices, events);	
	printf("\n >> Read Output  .... done \n");

       // printf("********* GPU computation Results ************** \n");
        /* Print Results */
      /*  for ( iCount = 0 ; iCount < vector_size  ; iCount++)
        {
                printf("\t %f ", h_output[iCount]);
        }
        printf("\n"); */

	/** Check GPU Results against the CPU results **/
	checkResult( h_a, h_b, h_output , vector_size);


	/********* 12. Cleanup ***********/
	if (cmd_queue) 	free(cmd_queue);
	if(kernel)	free(kernel);
	if(platforms)	free(platforms);
	if(devices)	free(devices);
	if(d_a)		free(d_a);
	if(d_b)		free(d_b);
	if(d_output)	free(d_output);
	if(h_a)		free(h_a);	
	if(h_b)		free(h_b);
	if(h_output)	free(h_output);
	if(device_name) free(device_name);
	if(events)	free(events);

	printf(LINE);		


} /* End of main() */

//////////////////////////////////////////////////////////////////////////////////////////
//Function for checking the status of the OpenCL APIS
//////////////////////////////////////////////////////////////////////////////////////////
void checkStatus(const char *name,cl_int err)
{
        if (err != CL_SUCCESS)
        {

                printf("\n\t\t Error: %s (%d) \n",name, err);
                exit(-1);
        }
}


//////////////////////////////////////////////////////////////////////////////////////////
//Function for getting the ( NVIDIA )OpenCL Platform ID
//////////////////////////////////////////////////////////////////////////////////////////
int getPlatform()
{
	cl_int 		err;
	int		iCount , i;
	char 		pbuff[100];	

	/*err = clGetPlatformIDs ( 0, 0, &num_platforms);
	if( err != CL_SUCCESS || num_platforms == 0) {
		printf(" \n\t\t No Platform Found \n");
		return 1;
	}	
	else 
		printf("\n Number of Platform Found is : %d", num_platforms);


	assert( (platforms = (cl_platform_id *) malloc( sizeof(cl_platform_id) * num_platforms)) != NULL);
	err = clGetPlatformIDs( num_platforms, platforms, NULL);
	checkStatus(" Failed to get Platform IDs",err);

	for ( iCount = 0 ; iCount < num_platforms ; iCount++) {
	
		err = clGetPlatformInfo ( platforms[iCount], CL_PLATFORM_NAME, sizeof(pbuff), pbuff, NULL);
        	checkStatus ("clGetPlatformInfo Failed ",err);
		if ((strstr (pbuff,"NVIDIA")) != NULL ) {
		        platform_id = iCount;
			printf("\n Platform Name : %s \n",pbuff);	
			break;
		}
		
        	//printf("\n\t\t %d) OpenCL Platform \t: %s \n ", iCount+1 ,pbuff);
	}
		
	if ( platform_id == -1 ) {	
		printf("\n\t\t NVIDIA Platform is not found \n");	
		return 1;
	}*/
	/////////////////////////////////////////////////////////////////////////////

	// query host system for platform information

	err = clGetPlatformIDs(0 , 0 , &num_platforms);
        checkStatus("clGetPlatformIDs",err);

        assert(platforms = (cl_platform_id*)malloc(num_platforms * sizeof(cl_platform_id)));

        err = clGetPlatformIDs(num_platforms , platforms , NULL);
        checkStatus("clGetPlatformIDs",err);

        printf("\n Total number of platforms available are %d \n",num_platforms);

        for(i = 0 ; i < num_platforms ; i++)

        {
                err = clGetPlatformInfo(platforms[i] , CL_PLATFORM_NAME , sizeof(pbuff) , pbuff , NULL);
                checkStatus("clGetPlatformInfo for CL_PLATFORM_NAME",err);
                platform_id = i;
                printf("\n PLATFORM_NAME = %s \n",pbuff);        
        }

	return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
//Function for getting the OpenCL  Device IDs
//////////////////////////////////////////////////////////////////////////////////////////
int getDevice()
{

	cl_int 		err;
	int 		i;
	char		dbuff[100];
	char		pbuff[100];
	//char **deviceName;
	
         /*err = clGetDeviceIDs( platforms[platform_id], CL_DEVICE_TYPE_GPU, 0, 0, &num_devices);
      	 if( err != CL_SUCCESS || num_devices == 0)
         {
                        printf("\n\t\t ERROR : No OpenCL device found \n");
                        return 1;
         }
         if ( num_devices == 1){
       		         printf("\n\t\t Number of devices found is 1" );
       		         printf("\n\t\t Number of devices should be > 1 to perform Multi GPU \n");
			 return 1;
          }
	
	assert( (devices = (cl_device_id *) malloc( sizeof(cl_device_id) * num_devices)) != NULL);
        err = clGetDeviceIDs( platforms[platform_id], CL_DEVICE_TYPE_GPU, num_devices, devices, 0);
	checkStatus ("clGetDeviceIDs Failed", err);

	printf("\n Number of devices found : %d ", num_devices);	
	assert( (device_name = (char **) malloc( sizeof(char *) * num_devices)) != NULL);
	for ( iCount = 0 ; iCount < num_devices ; iCount++)
	{	
         	err = clGetDeviceInfo(devices[iCount], CL_DEVICE_NAME, sizeof(dbuff), &dbuff, NULL);
         	checkStatus("clGetDeviceInfo Failed ",err);
		assert( (device_name[iCount] = (char *) malloc( sizeof(char ) * 100)) != NULL);
		strcpy(device_name[iCount],dbuff);
         	printf("\n OpenCL Device \t: %s ",dbuff);

	}*/

	for(i = 0 ; i < num_platforms ; i++)

        {

                err = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU , 0 , 0 , &num_devices);

                if(err != CL_SUCCESS || num_devices == 0)

                {

                        err = clGetPlatformInfo(platforms[i] , CL_PLATFORM_NAME , sizeof(pbuff) , pbuff , NULL);
                        printf("\n------------------------------------------------------------------\n");
                        printf("\nNo GPU device found for the %s\n",pbuff);
                        printf("\n------------------------------------------------------------------\n");
                        continue;
                }

                else
                {

                         assert(devices = (cl_device_id*)malloc(num_devices * sizeof(cl_device_id)));
                         err = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU , num_devices , devices , 0);
                         platform_id = i;
                }

        }

        //query host system for device information

              assert( (device_name = (char **) malloc( sizeof(char *) * num_devices)) != NULL);

              for ( i = 0 ; i < num_devices ; i++)
              { 
                // Get devices Name 
                err = clGetDeviceInfo(devices[i], CL_DEVICE_NAME, sizeof(dbuff), &dbuff, NULL);
                checkStatus("clGetDeviceInfo ",err);
                assert( (device_name[i] = (char *) malloc( sizeof(char ) * sizeof(dbuff))) != NULL);
                strcpy(device_name[i],dbuff);
                printf("\n OpenCL Device Name \t= %s ",dbuff);

             }


	printf("\n\n");	
	return 0;

}	


/*/////////////////////////////////////////////////////////////
 Function to Check the GPU result against CPU result
////////////////////////////////////////////////////////////// */
void checkResult(float *h_a,float  *h_b,float *h_output, int vector_size)
{

        int i;
        int flag=0;

        for( i = 0 ; i < vector_size ; i++){
                if ( (h_a[i] + h_b[i]) != h_output[i] ){
                        flag = 1;
                        break;
                }
        }
        if ( flag == 1)
                printf ("\n >> Error : There is the differnce in the result on CPU and GPU \n\n");
        else
                printf ("\n >> Results are same from CPU and GPU computing \n\n");
}

	
