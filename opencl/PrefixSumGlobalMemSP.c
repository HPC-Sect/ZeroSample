/*****************************************************************************

                         C-DAC Tech Workshop : HEMPA-2011
                             Oct 17 - 21, 2011

  Example     : PrefixSumGlobalMemSP.c
 
  Objective   : Calcualte prefix sum of an array of N elements using global memory
                (Single precision)

  Input       : None 

  Output      : Execution time in seconds, gflops achived
                                                                                                                            
  Created     : Aug 2011    

  E-mail      : betatest@cdac.in         
                                 
****************************************************************************/


/*source code for Prefix sum*/
#include<CL/cl.h>
#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<string.h>
#include <math.h>
#define EPS 1.0e-8f /* threshhold aprrox epsilion value */

#define SIZE 128       // change this to run for different sizes

/* opencl check status mecro*/
#define OPENCL_CHECK_STATUS(NAME,ERR) {\
        const char *name=NAME;\
        cl_int Terr=ERR;\
        if (Terr != CL_SUCCESS) {\
                printf("\n\t\t Error: %s (%d) \n",name, Terr);\
                exit(-1);} }\

const char * kernelSourcePath = "PrefixSumGlobalMemSP_kernel.cl";

int prefixSumCheckResultGMSP(float *inputArray,float *output,int arrayLength);

/*
 * Fill in the vector with single precision values
 */
void fill_sp_vector(float* vec,int size)
{
        int ind;
        for(ind=0;ind<size;ind++)
                vec[ind]=rand()%10;
}


/* vector free*/
void hSpVectFree(float * arr,int len)
{
        int i;
        free(arr);
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

	printf("\n\n\t -----------------------------------------------------\n");

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
        printf("\n\t---------------%s----------------\n",programName);
        printf("\tSIZE\t TIME_SEC\t gflops \n");
        if(flag==1)
        printf("\t%d\t%f\t %lf\t",size,tsec,gflops);
        else
        printf("\t%d\t%lf \t%s\t",size,tsec,"---");
        printf("\n\t--------------------------------------------------");

}



/* set execution env for opencl*/
void setExeEnvPrefixSumGMSP(cl_context *context, cl_uint *numDevices, cl_device_id **devices, cl_program *program,cl_uint *numPlatforms,cl_platform_id *platforms,cl_int *err)
 {
        char            pbuff[100];   //holds platform information (platform name)
        char            dbuff[100];   //holds device information (platform name)
        int count;

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
        printf("\tNumber of GPU  devices used              :  %d\n",*numDevices);
        if ( *err != CL_SUCCESS || *context == 0)
        {
                printf("\n\t No GPU detected ");
                printf("\n\t Context : %d , Err : %d",context, *err);
                exit(-1);
        }

        printf("\t---------------------------------------------------------------------\n");
        /*create program with source*/
	// create a CL program using kernel source 
	  char* programSource = readKernelSource(kernelSourcePath);
   	size_t sourceSize =  strlen(programSource) ;
   	*program = clCreateProgramWithSource(*context, 1,(const char **) &programSource, &sourceSize, err);
       // *program = clCreateProgramWithSource(*context,1,(const char **) &ProgramSourceMatMatAdd,NULL,err );
        OPENCL_CHECK_STATUS("error while creating program",*err);

        /*build program*/
        *err = clBuildProgram(*program,1,devices[0],NULL,NULL,NULL);
        OPENCL_CHECK_STATUS("error while building  program",*err);

       

 }// end of setExeEnvPrefixSum



void    prefixSumGMSP (cl_uint numDevices,cl_device_id *devices, cl_program program,cl_context context,float * h_Vect, float *h_Output,int vectSize)
{
        cl_event                gpuExec[1];
        cl_int err;
        cl_command_queue cmdQueue;   //holds command queue object
        cl_kernel kernel;               //holds kernel object
        cl_mem d_Vect,d_VectSize,d_Output;          //holds device input output buffer
         cl_event                events;        // events
        size_t globalWorkSize[2]={vectSize,vectSize}; //holds global group size

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
        kernel = clCreateKernel(program,"prefixSum_kernel",&err);
        OPENCL_CHECK_STATUS("error while creating kernel",err);

        /*create buffer*/
       d_Vect=clCreateBuffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,sizeof(float)*vectSize,h_Vect,&err);
        OPENCL_CHECK_STATUS("error while creating buffer for input",err);

       d_VectSize=clCreateBuffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,sizeof(float),(void *)&vectSize,&err);
        OPENCL_CHECK_STATUS("error while creating buffer for input",err);

        d_Output=clCreateBuffer(context,CL_MEM_WRITE_ONLY,sizeof(float)*vectSize,NULL,&err);
        OPENCL_CHECK_STATUS("error while creating buffer for output",err);
        /*set kernel arg*/
        err=clSetKernelArg(kernel,0,sizeof(cl_mem),&d_Vect);
        OPENCL_CHECK_STATUS("error while setting arg 0",err);

        err=clSetKernelArg(kernel,2,sizeof(cl_mem),&d_VectSize);
        OPENCL_CHECK_STATUS("error while setting arg 1",err);

        err=clSetKernelArg(kernel,1,sizeof(cl_mem),&d_Output);
        OPENCL_CHECK_STATUS("error while setting arg 2",err);

        /*load kernel*/
        err = clEnqueueNDRangeKernel(cmdQueue,kernel,2,NULL,globalWorkSize,NULL,0,NULL,&gpuExec[0]);
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
        err = clEnqueueReadBuffer(cmdQueue,d_Output,CL_TRUE,0,sizeof(cl_float)*vectSize,h_Output,0,0,&events);
        OPENCL_CHECK_STATUS("error while reading buffer",err);


        /* calculate total gflops*/
         gflops= (1.0e-9 * (( vectSize * vectSize) / executionTimeInSeconds));


        // Print the gflops on the screen
         print_on_screen("Prefix sum",executionTimeInSeconds,vectSize,gflops,1);


        //check results 
//        VectVectAddCheckResult(h_VectA,h_VectB,h_output,vectSize);

        //release opencl objects
        clReleaseMemObject(d_Vect);
        clReleaseMemObject(d_VectSize);
        clReleaseMemObject(d_Output);
        clReleaseProgram(program);
        clReleaseKernel(kernel);
        clReleaseCommandQueue(cmdQueue);
        clReleaseContext(context);
}



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
        float *h_Vect;                 //holds host input buffer
        float *h_Output;                //holds host output buffer
        int i;

                
        int vectSize=SIZE;
        /* allocate host memory*/
        assert((h_Vect=(float *)malloc(vectSize*sizeof(float)))!=NULL);
        assert((h_Output=(float *)malloc(vectSize*sizeof(float)))!=NULL);
        
        /*initialize host memory*/
        fill_sp_vector(h_Vect,vectSize);
        for(i=0;i<vectSize;i++)
        {
                h_Output[i]=0;
        }
 
        //setting the execution environment for opencl
        setExeEnvPrefixSumGMSP( &context , &numDevices, &devices, &program,&numPlatforms,&platforms,&err );


        //function to fo Vector Vector Addition 
        prefixSumGMSP (numDevices, devices, program, context, h_Vect,h_Output,vectSize);

	prefixSumCheckResultGMSP(h_Vect, h_Output,vectSize);

/********************************************************
uncomment to print output vector  on the screen
********************************************************/
        /*for(i=0;i<vectSize;i++)
        {
                printf("%f\n",h_VectA[i]);
        }*/
        /* print buffer object*/
       /* for(i=0;i<vectSize;i++)
        {
                printf("%f\n",h_output[i]);
        }*/
        //free host memory
        hSpVectFree(h_Vect,vectSize);
        hSpVectFree(h_Output,vectSize);
}


int prefixSumCheckResultGMSP(float *h_Vect,float *output,int arrayLength)
{
	int curElementIndex ,counter;
	float temp_result;
	int j,flag=0;
	curElementIndex = 0;
	float *temp_out;
	float  errorNorm = 0.0;
        float  eps=EPS;
        float  relativeError=0.0;

	assert((temp_out = (float *)malloc( sizeof(float) * arrayLength))!=NULL);

	while(curElementIndex < arrayLength)	
	{
		temp_result = 0.00f;
		for(counter = 0 ; counter < curElementIndex ; counter++)
			temp_result = temp_result + h_Vect[counter];

		temp_out[curElementIndex] = temp_result;
		curElementIndex++ ;		
	}
	for( j=0 ; j < arrayLength  ; j++)
        {
		 if (fabs(temp_out[j]) > fabs(output[j]))
                        relativeError = fabs((temp_out[j] - output[j]) / temp_out[j]);
                else
                        relativeError = fabs((output[j] - temp_out[j]) / output[j]);
                       
 
                if (relativeError > eps && relativeError != 0.0e+00 )
                {
                        if(errorNorm < relativeError)
                        {
                               errorNorm = relativeError;
                               flag=1;
                        }
                }
        }
	if( flag == 1) {

                printf(" \n Results verfication : Failed");
                printf(" \n Considered machine precision : %e", eps);
                printf(" \n Relative Error                  : %e", errorNorm);

        }

        if(flag==0)
        {
                printf("\n\n\t Result Varification success:\n");
        }
             // printf("\n");
        free(temp_out);
        return 0;


	
}
