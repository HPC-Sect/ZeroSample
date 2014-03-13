/*****************************************************************************

                         C-DAC Tech Workshop : HEMPA-2011
                             Oct 17 - 21, 2011

  Example     : PieCal.c
 
  Objective   : Calcualte Pie value using global memory
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
#define EPS 1.0e-8f /* threshhold aprrox epsilion value */

#define NUM_OF_INTERVALS 128       // Modify NUM_OF_INTERVALS to execute for different data sizes

/* opencl check status mecro*/
#define OPENCL_CHECK_STATUS(NAME,ERR) {\
        const char *name=NAME;\
        cl_int Terr=ERR;\
        if (Terr != CL_SUCCESS) {\
                printf("\n\t\t Error: %s (%d) \n",name, Terr);\
                exit(-1);} }\

const char * pieCalKernelPath = "PieCalculationGlobalMemSP_kernel.cl";
int pieCalCheckResultGMSP(int noOfIntervals,float *finalPieValue);

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

/* print result on the screen */
void print_on_screen(char * programName,double tsec,int size,double gflops,int flag)//flag=1 if gflops has been calculated else flag =0
{
        printf("\n\t---------------%s----------------\n\n",programName);
        printf("\t\tNUM_OF_INTERVALS\t TIME_SEC\t gflops \n");
        if(flag==1)
        printf("\t\t%d\t\t%f\t %lf\t",size,tsec,gflops);
        else
        printf("\t\t%d\t\t\t%lf \t%s\t",size,tsec,"---");
        printf("\n\n\t----------------------------------------------");

}

/* get platform function*/
int getPlatform(cl_platform_id *selectedPlatform,cl_uint *numDevices)
{
        cl_int          err;
        int             count;
        char            pbuff[100];
        cl_uint         numPlatforms;
        cl_platform_id  *platforms;

        *selectedPlatform = NULL;

 /*  Get the number of OpenCL Platforms Available */
        err = clGetPlatformIDs ( 0, 0, &numPlatforms);
        if( err != CL_SUCCESS || numPlatforms == 0) {
                printf(" \n\t\t No Platform Found \n");
                return 1;
        }
        else
        {
                if( numPlatforms == 0)
                {
                     return 1;
                }
               else
                {
                        /* Allocate the space for available platform*/
                        assert( (platforms = (cl_platform_id *)malloc( sizeof(cl_platform_id) * (numPlatforms))) != NULL);
                        /*  Get available OpenCL Platforms IDs*/
                        err = clGetPlatformIDs( numPlatforms,platforms, NULL);
                        OPENCL_CHECK_STATUS(" Failed to get Platform IDs",err);
                        for ( count = 0 ; count < numPlatforms ; count++)
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
                                        printf("\tPlatform used                            :  %s\n",pbuff);
                                        break;
                                }
                        }
                }
        }

        if ( count == numPlatforms ) {
                printf(" \n\t No platform found \n");
                return 1;
        }  free(platforms);
        return 0;
}


/**************************************************************************
function to set execution environment of opencl
****************************************************************************/

void setExeEnvPiCalGMSP(cl_context *context, cl_uint *numDevices, cl_device_id **devices, cl_program *program,cl_uint *numPlatforms,cl_platform_id *platforms,cl_int *err)
{
	char            pbuff[100];   //holds platform information (platform name)
        char            dbuff[100];   //holds device information (platform name)
	int count;

	printf("\t---------------------------Device Deatils---------------------------\n\n");
        /*  Get the number of OpenCL Platforms Available */
        *err=getPlatform(platforms,numDevices);
        OPENCL_CHECK_STATUS("error while getting device info",*err);

        assert(((*devices)= (cl_device_id *) malloc( sizeof(cl_device_id ) *(*numDevices))) != NULL);
        *err = clGetDeviceIDs( *platforms, CL_DEVICE_TYPE_GPU, (*numDevices), *devices, 0);

        /* Get device Name */
        *err = clGetDeviceInfo(*devices[0], CL_DEVICE_NAME, sizeof(dbuff), &dbuff, NULL);
        OPENCL_CHECK_STATUS("error while getting device info",*err);
	printf("\tDevice used                              :  %s\n",dbuff);
        
	/*create context*/
        *context=clCreateContext(0,1,devices[0],0,0,err);
	printf("\tNumber of GPU  devices used              :  %d\n\n",*numDevices);

        if ( *err != CL_SUCCESS || *context == 0)
        {
                printf("\n\t No GPU detected ");
                printf("\n\t Context : %d , Err : %d",context, err);
                exit(-1);
        }

	printf("\t---------------------------------------------------------------------\n");
	
	/*create program with source*/
	char* programSource = readKernelSource(pieCalKernelPath);
        size_t sourceSize =  strlen(programSource) ;
        *program = clCreateProgramWithSource(*context, 1,(const char **) &programSource, &sourceSize, err);

        OPENCL_CHECK_STATUS("error while creating program",*err);
        
	/*build program*/
        *err = clBuildProgram(*program,1,devices[0],NULL,NULL,NULL);
        OPENCL_CHECK_STATUS("error while building  program",*err);
        
}


/**********************************************************************
function to execute Pie value
**********************************************************************/

void    pieCalculationGMSP (cl_uint numDevices,float *finalPieValue,cl_device_id *devices, cl_program program,cl_context context,int numOfIntervals)
{
	cl_event                gpuExec[1];
        cl_int err;	
	cl_command_queue cmdQueue;   //holds command queue object
	cl_kernel kernel;		//holds kernel object
	cl_mem d_area,d_numOfIntervals;		//holds device input output buffer
	cl_event                events;        // events
	size_t globalWorkSize[1]={numOfIntervals}; //holds global group size
	double                  gflops=0.0;             //holds total achieved gflops
        cl_ulong startTime, endTime,elapsedTime;        //holds time
        float executionTimeInSeconds;                    //holds total execution time
	  
    	/*create command queue*/
        cmdQueue = clCreateCommandQueue(context, devices[0],  CL_QUEUE_PROFILING_ENABLE, &err);
        if( err != CL_SUCCESS ||  cmdQueue == 0)
         {
               printf("\n\t Failed to create command queue  \n" );
               exit (-1);
         }
        
	/*create kernel object*/
        kernel = clCreateKernel(program,"pieCalculation",&err);
        OPENCL_CHECK_STATUS("error while creating kernel",err);
        
	/*create buffer*/
	d_area = clCreateBuffer(context,CL_MEM_WRITE_ONLY , sizeof(cl_float), NULL, &err);
	OPENCL_CHECK_STATUS("error while creating buffer for devc_area",err);
	d_numOfIntervals = clCreateBuffer(context,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR , sizeof(cl_int), (void*)&numOfIntervals, &err);	
	OPENCL_CHECK_STATUS("error while creating buffer for devc_area",err);

	/*set kernel arg*/
	err= clSetKernelArg( kernel,0,sizeof(cl_mem),(void *) &d_numOfIntervals);
        OPENCL_CHECK_STATUS("error while setting arg 0",err);
        err = clSetKernelArg(kernel,1,sizeof(cl_mem),(void *) &d_area);
        OPENCL_CHECK_STATUS("error while setting arg 1",err);
        
	/*load kernel*/
        err = clEnqueueNDRangeKernel(cmdQueue,kernel,1,NULL,globalWorkSize,NULL,0,NULL,&gpuExec[0]);
        OPENCL_CHECK_STATUS("error while creating ND range",err);
        
	//completion of all commands to command queue
        err = clFinish(cmdQueue);
        OPENCL_CHECK_STATUS("clFinish",err);

	/* calculate start time and end time*/
        clGetEventProfilingInfo(gpuExec[0], CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &startTime, NULL);
        clGetEventProfilingInfo(gpuExec[0], CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &endTime, NULL);

	/* total elapsed time*/
        elapsedTime = endTime-startTime;

	/*total execution time*/
        executionTimeInSeconds = (float)(1.0e-9 * elapsedTime);
	
	/* reading buffer object*/
        err = clEnqueueReadBuffer(cmdQueue,d_area,CL_TRUE,0,sizeof(cl_float),finalPieValue,0,0,&events);
        OPENCL_CHECK_STATUS("error while reading buffer",err);

	printf("\n\t\tPI value%f",(*finalPieValue));	

        // Print the gflops on the screen
         print_on_screen("Pie calculation-global memory using SP",executionTimeInSeconds,numOfIntervals,gflops,0);

	//release opencl objects
	clReleaseMemObject(d_area);
	clReleaseMemObject(d_numOfIntervals);
	clReleaseProgram(program);
	clReleaseKernel(kernel);
	clReleaseCommandQueue(cmdQueue);
	clReleaseContext(context);
}


/*****************************************************
function to execute main function
******************************************************/
int main(int argc, char *argv[])
{
	cl_platform_id platforms;      //holds list of platforms
  	cl_uint numPlatforms;         //holds number of platforms
	cl_int err;                    //holds error (return value)
	cl_uint         numDevices;   /*hold the number of devices */
        cl_device_id    *devices;      /* hold list of devices */
	int count;
	cl_context context;           //holds context object
	cl_program program;           //holds program object
	size_t retValSize;
	int i;

	float *finalPieValue;
		
	assert(finalPieValue = (float*)malloc(sizeof(float)));

	int numOfIntervals=NUM_OF_INTERVALS;
	
	//setting the execution environment for opencl
	setExeEnvPiCalGMSP( &context , &numDevices, &devices, &program,&numPlatforms,&platforms,&err );


	//function to fo Vector Vector Addition	
	pieCalculationGMSP(numDevices,finalPieValue, devices, program, context, numOfIntervals);
	
	/* check result */
	pieCalCheckResultGMSP(numOfIntervals,finalPieValue);
	free(finalPieValue);
}


/***************************************************************************
function to check Opencl result woth sequential result
*****************************************************************************/

int pieCalCheckResultGMSP(int noOfIntervals,float *finalPieValue)
{
	float totalsum=0.0f,x=0.0f;
	float h = 1.0 / noOfIntervals;
	int i;
	float  errorNorm = 0.0f;
        float  eps=EPS;
        float  relativeError=0.0f;
        int flag=0;
	/* sequential code to calulate Pie value*/
	for (i = 1; i < noOfIntervals +1; i = i+1) 
	{
		x = h * (i + 0.5);
		totalsum = totalsum + 4.0/(1.0 + x * x);
	} 
	totalsum = totalsum * h;
	/* check Opecl results with sequential results*/
        relativeError = ((totalsum - ((*finalPieValue))) / totalsum);
        if (relativeError > eps && relativeError != 0.0e+00 )
        {
               if(errorNorm < relativeError)
                {
                        errorNorm = relativeError;
                        flag=1;
                }
         }
        if( flag == 1) {

                printf(" \n Results verfication : Failed");
                printf(" \n Considered machine precision : %e", eps);
                printf(" \n Relative Error                  : %e", errorNorm);

        }
        if(flag==0)
        {
                printf("\n\t\t Result Verification success\n\n");
        }
        return 0;

}
