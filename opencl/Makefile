##---------- Makefile for opencl codes----------------
##---------- uncomment the necessary object to compile----------------


CC=g++


#CLHOME=/usr/local/cuda
#CLINC=-I$(CLHOME)/include
#CLINKLIB=-L$(CLHOME)/lib64
#CLLINKFLAGS=-lOpenCL
CLINC = -I/usr/local/AMD-APP-SDK-v2.4-lnx64/include
CLINKLIB=-L/usr/local/AMD-APP-SDK-v2.4-lnx64/lib/x86_64
CLLINKFLAGS=-lOpenCL


# Names of set of example programs
OBJECT=ScalarVectGlobalMemDP
#OBJECT=ScalarVectGlobalMemSP
#OBJECT=ScalarMatrixGlobalMemSP
#OBJECT=ScalarMatrixGlobalMemDP
#OBJECT=MatMatAddGlobalMemSP
#OBJECT=MatMatAddGlobalMemDP
#OBJECT=MatMatMultLocalMemSP
#OBJECT=MatMatMultLocalMemDP
#OBJECT=MatInfinityNormGlobalMemSP
#OBJECT=MatInfinityNormGlobalMemDP
#OBJECT=PieCalculationGlobalMemSP
#OBJECT=PieCalculationGlobalMemDP
#OBJECT=PrefixSumGlobalMemSP
#OBJECT=PrefixSumGlobalMemDP
#OBJECT=MatTransposeGlobalMemSP
#OBJECT=MatTransposeGlobalMemDP
#OBJECT=MatTransposeLocalMemSP
#OBJECT=MatTransposeLocalMemDP
#OBJECT=MatVectMultGlobalMemSP
#OBJECT=MatVectMultLocalMemSP
#OBJECT=MatVectMultGlobalMemDP
#OBJECT=MatVectMultLocalMemDP
#OBJECT=VectVectAddGlobalMemSP
#OBJECT=VectVectAddGlobalMemDP
#OBJECT=VectVectMultGlobalMemSP
#OBJECT=VectVectMultGlobalMemDP
#OBJECT=MatMatMultGlobalMemSP
#OBJECT=MatMatMultGlobalMemDP
#OBJECT=clMultiGPU-VectVectAdd


run:
	$(CC) $(CLLINKFLAGS) $(CLINC) $(CLINKLIB) -o $(OBJECT) $(OBJECT).c

clean:
	rm -f  $(OBJECT)

