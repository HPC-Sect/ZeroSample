CC = nvcc
CFLAGS = -O2

all: globalMemoryAccessPatterns coalescedFloat3Access deviceDetails sharedMemoryRestructuringDataTypes sharedMemoryStridedAccessPatterns sharedMemoryReadingSameWord SOAvsAOS 

deviceDetails: deviceDetails.cu
	$(CC) $(CFLAGS) deviceDetails.cu -o deviceDetails

coalescedFloat3Access: coalescedFloat3Access.cu
	$(CC) $(CFLAGS) coalescedFloat3Access.cu -o coalescedFloat3Access

globalMemoryAccessPatterns: globalMemoryAccessPatterns.cu
	$(CC) $(CFLAGS) globalMemoryAccessPatterns.cu -o globalMemoryAccessPatterns

sharedMemoryRestructuringDataTypes: sharedMemoryRestructuringDataTypes.cu
	$(CC) $(CFLAGS) sharedMemoryRestructuringDataTypes.cu -o sharedMemoryRestructuringDataTypes

sharedMemoryStridedAccessPatterns: sharedMemoryStridedAccessPatterns.cu
	$(CC) $(CFLAGS) sharedMemoryStridedAccessPatterns.cu -o sharedMemoryStridedAccessPatterns

sharedMemoryReadingSameWord: sharedMemoryReadingSameWord.cu
	$(CC) $(CFLAGS) sharedMemoryReadingSameWord.cu -o sharedMemoryReadingSameWord

SOAvsAOS: SOAvsAOS.cu
	$(CC) $(CFLAGS) SOAvsAOS.cu -o SOAvsAOS

clean:
	rm -f globalMemoryAccessPatterns coalescedFloat3Access deviceDetails 
	rm -f sharedMemoryRestructuringDataTypes sharedMemoryReadingSameWord sharedMemoryStridedAccessPatterns SOAvsAOS


