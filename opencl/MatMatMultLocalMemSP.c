/*****************************************************************************

                         C-DAC Tech Workshop : HEMPA-2011
                             Oct 17 - 21, 2011

  Example     : MatMatMulLocalMemSP.c
 
  Objective   : Perform matrix matrix multiplication using local memory
                (single precision)

  Input       : None 

  Output      : Execution time in seconds , Gflops achieved
                                                                                                                            
  Created     : Aug 2011    

  E-mail      : betatest@cdac.in         
                                 
****************************************************************************/
#include<CL/cl.h>
#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<string.h>
#include <math.h>

#define EPS 1.0e-8f     /* threshhold aprrox epsilion value */

#define BLOCK_SIZE 32   // If  value is modified , modification 
                        // should be done in .cl file(kernel file)

#define SIZE 128       // Modify SIZE to execute for different data 
                       // sizes and SIZE should be multiple of BLOCK_SIZE

#define OPENCL_CHECK_STATUS(NAME,ERR) {\
        const char *name=NAME;\
        cl_int Terr=ERR;\
        if (Terr != CL_SUCCESS) {\
                printf("\n\t\t Error: %s (%d) \n",name, Terr);\
                exit(-1);} }\

const char * matMatMultKernelPath = "MatMatMultLocalMemSP_kernel.cl";

/* read program source file*/
char* readKernelSource(const char* kernelSourcePath)
 {
        FILE    *fp = NULL;
        size_t  sourceLength;
        char    *sourceString ;
        fp = fopen( kernelSourcePath , "r");
        if(fp == 0)
        {
                printf("failed to open file");
                return NULL;
        }
        // get the length of the source code
        fseek(fp, 0, SEEK_END);
        sourceLength = ftell(fp);
        rewind(fp);
        // allocate a buffer for the source code string and read it in
        sourceString = (char *)malloc( sourceLength + 1);
        if( fread( sourceString, 1, sourceLength, fp) !=sourceLength )
        {
                printf("\n\t Error : Fail to read file ");
                return NULL;
        }
        sourceString[sourceLength+1]='\0';
        fclose(fp);
        return sourceString;
 }// end of readKernelSource

void print_on_screen(char * programName,double tsec,int size,double gflops,int flag)//flag=1 if gflops has been calculated else flag =0
{
        printf("\n\t---------------%s----------------\n\n",programName);
        printf("\t\t\tSIZE\t TIME_SEC\t gflops\n \n");
        if(flag==1)
        printf("\t\t\t%d\t%f\t %lf\t\n",size,tsec,gflops);
        else
        printf("\t\t\t%d\t%lf \t%s\t\n",size,tsec,"---");
        printf("\n\t---------------------------------------------------------------------------------------");

}

void fill_sp_matrix(float* matrix,int rowSize,int colSize)
{
        int     row, col ;

        for( row=0; row < rowSize; row++)
             for( col=0; col < colSize; col++)
                        matrix[row * colSize + col] = rand()%10;

}

//free  host matrix memory
void hSpMatrixFree(float * arr,int len)
{
        free(arr);
}

int getPlatform(cl_platform_id *selectedPlatform,cl_uint *numDevices)
{
        cl_int          err;
        int             count;
        char            pbuff[100];
        cl_uint         num_platforms;
        cl_platform_id  *platforms;

        *selectedPlatform = NULL;

 /*  Get the number of OpenCL Platforms Available */
        err = clGetPlatformIDs ( 0, 0, &num_platforms);
        if( err != CL_SUCCESS || num_platforms == 0) {
                printf(" \n\t\t No Platform Found \n");
                return 1;
        }
        else
        {
                if( num_platforms == 0)
                {
                     return 1;
                }
               else
                {
                        /* Allocate the space for available platform*/
                        assert( (platforms = (cl_platform_id *)malloc( sizeof(cl_platform_id) * (num_platforms))) != NULL);
                        /*  Get available OpenCL Platforms IDs*/
                        err = clGetPlatformIDs( num_platforms,platforms, NULL);
                        OPENCL_CHECK_STATUS(" Failed to get Platform IDs",err);
                        for ( count = 0 ; count < num_platforms ; count++)
                        {
                                /* get platform info*/ 
				err=clGetPlatformInfo(platforms[count],CL_PLATFORM_NAME,sizeof(pbuff),pbuff,NULL);
                                OPENCL_CHECK_STATUS("clGetPlatformInfo Failed",err);
                                /* get device id and info*/
                                err = clGetDeviceIDs( platforms[count],CL_DEVICE_TYPE_GPU,0,0,numDevices);
                                if( err != CL_SUCCESS  || *numDevices ==0)
                                {
                                         continue;
                                }
                                else
                                {
                                        /* get selected platform*/
                                        *selectedPlatform =platforms[count];
					printf("\n\t---------------------------Device details-------------------------------------\n\n");
                                        printf("\tPlatform used                            :  %s\n",pbuff);
                                        break;
                                }
                        }
                }
        }

        if ( count == num_platforms ) {
                printf(" \n\t No platform found \n");
                return 1;
        }  free(platforms);
        return 0;
}


/********************************************************************
function to execute set execution env
********************************************************************/
void setExeEnvMatMatMultLMSP(cl_context *context, cl_uint *numDevices, cl_device_id **devices, cl_program *program,cl_uint *num_platforms,cl_platform_id *selectedPlatform,cl_int *err)
{
        char            pbuff[100];              //holds platform information (platform name)
        char            dbuff[100];             //holds device information (platform name)
	size_t          kernel_src_length;      /* hold the kernel source string length */
        char            *kernel_src_str;       /* hold the kernel source string */

        cl_build_status buildbuff;             //holds device information (platform name)
       

        int count;

        /*  Get the number of OpenCL Platforms Available */
        *err=getPlatform(selectedPlatform,numDevices);
        OPENCL_CHECK_STATUS("error while getting device info",*err);

        assert(((*devices)= (cl_device_id *) malloc( sizeof(cl_device_id ) *(*numDevices))) != NULL);
        *err = clGetDeviceIDs( *selectedPlatform, CL_DEVICE_TYPE_GPU, (*numDevices), *devices, 0);

        /* Get device Name */
        *err = clGetDeviceInfo(*devices[0], CL_DEVICE_NAME, sizeof(dbuff), &dbuff, NULL);
        OPENCL_CHECK_STATUS("error while getting device info",*err);
	printf("\tDevice used                              :  %s\n",dbuff);
	
        /*create context*/
        *context=clCreateContext(0,1,devices[0],0,0,err);
	 printf("\tNumber of GPU  devices used              :  %d\n",*numDevices);
        if ( *err != CL_SUCCESS || *context == 0)
        {
                printf("\n\t No GPU detected ");
                printf("\n\t Context : %d , Err : %d",context, err);
                exit(-1);
        }
	printf("\n\t------------------------------------------------------------------------------\n");
        /*create program with source*/

	char* programSource = readKernelSource(matMatMultKernelPath);
        size_t sourceSize =  strlen(programSource) ;
        *program = clCreateProgramWithSource(*context, 1,(const char **) &programSource, &sourceSize, err);
        OPENCL_CHECK_STATUS("error while creating program",*err);

        /*build program*/
        *err = clBuildProgram(*program,1,devices[0],NULL,NULL,NULL);
        OPENCL_CHECK_STATUS("error while building  program",*err);



}



/**************************************************************
function to execute Matrix Matrix Multiplication
*************************************************************/
void    matrixMatrixMultLMSP (cl_uint numDevices,cl_device_id *devices, cl_program program,cl_context context,float * h_MatA, float *h_MatB, float *h_Output,int height,int width)
{

        cl_command_queue        cmdQueue;     // Command Queue  object
        cl_mem                  d_MatA, d_MatB;     //  device Mat buffer
        cl_mem                  d_rows, d_cols;     //  device rpw and cols buffer
        cl_mem                  d_Output;      // device output buffer
        cl_kernel               kernel;        //  kernel object
        cl_int                  err;            // Holds the error 
        cl_event                events;        // event object
        size_t                  globalWorkSize[2];    // holds global_work size
        size_t                  localWorkSize[2];    // holds local work size
        int                      count;
        char                    dbuff[100];          
        float                  gflops=0.0f;             //holds total achieved gflops

	cl_ulong startTime, endTime,elapsedTime; //holds time
	float executionTimeInSeconds;        //holds total execution time

	cl_event gpuExec[1];         // event object


        /* Get device Name */
       err = clGetDeviceInfo(devices[0], CL_DEVICE_NAME, sizeof(dbuff), &dbuff, NULL);
       OPENCL_CHECK_STATUS("Failed  to Get device Name",err);


	/** Create the command queue **/
        cmdQueue = clCreateCommandQueue( context, devices[0], CL_QUEUE_PROFILING_ENABLE, &err);
        if( err != CL_SUCCESS || cmdQueue == 0)
        {
               printf("\n\t Failed to create command queue  \n" );
               exit (-1);
        }
        


        /* create buffers*/
        d_MatA =clCreateBuffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,(height*width)*sizeof(float),h_MatA,&err);
        OPENCL_CHECK_STATUS("Failed to create device Mat buffer A  ",err);

        
        d_MatB =clCreateBuffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,(height*width)*sizeof(float),h_MatB,&err);
        OPENCL_CHECK_STATUS("Failed to create device Mat buffer B  ",err);

   	d_Output = clCreateBuffer ( context, CL_MEM_WRITE_ONLY ,( height*width) * sizeof(float),NULL, &err);
        OPENCL_CHECK_STATUS( "Failed to create device output  buffer   ",err);

        d_rows =clCreateBuffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,sizeof(cl_int),(void*)&height,&err);
        OPENCL_CHECK_STATUS( "Failed to create device output  buffer   ",err);

        d_cols =clCreateBuffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,sizeof(cl_int),(void*)&height,&err);
        OPENCL_CHECK_STATUS( "Failed to create device output  buffer   ",err);


         // Create the kernel
         kernel = clCreateKernel ( program, "matMatMultKernel", &err);
         OPENCL_CHECK_STATUS(" Create kernel failed ",err);

          //  Set the arguments
     	err = clSetKernelArg( kernel, 0, sizeof(cl_mem), (void *) &d_Output);
     	OPENCL_CHECK_STATUS( "Set  kernel argument 0 failed ",err);
           	
	err = clSetKernelArg( kernel, 1, sizeof(cl_mem), (void *) &d_MatA);
        OPENCL_CHECK_STATUS( "Set  kernel argument 1 failed ",err);
        
   	err = clSetKernelArg( kernel, 2, sizeof(cl_mem), (void *) &d_MatB);
        OPENCL_CHECK_STATUS( "Set  kernel argument 2 failed ",err);
           
	err = clSetKernelArg( kernel, 3, sizeof(cl_float) * BLOCK_SIZE * BLOCK_SIZE, 0);
        OPENCL_CHECK_STATUS( "Set  kernel argument 3 failed ",err);
           
	err = clSetKernelArg( kernel, 4, sizeof(cl_float) * BLOCK_SIZE * BLOCK_SIZE, 0);
        OPENCL_CHECK_STATUS( "Set  kernel argument 4 failed ",err);
           
	err = clSetKernelArg( kernel, 5, sizeof(cl_mem), (void *) &d_rows);
        OPENCL_CHECK_STATUS( "Set  kernel argument 5 failed ",err);
           
	err = clSetKernelArg( kernel, 6, sizeof(cl_mem), (void *) &d_cols);
        OPENCL_CHECK_STATUS( "Set  kernel argument 6 failed ",err);


         //set Global work size and local work size
        globalWorkSize [0]= height   ; // ND Range Size for each kernel launch 
        globalWorkSize[1]=  width ; // ND Range Size for each kernel launch

        localWorkSize [0]= BLOCK_SIZE    ; // ND Range Size for each kernel launch 
        localWorkSize[1]= BLOCK_SIZE   ; // ND Range Size for each kernel launch

         //launch the kernel

         err=clEnqueueNDRangeKernel(cmdQueue,kernel,2,NULL,globalWorkSize,localWorkSize,0,NULL,&gpuExec[0]);
        OPENCL_CHECK_STATUS( " Kernel launch failed ",err);

	
	//completion of all commands to command queue
	err = clFinish(cmdQueue);
        OPENCL_CHECK_STATUS("clFinish",err);

	//calculate start time and end time
	clGetEventProfilingInfo(gpuExec[0], CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &startTime, NULL);
        clGetEventProfilingInfo(gpuExec[0], CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &endTime, NULL);


	/*calculate total elapsed time*/
 	elapsedTime = endTime-startTime;
	/* total execuition time in seconds*/
 	executionTimeInSeconds = (float)(1.0e-9 * elapsedTime);

		
         //read the result
         err =clEnqueueReadBuffer(cmdQueue,d_Output,CL_TRUE,0,(height*width)*sizeof(cl_float),h_Output,0,0,&events);
         OPENCL_CHECK_STATUS(" Read output failed ",err);

	/* calculate gflops*/
        gflops= (1.0e-9 * ((2. * height*height*height) / executionTimeInSeconds));


        // Print the gflops on the screen
	 print_on_screen("Matrix Matrix Multiplication Local Memory Single Precision",executionTimeInSeconds,height,gflops,1);



        //free opencl objects
        if ( kernel )   clReleaseKernel(kernel);
        if ( cmdQueue) clReleaseCommandQueue(cmdQueue);
        if ( events )   clReleaseEvent(events);
	clReleaseMemObject(d_MatA);
        clReleaseMemObject(d_MatB);
        clReleaseMemObject(d_rows);
        clReleaseMemObject(d_cols);
        clReleaseMemObject(d_Output);


}

/*****************************************************************
function to execute check result
******************************************************************/
int matMatMultCheckResultLMSP (float *h_MatA, float *h_MatB,float *output, int rows, int cols)
{

        int i,j,k,step=0;
        float *temp_Out;
        float sum;
	float  errorNorm = 0.0;
        float  eps=EPS;
        float  relativeError=0.0;
        int     flag=0;

        assert((temp_Out = (float *)malloc( sizeof(float) * rows * rows))!=NULL);
        /*calculate sequential result*/
        for( i=0 ; i<rows ; i++)
        {
                for( j=0 ; j<rows  ; j++)
                {
                        sum = 0.00f;
                        for( k=0 ; k<cols  ; k++)
                        {
                                sum += h_MatA[i * cols   + k] * h_MatB[k * rows   + j];
                        }
                        temp_Out[step++] = sum;
                }
        }
        /* check opencl result with sequential result*/
        for( i=0 ; i < rows  ; i++)
        {
                for( j=0 ; j < rows  ; j++)
                {
			if (fabs(temp_Out[i*rows +j]) > fabs(output[i*rows +j]))
                        relativeError = fabs((temp_Out[i*rows +j] - output[i*rows +j]) / temp_Out[i*rows +j]);
                	else
                        relativeError = fabs((output[i*rows +j] - temp_Out[i*rows +j]) / output[i*rows +j]);

                	if (relativeError > eps && relativeError != 0.0e+00 )
			{
                        	if(errorNorm < relativeError) 
				{
                        		errorNorm = relativeError;
                        		flag=1;
					return 1;
                        	}
                	}
                 }
         }
	if( flag == 1) {

                printf(" \n Results verfication : Failed");
                printf(" \n Considered machine precision : %e", eps);
                printf(" \n Relative Error                  : %e", errorNorm);

        }
        else 
	{

                printf("\n\t\t\t Results verfication : Success\n");
        }

        free(temp_Out);
        return 0;
}



/*****************************************************************
			 main
*******************************************************************/
int main(int argc,char *argv[])
{
	cl_platform_id selectedPlatform;      //holds list of platforms
  	cl_uint num_platforms;         //holds number of platforms
	cl_int err;                    //holds error (return value)
	cl_uint         numDevices;   /*hold the number of devices */
        cl_device_id    *devices;      /* hold list of devices */
	int count;
	cl_context context;           //holds context object
	cl_program program;           //holds program object
	char build_log[10];		//holds program build info
	cl_kernel kernel;		//holds kernel object
	float *h_MatA;			//holds host MatA buffer
	float *h_Output;		//holds host output buffer
	float *h_MatB;		//holds host output buffer
	int i;
	int height=SIZE;
	int width=SIZE;
	
	/* allocate host memory*/
	assert((h_MatA=(float *)malloc(height*width*sizeof(float)))!=NULL);
	assert((h_Output=(float *)malloc(height*width*sizeof(float)))!=NULL);
	assert((h_MatB=(float *)malloc(height*width*sizeof(float)))!=NULL);

	/*initialize host memory*/
	fill_sp_matrix(h_MatA,height,width);
	fill_sp_matrix(h_MatB,height,width);
	for(i=0;i<height*width;i++)
	{
		h_Output[i]=0;
	}
	
	//function to set execution environment for opencl
	setExeEnvMatMatMultLMSP( &context , &numDevices, &devices, &program,&num_platforms,&selectedPlatform,&err );

	//function to calculate Matrix Matrix Multiplication
	matrixMatrixMultLMSP(numDevices, devices, program, context, h_MatA,h_MatB, h_Output,height,width);


	/*  check opencl results with cpu results*/

	  matMatMultCheckResultLMSP( h_MatA,h_MatB,h_Output, height , width );



	/********************************************************
		uncomment to print on the screen
	********************************************************/

	/* print buffer object*/
/*	printf("MatA\n");
	for(i=0;i<height*height;i++)
	{
		printf("%f\n",h_MatA[i]);
	}
	printf("mat B\n");
	for(i=0;i<height*height;i++)
	{
		printf("%f\n",h_MatB[i]);
	}
	printf("output\n");
	for(i=0;i<height*height;i++)
	{
		printf("%f\n",h_Output[i]);
	}*/

	/* free the host memories*/
        hSpMatrixFree(h_MatA,height);
        hSpMatrixFree(h_MatB,height);
        hSpMatrixFree(h_Output,height);
}

