#Makefile for running cuda bandwidth assignment
#---------------------------------------------------------------------
#               Machines Architecture :
#---------------------------------------------------------------------
CC   = /usr/local/cuda/bin/nvcc
CCLINKER   =  $(CC)
COPTION     =
OPTFLAGS    =
CCFLAGS     =
#CCFLAGS     = -deviceemu
LIBS        =
EXECS       = bandwidthExe 
#---------------------------------------------------------------------
#               Object files ...
#---------------------------------------------------------------------

OBJECTS = CudaBandwidthCal-v1.1.cu 
#----------------------------------------------------------------------

$(EXECS):$(OBJECTS)
	$(CCLINKER) $(CCFLAGS) $(OPTFLAGS) -o $@ $(OBJECTS) $(LIBS) 


clean:
	\rm -rf *.o $(EXECS) 


