#*******************************************************************
#                         C-DAC Tech Workshop : HEMPA-2011
#                             Oct 17 - 21, 2011
#
#comment	: Makefile for compiling MemcpyLatencyHiding package.
#
#
#*******************************************************************

#
#------------------------------------------------------
PROG = MemcpyLatencyHiding.cu
#------------------------------------------------------


# Cuda C++ compiler
CXX=nvcc 
INCLUDE = 
LIB = 

all:	release 

release: 
	$(CXX) -O2 -DNDEBUG $(CXXFLAGS) -o run $(PROG) -I$(INCLUDE) -L$(LIB) 
clean:
	rm -f run *.o *.d output.txt
test:
	./$(PROG) 1000
