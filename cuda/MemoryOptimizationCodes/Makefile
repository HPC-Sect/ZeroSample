CC = nvcc
CFLAGS = -O2

all: blockPartitioning vectorModel partitionCamping warpDivergence

#cudaStream: cudaStream.h cudaStreamKernels.cu cudaStream.cu
#	$(CC) $(CFLAGS) cudaStreamKernels.cu cudaStream.cu -o cudaStream

blockPartitioning: blockPartitioning.cu
	$(CC) $(CFLAGS) blockPartitioning.cu -o blockPartitioning

vectorModel: vectorModel.cu
	$(CC) $(CFLAGS) vectorModel.cu -o vectorModel

partitionCamping: partitionCamping.cu
	$(CC) $(CFLAGS) partitionCamping.cu -o partitionCamping

warpDivergence: warpDivergence.cu
	$(CC) $(CFLAGS) warpDivergence.cu -o warpDivergence



clean:
	rm -f blockPartitioning
	rm -f vectorModel
	rm -f partitionCamping
	rm -f warpDivergence
	rm -f *~
