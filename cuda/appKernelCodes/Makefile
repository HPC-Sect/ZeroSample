#Makefile for running cuda practice programs
#---------------------------------------------------------------------
#               Machines Architecture :
#---------------------------------------------------------------------
CC   =  nvcc
CCLINKER   =  $(CC)
COPTION     =
OPTFLAGS    =  
CCFLAGS     =  -arch sm_20
#CCFLAGS     = -deviceemu
LIBS        =
EXECS       =  run
#---------------------------------------------------------------------
#               Object files ...
#---------------------------------------------------------------------

OBJECTS = cuda-vector-vector-multiplication_GlobalMemory.cu
#OBJECTS = cuda-vector-vector-multiplication_SharedMemory.cu 
#OBJECTS = cuda-vector-vector-multiplication-mGPU.cu

#OBJECTS = cuda-PI-computation.cu
#OBJECTS =  cuda-prefix-sum.cu  

#OBJECTS=cuda-matrix-matrix-multiplication.cu
#OBJECTS=cuda-matrix-matrix-multiplication-mGPU.cu

#----------------------------------------------------------------------

run:$(OBJECTS)
	$(CCLINKER) $(CCFLAGS) $(OPTFLAGS) -o $@ $(OBJECTS) $(LIBS) 


clean:
	\rm -rf *.o run

