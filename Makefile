#===============================================================================
# makefile for program qcoeff 
# 
#===============================================================================

###########
#compilers:
###########
CXX = g++
CC = gcc

#Input format of the CFLP data file

#INPUT_FORMAT =CFL_AVELLA
#INPUT_FORMAT =CFL_ORLIB
INPUT_FORMAT=CFL_KLOSE

# Correspond the rows of cost matrix to customers or depots?
MATRIX = CFL_MATRIX_ROW_CUST_FORMAT
#MATRIX=CFL_MATRIX_ROW_DEPOT_FORMAT

################
#compiler flags:
################
FLAGS = -g -DIL_STD -I$(SUBROU)
CFLAGS = $(FLAGS) -D$(INPUT_FORMAT) -D$(MATRIX)
CXXFLAGS = $(FLAGS)

###################
#libraries required
###################
LNFLAGS = -lm

######################
# objects to be linked
######################
obj = QuadCoeff.o CFLrdlp.o

####################
# make rules follow:
####################
qcoeff: QuadCoeff.o CFLrdlp.o
	$(LINK.C) $(obj) $(LNFLAGS) -o $@

QuadCoeff.o: QuadCoeff.cpp
	$(COMPILE.C) $< -o $@ 

CFLrdlp.o: $(SUBROU)/CFLrdlp.c
	$(COMPILE.c) $< -o $@

clean-o:
	rm -f $(obj)
clean:
	rm -f $(obj) qcoeff
