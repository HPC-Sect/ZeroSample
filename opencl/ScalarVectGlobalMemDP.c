
/*****************************************************************************

                         C-DAC Tech Workshop : HEMPA-2011
                             Oct 17 - 21, 2011

  Example     : ScalarVectGlobalMemDP.c
 
  Objective   : Perform scalar-vector multiplication using global memory
                (double precision)

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
#include<math.h>
#define SIZE 128       // Modify SIZE to execute for different data sizes
#define EPS 1.0e-15    // threshhold aprrox epsilion value
#define OPENCL_CHECK_STATUS(NAME,ERR) {\
        const char *name=NAME;\
        cl_int Terr=ERR;\
        if (Terr != CL_SUCCESS) {\
                printf("\n\t\t Error: %s (%d) \n",name, Terr);\
                exit(-1);} }\

const char * kernelSourcePathScalarVectDp = "ScalarVectGlobalMemDP_kernel.cl";
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
                                         printf("\n\t---------------------------Device details-------------------------------------\n\n");
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
                return 0;
        }
        sourceString[sourceLength+1]='\0';
        fclose(fp);
        return sourceString;
 }// end of readKernelSource 


// Fill in the vector with double precision values

void fill_dp_vector(double* vec,int size)
{
        int ind;
        for(ind=0;ind<size;ind++)
                vec[ind]=drand48();
}


void print_on_screen(char * programName,double tsec,int size,double gflops,int flag)//flag=1 if gflops has been calculated else flag =0
{
        printf("\n\t---------------%s----------------\n\n",programName);
        printf("\t\t\tSIZE\t TIME_SEC\t gflops \n\n");
        if(flag==1)
        printf("\t\t\t%d\t%f\t %lf\t\n",size,tsec,gflops);
        else
        printf("\t\t\t%d\t%lf \t%s\t\n",size,tsec,"---");
	printf("\n\t-----------------------------------------------------------------");

}

//free host vector memory
void hDpVectFree(double * arr,int len)
{
        free(arr);
}

void setExeEnvScalarVectGMDP(cl_context *context, cl_uint *numDevices, cl_device_id **devices, cl_program *program,cl_uint *numPlatforms,cl_platform_id *selectedPlatform,cl_int *err)
 {
        char            pbuff[100];   //holds platform information (platform name)
        char            dbuff[100];   //holds device information (platform name)
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
                printf("\n\t Context : %d , Err : %d",context, *err);
                exit(-1);
        }
	printf("\n\t------------------------------------------------------------------------------\n");
        /*create program with source*/
	// create a CL program using kernel source 
	  char* sProgramSource = readKernelSource(kernelSourcePathScalarVectDp);
   	size_t sourceSize =  strlen(sProgramSource) ;
   	*program = clCreateProgramWithSource(*context, 1,(const char **) &sProgramSource, &sourceSize, err);
        OPENCL_CHECK_STATUS("error while creating program",*err);

        /*build program*/
        *err = clBuildProgram(*program,1,devices[0],NULL,NULL,NULL);
        OPENCL_CHECK_STATUS("error while building  program",*err);

 }// end of setExeEnvPrefixSum



void    scalarVectGMDP(cl_uint numDevices,cl_device_id *devices, cl_program program,cl_context context,double * h_VectA,double *h_Output,int vectSize,int h_Scalar)
{
        cl_event                gpuExec[1];
        cl_int err;
        cl_command_queue cmdQueue;   //holds command queue object
        cl_kernel kernel;               //holds kernel object
        cl_mem d_VectA,d_Length,d_Output ,d_Scalar;          //holds device input output buffer
        cl_event                events;        // events
        size_t globalWorkSize[1]={vectSize}; //holds global group size
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
        kernel = clCreateKernel(program,"scalVectMultKernelDp",&err);
        OPENCL_CHECK_STATUS("error while creating kernel",err);

        /*create buffer*/
       d_VectA=clCreateBuffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,sizeof(double)*vectSize,h_VectA,&err);
        OPENCL_CHECK_STATUS("error while creating buffer for input",err);

        d_Output=clCreateBuffer(context,CL_MEM_WRITE_ONLY,sizeof(double)*vectSize,NULL,&err);
        OPENCL_CHECK_STATUS("error while creating buffer for output",err);

	d_Scalar=clCreateBuffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,sizeof(int),&h_Scalar,&err);
        OPENCL_CHECK_STATUS("error while creating buffer scalar",err);

        /*set kernel arg*/
        err=clSetKernelArg(kernel,0,sizeof(cl_mem),&d_VectA);
        OPENCL_CHECK_STATUS("error while setting arg 0",err);
        
	err=clSetKernelArg(kernel,1,sizeof(cl_mem),&d_Output);
        OPENCL_CHECK_STATUS("error while setting arg 2",err);

        err=clSetKernelArg(kernel,2,sizeof(cl_mem),&d_Scalar);
        OPENCL_CHECK_STATUS("error while setting arg 3",err);


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
        err = clEnqueueReadBuffer(cmdQueue,d_Output,CL_TRUE,0,sizeof(cl_double)*vectSize,h_Output,0,0,&events);
        OPENCL_CHECK_STATUS("error while reading buffer",err);


        /* calculate total gflops*/
         gflops= (1.0e-9 * ( (vectSize) / executionTimeInSeconds));


        // Print the gflops on the screen
         print_on_screen("ScalarTimesVectorDpGlobalMemory",executionTimeInSeconds,vectSize,gflops,1);

        //release opencl objects
        clReleaseMemObject(d_VectA);
        clReleaseMemObject(d_Scalar);
        clReleaseMemObject(d_Output);
        clReleaseProgram(program);
        clReleaseKernel(kernel);
        clReleaseCommandQueue(cmdQueue);
        clReleaseContext(context);
}

int scalarVectCheckResultGMDP(double *h_VectA,double *output,int vectSize,int h_Scalar)
{
        int CurElementIndex ,counter;
        int i,flag=0;
        CurElementIndex = 0;
        double *temp_Out;
	double  eps=EPS;
	double relativeError=0.0;
	double  errorNorm = 0.0;

        assert((temp_Out = (double *)malloc( sizeof(double) * vectSize))!=NULL);

       for(counter = 0 ; counter < vectSize ; counter++)
       {
                        temp_Out[counter]= h_VectA[counter] * h_Scalar ;
       }


	/*** check relative error with approx precision ****/
        for( i = 0; i < vectSize; ++i)
        {
		if (fabs(temp_Out[i]) > fabs(output[i]))
	                relativeError = ((temp_Out[i] - output[i]) / temp_Out[i]);
		else
                        relativeError = fabs((output[i] - temp_Out[i]) / output[i]);
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

                printf(" \n \t\t Results verfication : Failed\n");
                printf(" \n \t\t Considered machine precision : %e\n", eps);
                printf(" \n \t\t Relative Error                  : %e\n", errorNorm);

        }
        else
        {

                printf("\n \n\t\tResults verfication : Success\n\n");
        }

                                                  
}

//void Execute_scalarVectGMDP(int SIZE,FILE *fp)
int main(int argc,char *argv[])
 {

        cl_platform_id selectedPlatform;      //holds list of platforms
        cl_uint numPlatforms;         //holds number of platforms
        cl_int err;                    //holds error (return value)
        cl_uint         numDevices;   /*hold the number of devices */
        cl_device_id    *devices;      /* hold list of devices */
        int count;
        cl_context context;           //holds context object
        cl_program program;           //holds program object
        double *h_VectA;                 //holds host input buffer
        double *h_Output;                //holds host output buffer
        int i ,h_Scalar;

                
        int vectSize=SIZE;
	h_Scalar=4;  //can be changed for different scalar value 
        /* allocate host memory*/
        assert((h_VectA=(double *)malloc(vectSize*sizeof(double)))!=NULL);
        assert((h_Output=(double *)malloc(vectSize*sizeof(double)))!=NULL);
        
        /*initialize host memory*/
        fill_dp_vector(h_VectA,vectSize);
        for(i=0;i<vectSize;i++)
        {
                h_Output[i]=0.0;
        }
 
        //setting the execution environment for opencl
        setExeEnvScalarVectGMDP( &context , &numDevices, &devices, &program,&numPlatforms,&selectedPlatform,&err );


        //function for Scalar Vector multiplication 
        scalarVectGMDP (numDevices, devices, program, context, h_VectA,h_Output,vectSize,h_Scalar);

	//check result
	scalarVectCheckResultGMDP(h_VectA, h_Output,vectSize,h_Scalar);

	/********************************************************
		uncomment to print output vector  on the screen
	********************************************************/
        /*for(i=0;i<vectSize;i++)
        {
                printf("%f\n",h_VectA[i]);
        }*/
        /* print buffer object*/
        /*for(i=0;i<vectSize;i++)
        {
                printf("%f\n",h_Output[i]);
        }*/

        //free host memory
        hDpVectFree(h_VectA,vectSize);
        hDpVectFree(h_Output,vectSize);

	return 0;
}


