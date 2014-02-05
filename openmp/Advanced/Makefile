#/*****************************************************************************
#                     Makefile
#          
# Description          : This file is used to compile and link the openmp programs.
#                        The user has to specify the names of the program and appropriate 
#                        compiler roptions/paths to link some of the libraries required for 
#                        OpenMP programs in the Makefile.
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

SHELL=/bin/sh

#CC = /usr/local/gcc-4.4.3/bin/gcc
#CFLAGS= -fopenmp
CC = icc
CFLAGS= -openmp

###################### compiling all source code ######################## 

CURRENT_OBJ=icv-threads-openmp3x loop-construct-matvect-openmp3x mem-consistency-openmp3x task-construct-alg-openmp3x link-list-alg-openmp3x parallel-construct-openmp3x tree-traverse-alg-openmp3x loop-collapse-openmp3x mem-cond-compile-openmp3x 

default:$(CURRENT_OBJ)
 
$(CURRENT_OBJ):  %:%.o  
	$(CC) -o $@ $^  $(CLINKFLAGS)  $(CFLAGS)
	@rm -f *.o
.c.o:%:%.c
	@$(CC)  $(CFLAGS) -c $< $(C_LIB) $(C_INC)


###################### compiling all source code End ######################## 


clean: 
	rm -f *.o $(CURRENT_OBJ)


