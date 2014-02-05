
CC = gcc
CFLAGS = -w 
LDFLAGS =  -lpthread -lm

TARGETS =   pthread-demo-datarace pthread-helloworld pthread-join  pthread-stack  pthread-mutex  pthread-conditionvariable 

all: $(TARGETS) 

$(TARGETS):  %:%.o 
	$(CC) -o $@ $(CFLAGS) $^  $(LDFLAGS)
clean:
	$(RM) *.o $(TARGETS) 


