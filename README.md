# CFLP-with-convex-production-costs---Problem-generator
Modifies instances of the linear CFLP to include convex production costs (either quadratic or inverse cost functions)

# Files
- QuadCoeff.cpp : C++ code that reads a problem instance of a linear CFLP and calculates the coefficients of a quadratic or, alternatively, inverse cost function.
- CFLrdlp.c : C code that for reading data of a linear CFLP.
- CFLrdlp.h : header file of CFLrdlp.c.
- Makefile : makefile for compiling on a Linux system; compiled executable is named "qcoeff".

# Usage
- qcoeff datafile 
- qcoeff datafile inverse

In the first case, a quadratic cost function of the type $h(u) = a + b*u^2$ is generated, where u is the total demand allocated to to a depot, $a=0$ and $b$ is he quadratic cost coefficient. In the second case, an inverse cost function of type $h(u) = b/(s-u)$ is created, where s is a depot's capacity.

The output, i.e. the cost data for each potential depot site, is send to the terminal.

# Requires

Data or the data generator for the linear CFLP instances (cf. https://github.com/AndreasKlose/CFLP-Generator)

