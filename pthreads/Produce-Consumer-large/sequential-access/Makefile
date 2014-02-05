
CC = gcc
CFLAGS = -w 
LDFLAGS =  -lpthread -lm

TARGETS =   prod_consumer_cond_multi_access prod_consumer_mutex_multi_access

all: $(TARGETS) 

$(TARGETS):  %:%.o 
	$(CC) -o $@ $(CFLAGS) $^  $(LDFLAGS)
clean:
	$(RM) *.o $(TARGETS) 


