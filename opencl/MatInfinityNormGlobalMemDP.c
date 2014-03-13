/*****************************************************************************

                         C-DAC Tech Workshop : HEMPA-2011
                             Oct 17 - 21, 2011

  Example     : MatInfinityNormGlobalMemDP.c
 
  Objective   : Calcualte Infinity Norm of Matrix
                (Double precision)

  Input       : None 

  Output      : Execution time in seconds , Gflops achieved
                                                                                                                            
  Created     : Aug 2011    

  E-mail      : betatest@cdac.in         
                                 
****************************************************************************/
/*source code for Matrix Infinity Norm*/
#include<CL/cl.h>
#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<string.h>
#include<math.h>


#define SIZE 128       // Modify SIZE to execute for different data 
                       // sizes and SIZE should be multiple of BLOCK_SIZE

#define EPS 1.0e-15    // threshhold aprrox epsilion value 

 
const char * matInfinityNormKernelPath = "MatInfinityNormGlobalMemDP_kernel.cl";

/* opencl check status mecro*/
#define OPENCL_CHECK_STATUS(NAME,ERR) {\
        const char *name=NAME;\
        cl_int Terr=ERR;\
        if (Terr != CL_SUCCESS) {\
                printf("\n\t\t Error: %s (%d) \n",name, Terr);\
                exit(-1);} }\

int matInfinityNormCheckResultGMDP(double *h_Mat,int rows,int cols,double *finalSum);

/* matrix free */
void hDpMatFree(double * arr,int len)
{
        int i;
        free(arr);
}

/* vector free*/
void hDpVectFree(double * arr,int len)
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



/* Fill in the matrix with single precision values */
void fill_dp_matrix(double* matrix,int rowSize,int colSize)
{
	int     row, col ;

        for( row=0; row < rowSize; row++)
             for( col=0; col < colSize; col++)
                        matrix[row * colSize + col] = drand48();

}


/* print result on the screen */
void print_on_screen(char * programName,double tsec,int size,double gflops,int flag)//flag=1 if gflops has been calculated else flag =0
{
        printf("\n\t---------------%s----------------\n\n",programName);
        printf("\t\tSIZE\t TIME_SEC\t gflops \n");
        if(flag==1)
        printf("\t\t%d\t%f\t %lf\t",size,tsec,gflops);
        else
        printf("\t\t%d\t%lf \t%s\t",size,tsec,"---");
        printf("\n\n\t----------------------------------------------------");
}

//free host vector memory
void hDpFree(double * arr,int len)
{
        int i;
        free(arr);
}

/********************************************************************
function to execute set execution env
********************************************************************/
void setExeEnvMatInfinityGMDP(cl_context *context, cl_uint *numDevices, cl_device_id **devices, cl_program *program,cl_uint *numPlatforms,cl_platform_id *platforms,cl_int *err)
{
        char            pbuff[100];              //holds platform information (platform name)
        char            dbuff[100];             //holds device information (platform name)
	size_t          kernelSrcLength;      /* hold the kernel source string length */
        char            *kernelSrcStr;       /* hold the kernel source string */
        int count;

	printf("\t--------------------------Device Details-----------------------------\n\n");
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
	char* programSource = readKernelSource(matInfinityNormKernelPath);
        size_t sourceSize =  strlen(programSource) ;

        *program = clCreateProgramWithSource(*context, 1,(const char **) &programSource, &sourceSize, err);
        OPENCL_CHECK_STATUS("error while creating program",*err);

        /*build program*/
        *err = clBuildProgram(*program,1,devices[0],NULL,NULL,NULL);
        OPENCL_CHECK_STATUS("error while building  program",*err);

        
}


/**************************************************************
function to calculate Matrix Infinity norm
*************************************************************/
void    matInfinityNormGMDP(cl_uint numDevices,cl_device_id *devices, cl_program program,cl_context context,double * h_Mat, int *h_rowcol, double *h_infi,int height,int width)
{
        cl_command_queue        cmdQueue;     // Command Queue  object
        cl_mem                  d_Mat;     //  device input buffer
        cl_mem                  d_rowcol;     //  device input buffer
        cl_mem                  d_infi;      // device output buffer
        cl_kernel               kernel;        //  kernel object
        cl_int                  err;            // Holds the error 
        cl_event                events;        // event object
        double                  totalTime=0.0; //holds total time taken for execution
        size_t                  globalWorkSize[1];    // holds global_work size
        size_t                  localWorkSize[1];    // holds local work size
        int                      count;
        char                    dbuff[100];          
        double                  gflops=0.0;             //holds total achieved gflops
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
        d_Mat =clCreateBuffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,(height*width)*sizeof(double),h_Mat,&err);
        OPENCL_CHECK_STATUS("Failed to create device input buffer A  ",err);

        
        d_rowcol =clCreateBuffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,2*sizeof(cl_int),h_rowcol,&err);
        OPENCL_CHECK_STATUS("Failed to create device input buffer d_rowcol  ",err);

   	 d_infi = clCreateBuffer ( context, CL_MEM_WRITE_ONLY , sizeof(double),NULL, &err);
        OPENCL_CHECK_STATUS( "Failed to create device output  buffer   ",err);

         // Create the kernel
         kernel = clCreateKernel ( program, "infinityNorm_kernel", &err);
         OPENCL_CHECK_STATUS(" Create kernel failed ",err);

          //  Set the arguments
     	err = clSetKernelArg( kernel, 0, sizeof(cl_mem), (void *) &d_Mat);
     	OPENCL_CHECK_STATUS( "Set  kernel argument 0 failed ",err);
           	
	err = clSetKernelArg( kernel, 1, sizeof(cl_mem), (void *) &d_rowcol);
        OPENCL_CHECK_STATUS( "Set  kernel argument 1 failed ",err);
        
   	err = clSetKernelArg( kernel, 2, sizeof(cl_mem), (void *) &d_infi);
        OPENCL_CHECK_STATUS( "Set  kernel argument 2 failed ",err);
           
         //set Global work size and local work size
        globalWorkSize [0]= height   ; // ND Range Size for each kernel launch 

         //launch the kernel
         err=clEnqueueNDRangeKernel(cmdQueue,kernel,1,NULL,globalWorkSize,NULL,0,NULL,&gpuExec[0]);
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
         err =clEnqueueReadBuffer(cmdQueue,d_infi,CL_TRUE,0,sizeof(cl_double),h_infi,0,0,&events);
         OPENCL_CHECK_STATUS(" Read output failed ",err);

	/* calculate gflops*/
        gflops= (1.0e-9 * ((1.0 *height*height) / executionTimeInSeconds));


        // Print the gflops on the screen
	 print_on_screen("Matrix Infinity Norm",executionTimeInSeconds,height,gflops,1);

        //free opencl objects
        if ( kernel )   clReleaseKernel(kernel);
        if ( cmdQueue) clReleaseCommandQueue(cmdQueue);
        if ( events )   clReleaseEvent(events);
	clReleaseMemObject(d_Mat);
        clReleaseMemObject(d_rowcol);
        clReleaseMemObject(d_infi);
}


/*****************************************************************
main function
*******************************************************************/
int main(int argc,char *argv[])
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
	cl_kernel kernel;		//holds kernel object
	double *h_Mat;			//holds host input buffer
	int *h_Rowcol;	
	double *h_InfiNorm;
	//float *h_inputA;		//holds host output buffer
	int i;
	int height=SIZE;
	int width=SIZE;
	
	/* allocate host memory*/
	h_Mat=(double *)malloc(height*width*sizeof(double));
	h_Rowcol=(int *)malloc(2*sizeof(int));
	h_InfiNorm=(double *)malloc(sizeof(double));
	h_Rowcol[0] = height;
  	h_Rowcol[1] = width;

	/*initialize host memory*/
	fill_dp_matrix(h_Mat,height,width);
	
	//function to set execution environment for opencl
	setExeEnvMatInfinityGMDP( &context , &numDevices, &devices, &program,&numPlatforms,&platforms,&err );

	//function to calculate Matrix Matrix Multiplication
	matInfinityNormGMDP(numDevices, devices, program, context, h_Mat,h_Rowcol,h_InfiNorm ,height,width);

	matInfinityNormCheckResultGMDP(h_Mat,height,width,h_InfiNorm);


	/* free the host memories*/
        hDpMatFree(h_Mat,height);
        hDpVectFree(h_InfiNorm,1);
	return 0;
}


/*****************************************************************
function to execute check result
******************************************************************/
int matInfinityNormCheckResultGMDP(double *h_Mat,int rows,int cols,double *cal_norm)
{
	int currRow,iCol,flag=0;
	double temp_norm =0.0,myRowSum=0.0;

	double  eps=EPS;
	double  relativeError=0.0;
	double  errorNorm = 0.0;

	/* sequential code to calculate Infinity norm*/
	for (currRow=0;currRow<rows;currRow++)
       	{
         
            myRowSum = 0;
            for(iCol = 0 ;iCol < cols; iCol++)
            myRowSum += h_Mat[currRow*cols+iCol];

            if(temp_norm < myRowSum )
            temp_norm = myRowSum;
       }


//	 check opencl result with sequential code 
	if (fabs(temp_norm) > fabs(*cal_norm))
                        relativeError = fabs((temp_norm - *cal_norm) / temp_norm);
                else
                        relativeError = fabs((*cal_norm - temp_norm) / *cal_norm);

                if (relativeError > eps && relativeError != 0.0e+00 )
                {
                        if(errorNorm < relativeError)
                        {
                                errorNorm = relativeError;
                                flag=1;
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
