#/*****************************************************************************
#                      
#                     Makefile_C
#          
# Description          : This file is used to compile and link the openmp programs.
#                        The user has to specify the names of the program and appropriate 
#                        compil roptions/paths to link some of the libraries required for 
#                        OpenMP programs in the Makefile.
#
# Created               : Aug 2011 
#
# Author                : RarchK
#
#
#*********************************************************************************/





#---------------------------------------------------------------------
#            Compiler
#           ---------- 
# Use the appropriate CC (compiler) and CCFLAGS ( compiler option) values
# to "turn on" the OpenMP Compilation.
# Following are the compiler and compiler option to turn on the 
# OpenMP compilation : 
#
#      Compiler(CC)                    Compiler Option (For CCFLAGS) 
#      -------------                   -------------------------------
# 1)   GNU   (gcc)                              -fopenmp  
# 2)   SUN   (suncc)                            -xopenmp 
# 3)   INTEL (icc)                              -openmp
# 4)   PGI   (pgcc)                             -mp 
# 5)   PATHScale (pathcc)                       -openmp
# 6)   IBM (xlc)                                -qsmp=omp
#---------------------------------------------------------------------

CC   = icc 

CCFLAGS   = -openmp
OPTFLAGS    =  -O3

LIBS    = 
EXECS       =  run
#---------------------------------------------------------------------
#		Object files ...
#---------------------------------------------------------------------
#

#OBJECTS = omp-hello-world.c
#OBJECTS = omp-unique-threadid.c
#OBJECTS = omp-recurrence.c
#OBJECTS = omp-sumof-elements.c
#OBJECTS = omp-sumof-elements-reduction.c
OBJECTS = omp-loop-carried-depend.c  
#OBJECTS = omp-matmat-schedule.c  
#OBJECTS = omp-shared-private-data.c
#OBJECTS = omp-workshare-section.c
#OBJECTS = omp-loop-invert.c  
 
#OBJECTS = omp-pi-calculation.c 
#OBJECTS = omp-pi-calculation-reduction.c

#----------------------------------------------------------------------

run:$(OBJECTS)
	$(CC) $(OPTFLAGS) -o $@ $(OBJECTS) $(CCFLAGS) $(LIBS) -lm

.c.o:
	$(CC) -c $(OPTFLAGS) $(CCFLAGS) $<

clean:
	\rm -rf *.o run

