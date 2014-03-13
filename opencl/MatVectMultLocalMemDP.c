
/*****************************************************************************

                         C-DAC Tech Workshop : HEMPA-2011
                             Oct 17 - 21, 2011

  Example     : MatVectMulLocalMemDP.c
 
  Objective   : Perform matrix-vector multiplication using local memory
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
#include <math.h>
#define EPS 1.0e-14 /* threshhold aprrox epsilion value */
#define BLOCK_SIZE 16   // If  value is modified , modification 
#define SIZE 128       // Modify SIZE to execute for different data sizes
/* opencl check status mecro*/
#define OPENCL_CHECK_STATUS(NAME,ERR) {\
        const char *name=NAME;\
        cl_int Terr=ERR;\
        if (Terr != CL_SUCCESS) {\
                printf("\n\t\t Error: %s (%d) \n",name, Terr);\
                exit(-1);} }\

const char * matVectMultDpKernelPath = "MatVectMultLocalMemDP_kernel.cl";
int matVectMultCheckResultLMDP (double *h_Mat, double *h_Vect,double *output, int rows, int cols);

//free  host matrix memory
void hDpMatrixFree(double * arr,int len)
{
        free(arr);
}

//free host vector memory
void hDpVectFree(double * arr,int len)
{
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

/* Fill in the matrix with double precision values */
void fill_dp_matrix(double* matrix,int rowSize,int colSize)
{
        int     row, col ;
        for( row=0; row < rowSize; row++)
             for( col=0; col < colSize; col++)
                        matrix[row * colSize + col] = drand48();
}

/* Fill in the vector with double precision values */
void fill_dp_vector(double* vec,int size)
{
        int ind;
        for(ind=0;ind<size;ind++)
                vec[ind]=drand48();
}

/* print result on the screen */
void print_on_screen(char * program_name,double tsec,int size,double gflops,int flag)//flag=1 if gflops has been calculated else flag =0
{
        printf("\n\t---------------%s----------------\n\n",program_name);
        printf("\t\t\tSIZE\t TIME_SEC\t gflops \n");
        if(flag==1)
        printf("\t\t\t%d\t%f\t %lf\t",size,tsec,gflops);
        else
        printf("\t\t\t%d\t%lf \t%s\t",size,tsec,"---");
	printf("\n\n\t------------------------------------------------------------------------------------------");
}




/********************************************************************
function to execute set execution env
********************************************************************/
void setExeEnvMatVectMultLMDP(cl_context *context, cl_uint *numDevices, cl_device_id **devices, cl_program *program,cl_uint *numPlatforms,cl_platform_id *selectedPlatform,cl_int *err)
{
        char            pbuff[100];              //holds platform information (platform name)
        char            dbuff[100];             //holds device information (platform name)
        int count;
	printf("\t---------------------------Device Details----------------------------\n\n");
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
	printf("\tNumber of GPU  devices used                :  %d\n\n",*numDevices);
        if ( *err != CL_SUCCESS || *context == 0)
        {
                printf("\n\t No GPU detected ");
                printf("\n\t Context : %d , Err : %d",context, err);
                exit(-1);
        }

	printf("\t---------------------------------------------------------------------\n");
        /*create program with source*/
	     char* sProgramSource = readKernelSource(matVectMultDpKernelPath);
        size_t sourceSize =  strlen(sProgramSource) ;

	*program = clCreateProgramWithSource(*context, 1,(const char **) &sProgramSource, &sourceSize, err);
        OPENCL_CHECK_STATUS("error while creating program",*err);

        /*build program*/
        *err = clBuildProgram(*program,1,devices[0],NULL,NULL,NULL);
        OPENCL_CHECK_STATUS("error while building  program",*err);
}



/********************************************************************
function to execute Matrix Vector  Multiplication with double Prec
*********************************************************************/
void    matrixVectorMultLMDP (cl_uint numDevices,cl_device_id *devices, cl_program program,cl_context context,double * h_Mat, double *h_Vect, double *h_Output,int height,int width)
{

        cl_command_queue        cmdQueue;     // Command Queue  object
        cl_mem                  d_Mat, d_Vect;     //  device input buffer
        cl_mem                  d_rows, d_cols;     //  device input buffer
        cl_mem                  d_Output;      // device output buffer
        cl_kernel               kernel;        //  kernel object
        cl_int                  err;            // Holds the error 
        cl_event                events;        // event object
        size_t                  globalWorkSize[1];    // holds global_work size
        size_t                  localWorkSize[1];    // holds local work size
        int                      count,i;
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

        
        d_Vect =clCreateBuffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,width*sizeof(double),h_Vect,&err);
        OPENCL_CHECK_STATUS("Failed to create device input vector  ",err);

   	d_Output = clCreateBuffer ( context, CL_MEM_WRITE_ONLY ,height * sizeof(double),NULL, &err);
        OPENCL_CHECK_STATUS( "Failed to create device output  buffer   ",err);

        d_rows =clCreateBuffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,sizeof(cl_int),(void*)&height,&err);
        OPENCL_CHECK_STATUS( "Failed to create device output  buffer   ",err);

        d_cols =clCreateBuffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,sizeof(cl_int),(void*)&width,&err);
        OPENCL_CHECK_STATUS( "Failed to create device output  buffer   ",err);


         // Create the kernel
         kernel = clCreateKernel ( program, "matrixvectorMult", &err);
         OPENCL_CHECK_STATUS(" Create kernel failed ",err);

          //  Set the arguments
     	err = clSetKernelArg( kernel, 0, sizeof(cl_mem), (void *) &d_Mat);
     	OPENCL_CHECK_STATUS( "Set  kernel argument 0 failed ",err);
           	
	err = clSetKernelArg( kernel, 1, sizeof(cl_mem), (void *) &d_Vect);
        OPENCL_CHECK_STATUS( "Set  kernel argument 1 failed ",err);
        
   	err = clSetKernelArg( kernel, 2, sizeof(cl_mem), (void *) &d_Output);
        OPENCL_CHECK_STATUS( "Set  kernel argument 2 failed ",err);
           
	err = clSetKernelArg( kernel, 3, sizeof(cl_mem), (void *) &d_rows);
        OPENCL_CHECK_STATUS( "Set  kernel argument 3 failed ",err);
           
	err = clSetKernelArg( kernel, 4, sizeof(cl_mem), (void *) &d_cols);
        OPENCL_CHECK_STATUS( "Set  kernel argument 4 failed ",err);

	err = clSetKernelArg( kernel, 5, sizeof(cl_double)*BLOCK_SIZE*BLOCK_SIZE , 0);
        OPENCL_CHECK_STATUS( "Set  kernel argument 3 failed ",err);


        globalWorkSize [0]= height   ; // ND Range Size for each kernel launch 
        localWorkSize [0]= BLOCK_SIZE   ; // ND Range Size for each kernel launch 

         //launch the kernel
         err=clEnqueueNDRangeKernel(cmdQueue,kernel,1,NULL,globalWorkSize,localWorkSize,0,NULL,&gpuExec[0]);
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
         err =clEnqueueReadBuffer(cmdQueue,d_Output,CL_TRUE,0,height*sizeof(cl_double),h_Output,0,0,&events);
         OPENCL_CHECK_STATUS(" Read output failed ",err);

	/* calculate gflops*/
        gflops= (1.0e-9 * ((2.0 * height*width) / executionTimeInSeconds));

        // Print the gflops on the screen
	 print_on_screen("Matrix Vector Multiplication-double precision shared memory",executionTimeInSeconds,height,gflops,1);

        //free opencl objects
        if ( kernel )   clReleaseKernel(kernel);
        if ( cmdQueue) clReleaseCommandQueue(cmdQueue);
        if ( events )   clReleaseEvent(events);
	clReleaseMemObject(d_Mat);
        clReleaseMemObject(d_Vect);
        clReleaseMemObject(d_rows);
        clReleaseMemObject(d_cols);
        clReleaseMemObject(d_Output);
}



/*****************************************************************
function to execute main
*******************************************************************/
int main(int argc, char *argv[])
{
	cl_platform_id selectedPlatform;      //holds list of platforms
  	cl_uint numPlatforms;         //holds number of platforms
	cl_int err;                    //holds error (return value)
	cl_uint         numDevices;   /*hold the number of devices */
        cl_device_id    *devices;      /* hold list of devices */
	int count;
	cl_context context;           //holds context object
	cl_program program;           //holds program object
	cl_kernel kernel;		//holds kernel object
	double *h_Mat;			//holds host input buffer
	double *h_Output;		//holds host output buffer
	double *h_Vect;		//holds host output buffer
	int i;
	int height=SIZE;
	int width=SIZE;
	
	/* allocate host memory*/
	assert((h_Mat=(double *)malloc(height*width*sizeof(double)))!=NULL);
	assert((h_Output=(double *)malloc(height*sizeof(double)))!=NULL);
	assert((h_Vect=(double *)malloc(width*sizeof(double)))!=NULL);

	/*initialize host memory*/
	fill_dp_matrix(h_Mat,height,width);
	fill_dp_vector(h_Vect,width);
	for(i=0;i<height;i++)
	{
		h_Output[i]=0;
	}
	
	//function to set execution environment for opencl
	setExeEnvMatVectMultLMDP( &context , &numDevices, &devices, &program,&numPlatforms,&selectedPlatform,&err );

	//function to calculate Matrix Vector Multiplication
	matrixVectorMultLMDP(numDevices, devices, program, context, h_Mat,h_Vect, h_Output,height,width);


	// check opencl results with cpu results

	  matVectMultCheckResultLMDP( h_Mat,h_Vect,h_Output, height , width );



/********************************************************
uncomment to print on the screen
********************************************************/
	/* print buffer object*/
	/*for(i=0;i<height;i++)
	{
		printf("%f\n",h_Output[i]);
	}
*/
	/* free the host memories*/
        hDpMatrixFree(h_Mat,height);
        hDpVectFree(h_Vect,width);
        hDpVectFree(h_Output,height);
}




/*****************************************************************
function to execute check result
******************************************************************/
int matVectMultCheckResultLMDP (double *h_Mat, double *h_Vect,double *output, int rows, int cols)
{
	long int colIndex,count;
	double tempResult = 0.0;
	double *temp_Out;
	int i,j,flag=0;
	double  errorNorm = 0.0;
        double  eps=EPS;
        double  relativeError=0.0;
        
        assert((temp_Out = (double *)malloc( sizeof(double) * rows))!=NULL);
	colIndex = 0;
	/* calculate sequential Matrix Vector Multiplication*/
	while(colIndex != rows)
	{
		tempResult = 0.0;
		for(count = 0 ; count < cols; count++ )
		{
			tempResult +=  h_Vect[count] * h_Mat[count + rows   * colIndex];
		}
		temp_Out[colIndex] = tempResult;
		colIndex++;
	}
	/* check opencl result with sequential result*/
	for(i=0;i<rows;i++)
	{
		if (fabs(temp_Out[i]) > fabs(output[i]))
                        relativeError = fabs((temp_Out[i] - output[i]) / temp_Out[i]);
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

                printf(" \n\t\t Results verfication : Failed");
                printf(" \n\t\t Considered machine precision : %e", eps);
               printf(" \n\t\t Relative Error                  : %e\n", errorNorm);

        }
        if(flag==0)
        {
                printf("\n\n\t\tResult Verification success\n\n");
        }

	free(temp_Out);
	return 0;

}
