#CC = gcc
CC = g++
CFLAGS = -w
LDFLAGS =  -lpthread -lm

TARGETS =  pthread_prod_cons_conditionVariable pthread_prod_cons_large

all: $(TARGETS)

$(TARGETS):  %:%.o
	$(CC) -o $@ $(CFLAGS) $^  $(LDFLAGS)
clean:
	$(RM) *.o $(TARGETS)
