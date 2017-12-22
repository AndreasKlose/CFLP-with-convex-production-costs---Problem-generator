# CFLP-with-convex-production-costs---Problem-generator
Modifies instances of the linear CFLP to include convex production costs (either quadratic or inverse cost functions)

# Files
- QuadCoeff.cpp : c++ code that reads in a problem instance of a linear CFLP and calculates coefficients of a quadratic (or, alternatively, inverse cost functio)
- CFLrdlp.c : c code that for reading in data of a linear CFLP
- CFLrdlp.h : header file of CFLrdlp.c
- Makefile : makefile for compiling on a Linux system; compiled executable is named "qcoeff"

# Usage
- qcoeff datafile 
- qcoeff datafile inverse

In the first case, a quadratic cost function of the type $h(u) = a + b*u^2$ is generated, where u is production/throughput at a depot, $a=0$ and $b$ the quadratic cost coefficient.

In the second case, an inverse cost function of type $h(u) = b/(s-u)$ is created, where s is a depot's capacity.
The cost data is appended to the data of the linear CFLP and all output send to the terminal.

# Requires

Data or the data generator for the linear CFLP instances (cf. https://github.com/AndreasKlose/CFLP-Instance-Generator)

