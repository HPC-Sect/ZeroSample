#
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

CFLAGS    = -DMPICH_IGNORE_CXX_SEEK

### End User configurable options ###

default: run

OBJECTS=mpicpp-diag-comm.C
#OBJECTS=mpicpp-pie-Collective.C
#OBJECTS=mpicpp-reduce.C
#OBJECTS=mpicpp-scatter.C
#OBJECTS=mpicpp-vv-mult-blk-cyclic.C
#OBJECTS=mpicpp-vv-mult-blkstp-nonunf.C
#OBJECTS=mpicpp-vv-mult-blkstp-unf.C


run: $(OBJECTS)
	$(CC) -o run $(OBJECTS) $(LIBS) $(CFLAGS)

.c.o:
	$(CC) -c $<

	
clean: 
	\rm -f *.o run

