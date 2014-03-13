/*****************************************************************************

                         C-DAC Tech Workshop : HEMPA-2011
                             Oct 17 - 21, 2011

  Example     : MatTransposeGlobalMemSP.c
 
  Objective   : Perform matrix transpose operation using global memory
                (single precision)

  Input       : None 

  Output      : Execution time in seconds
                                                                                                                            
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

#define EPS 1.0e-8f    // threshhold aprrox epsilion value                        


/* opencl check status mecro*/
#define OPENCL_CHECK_STATUS(NAME,ERR) {\
        const char *name=NAME;\
        cl_int Terr=ERR;\
        if (Terr != CL_SUCCESS) {\
                printf("\n\t\t Error: %s (%d) \n",name, Terr);\
                exit(-1);} }\

const char * transposeMatGlobalMemKernelPath = "MatTransposeGlobalMemSP_kernel.cl";
int matTransposeCheckResultGMSP(float *h_Mat,float *output,int rows,int cols);

/* matrix free */
void hSpMatFree(float * arr,int len)
{               
        int i;  
        free(arr);   
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

        
/* Fill in the matrix with single precision values */     
void fill_sp_matrix(float* matrix,int rowSize,int colSize)
{
        int     row, col;
        for( row=0; row < rowSize; row++)
             for( col=0; col < colSize; col++)
                        matrix[row * colSize + col] = rand()%10;

}


/* print result on the screen */
void print_on_screen(char * programName,double tsec,int size,double gflops,int flag)
{
        printf("\n\t---------------%s----------------\n\n",programName);
        printf("\t\t\tSIZE\t TIME_SEC\t gflops \n");
        if(flag==1)
        printf("\t\t\t%d\t%f\t %lf\t",size,tsec,gflops);
        else
        printf("\t\t\t%d\t%lf \t%s\t",size,tsec,"---");
        printf("\n\n\t-----------------------------------------------------------------");
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


/*************************************************************
function to execute set execution environment
**************************************************************/
void setExeEnvMatTransposeGMSP(cl_context *context, cl_uint *numDevices, cl_device_id **devices, cl_program *program,cl_uint *numPlatforms,cl_platform_id *selectedPlatform,cl_int *err)
{
        char            pbuff[100];   //holds platform information (platform name)
        char            dbuff[100];   //holds device information (platform name)
        cl_platform_id *platforms;
        int count;
	
	printf("\t---------------------------Device Deatils------------------------------\n\n");
         /*  Get the number of OpenCL Platforms Available */
        *err = clGetPlatformIDs ( 0, 0, numPlatforms);
        if( *err != CL_SUCCESS || *numPlatforms == 0) {
                printf(" \n\t\t No Platform Found \n");
                exit(0);
        }
        else
        {
                if( *numPlatforms == 0)
                {
                        printf(" \n\t\t No Platform Found \n");
                        exit(0);
                }
               else
                {
                        /* Allocate the space for available platform*/
                        assert( (platforms = (cl_platform_id *) malloc( sizeof(cl_platform_id) * (*numPlatforms))) != NULL);
                        /*  Get available OpenCL Platforms IDs*/
                        *err = clGetPlatformIDs( *numPlatforms, platforms, NULL);
                        OPENCL_CHECK_STATUS(" Failed to get Platform IDs",*err);
                        for ( count = 0 ; count < *numPlatforms ; count++)
                        {
                                /* get platform info*/
                                *err=clGetPlatformInfo(platforms[count],CL_PLATFORM_NAME,sizeof(pbuff),pbuff,NULL);
                                OPENCL_CHECK_STATUS ("clGetPlatformInfo Failed ",*err);
                                /* get device id and info*/
                                *err = clGetDeviceIDs( platforms[count], CL_DEVICE_TYPE_GPU,0,0,numDevices);
                                if( *err != CL_SUCCESS  || *numDevices == 0)
                                {
                                         continue;
                                }
                                else
                                {
                                       assert(((*devices)= (cl_device_id *) malloc( sizeof(cl_device_id ) *(*numDevices))) != NULL);
                                        *err = clGetDeviceIDs( platforms[count], CL_DEVICE_TYPE_GPU, (*numDevices), *devices, 0);
                                        /* get selected platform*/
                                        *selectedPlatform=platforms[count];
                                        printf("\tPlatform used                            :  %s\n",pbuff);
                                        break;
                                }
                        }
                }
        }


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

	printf("\n\t---------------------------------------------------------------------\n");
        /*create program with source*/
	 char* programSource = readKernelSource(transposeMatGlobalMemKernelPath);
        size_t sourceSize =  strlen(programSource) ;
	 *program = clCreateProgramWithSource(*context, 1,(const char **) &programSource, &sourceSize, err);
       // *program = clCreateProgramWithSource(*context,1,(const char **) &ProgramSourceMatMatAdd,NULL,err );
        OPENCL_CHECK_STATUS("error while creating program",*err);

        /*build program*/
        *err = clBuildProgram(*program,1,devices[0],NULL,NULL,NULL);
        OPENCL_CHECK_STATUS("error while building  program",*err);

        
}

/***********************************************************************************
function to perform Matrix Matrix Addition
**************************************************************************************/

void    matrixTransposeGMSP (cl_uint numDevices,cl_device_id *devices, cl_program program,cl_context context,float * h_Mat, float *h_Output,int height,int width)
{
        cl_int err;
        cl_command_queue cmdQueue;   //holds command queue object
        cl_kernel kernel;               //holds kernel object
        cl_mem d_Mat,d_rows,d_Output;            //holds device input output buffer
	int workgroup=height;
	size_t globalWorkSize[2]={workgroup,workgroup}; //holds global group size
        double                  gflops=0.0;             //holds total achieved gflops
        cl_ulong startTime, endTime,elapsedTime;       //holds time
        float executionTimeInSeconds;                 //holds total execution time
	cl_event                events; 
         cl_event                gpuExec[1];        // events

	//create command queue
        cmdQueue = clCreateCommandQueue(context, devices[0], CL_QUEUE_PROFILING_ENABLE, &err);
        if( err != CL_SUCCESS ||  cmdQueue == 0)
         {
               printf("\n\t Failed to create command queue  \n" );
               exit (-1);
         }

        /*create kernel object*/
        kernel = clCreateKernel(program,"transMatrix",&err);
        OPENCL_CHECK_STATUS("error while creating kernel",err);

        /*create buffer*/

        d_Mat=clCreateBuffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,sizeof(float)*(height*width),h_Mat,&err);
        OPENCL_CHECK_STATUS("error while creating buffer for input",err);
      
	d_rows=clCreateBuffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,sizeof(float),(void *)&height,&err);
        OPENCL_CHECK_STATUS("error while creating buffer for input",err);
        
	d_Output=clCreateBuffer(context,CL_MEM_WRITE_ONLY,sizeof(float)*(height*width),NULL,&err);
        OPENCL_CHECK_STATUS("error while creating buffer for output",err);


        /*set kernel arg*/
        err=clSetKernelArg(kernel,0,sizeof(cl_mem),(void *)&d_Mat);
        OPENCL_CHECK_STATUS("error while setting arg 1",err);

        err=clSetKernelArg(kernel,1,sizeof(cl_mem),(void *)&d_Output);
        OPENCL_CHECK_STATUS("error while setting arg 1",err);

        err=clSetKernelArg(kernel,2,sizeof(cl_mem),(void *)&d_rows);
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

	/* total alapsed time*/
        elapsedTime = endTime-startTime;
	/* total execution time*/
        executionTimeInSeconds = (float)(1.0e-9 * elapsedTime);

        /* reading buffer object*/
        err = clEnqueueReadBuffer(cmdQueue,d_Output,CL_TRUE,0,sizeof(cl_float)*height*width,h_Output,0,0,&events);
        OPENCL_CHECK_STATUS("error while reading buffer",err);

        // Print the gflops on the screen
         print_on_screen("Matrix Tranpose using global memory",executionTimeInSeconds,height,gflops,0);

	//release opencl objects
        clReleaseMemObject(d_Mat);
        clReleaseMemObject(d_rows);
        clReleaseMemObject(d_Output);
        clReleaseProgram(program);
        clReleaseKernel(kernel);
        clReleaseCommandQueue(cmdQueue);
        clReleaseContext(context);
}


/***********************************************
function to execute main
*************************************************/
int main(int argc,char *argv[])
{
	cl_platform_id platforms;      //holds list of platforms
  	cl_uint numPlatforms;         //holds number of platforms
	cl_int err;                    //holds error (return value)
	cl_uint         numDevices;   /*hold the number of devices */
        cl_device_id    *devices;      /* hold list of devices */
	int count;
	cl_context context;           //holds context object
	cl_command_queue cmdQueue;   //holds command queue object
	cl_program program;           //holds program object
	size_t retValSize;
	cl_kernel kernel;		//holds kernel object
	float *h_Mat;			//holds host input buffer
	float *h_Output;		//holds host output buffer
	int i;

	int height=SIZE;         //set height of matrix
	int width=SIZE;		//set width of matrix
	/* allocate host memory*/
	assert((h_Mat=(float *)malloc(height*width*sizeof(float)))!=NULL);
	assert((h_Output=(float *)malloc(height*width*sizeof(float)))!=NULL);
	
	
	/*initialize host memory*/
	fill_sp_matrix(h_Mat,height,width);
	for(i=0;i<height*width;i++)
	{
		h_Output[i]=0;
	}

	//set execution environment for opencl
	setExeEnvMatTransposeGMSP( &context , &numDevices, &devices, &program,&numPlatforms,&platforms,&err );

	//function to calculate Matrix Matrix addition
	matrixTransposeGMSP(numDevices, devices,program,context,h_Mat,h_Output,height,width);
	
	//check opencl result with sequential result
	matTransposeCheckResultGMSP(h_Mat,h_Output,height,width);
	

/********************************************************
uncomment to print it on the screen
********************************************************/
	/*for(i=0;i<height*height;i++)
	{
		printf("%f\n",h_MatA[i]);
	}*/
	/* print buffer object*/
/*	for(i=0;i<height*height;i++)
	{
		printf("%f\n",h_output[i]);
	}*/

	/* free host memory*/
        hSpMatFree(h_Mat,height);
        hSpMatFree(h_Output,height);
}

/************************************************************
function to check the result with sequential result
***************************************************************/
int matTransposeCheckResultGMSP(float *h_Mat,float *output,int rows,int cols)
{
	int i,j,count,flag=0;
	float *temp_out;

	float  eps=EPS;
	float  relativeError=0.0f;
	float  errorNorm = 0.0f;

	assert((temp_out = (float *)malloc( sizeof(float) * rows*cols))!=NULL);
	int colIndex=0;
	while(colIndex != rows)
        {
                for(count = 0 ; count < cols; count++ )
                {
                        temp_out[colIndex+rows*count] =  h_Mat[count + rows   * colIndex];
                }
                colIndex++;
        }

	/*** check relative error with approx precision ****/
        for( i = 0; i < rows*cols; ++i)
        {

                if (fabs(temp_out[i]) > fabs(output[i]))
                        relativeError = fabs((temp_out[i] - output[i]) / temp_out[i]);
                else
                        relativeError = fabs((output[i] - temp_out[i]) / output[i]);

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

	 /* check opencl result with sequential result
        for( j=0 ; j < rows*cols  ; j++)
        {        
                 if ( (( temp_out[j] - output[j])/temp_out[j]) > 1.0e-6f && (( temp_out[j] - output[j])/temp_out[j]) != 0.0e+00)         
                        {       
                                printf("\n\n\tRelative error %e\n",(( temp_out[j] - output[j])/temp_out[j]));
                                flag=1;
                               return 1;
                        }
        
        }
        if(flag==0)
        {       
                printf("\n\n\tResult verification success:\n\n");
        }*/
        free(temp_out);
        //return 0;



}
