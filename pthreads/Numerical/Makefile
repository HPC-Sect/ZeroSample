
CC = gcc
CFLAGS = -w 
LDFLAGS =  -lpthread -lm

TARGETS = pthread-sumn pthread-numerical-integration  pthread-vectorvector-multi  pthread-infinitynorm-rowwise  pthread-infinitynorm-colwise  pthread-jacobi 

all: $(TARGETS) 

$(TARGETS):  %:%.o 
	$(CC) -o $@ $(CFLAGS) $^  $(LDFLAGS)
clean:
	$(RM) *.o $(TARGETS) 


