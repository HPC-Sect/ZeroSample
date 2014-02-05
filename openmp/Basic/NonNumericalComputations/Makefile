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

#OBJECTS = omp-maxof-elements-critical.c 
#OBJECTS = omp-maxof-elements-lock.c
#OBJECTS = omp-prime-datarace-condt.c
#----------------------------------------------------------------------

run:$(OBJECTS)
	$(CC) $(OPTFLAGS) -o $@ $(OBJECTS) $(CCFLAGS) $(LIBS) -lm

.c.o:
	$(CC) -c $(OPTFLAGS) $(CCFLAGS) $<

clean:
	\rm -rf *.o run

