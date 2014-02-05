#
# *******************************************************************
#		 C-DAC Tech Workshop PEMG-2010 
#		     June 21 - 24, 2010
#
# *******************************************************************
#   Modified            : April 2010
#   Created             : May 2008
#                         National PARAM Supercomputing Facility
#
# *******************************************************************

##### User configurable options #####

CC          =  mpicxx

LIBS        =  -lm 

CFLAGS      = -DMPICH_IGNORE_CXX_SEEK


### End User configurable options ###

default: run


#OBJECTS=mpicpp-hello-world.C
#OBJECTS=mpicpp-pie-pt-to-pt.C
#OBJECTS=mpicpp-sum-associative-fanin-blocking-tree.C
OBJECTS=mpicpp-sum-pt-to-pt.C
#OBJECTS=mpicpp-sum-ring-topology.C
#OBJECTS=sum-pt-to-pt.C

run: $(OBJECTS)
	$(CC) -o run $(CFLAGS) $(OBJECTS) $(LIBS)  

.c.o:
	$(CC) -c $<

	
clean: 
	\rm -f *.o run

