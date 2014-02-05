
CC = gcc
CFLAGS = -w 
LDFLAGS =  -lpthread -lm

TARGETS =    pthread-findminimumvalue   pthread-finding-k-matches

all: $(TARGETS) 

$(TARGETS):  %:%.o 
	$(CC) -o $@ $(CFLAGS) $^  $(LDFLAGS)
clean:
	$(RM) *.o $(TARGETS) 


