/*=========================================================================

FILE      : CFLrdlp.h ( header file for c-module CFLrdlp.c )

VERSION   : 1.11
DATE      : 01/12/2005
            06/08/2008 (Adapted such that also instances of multi-type
                        facility location problems can be read ) 
            07/08/2008 Routine ReadDLocprob simpliefied a bit
            25/01/2011 (optional lower limit on throughput of facilities introduced)
	    24/09/2012 Add support for convex production cost (see CFLsg73.h CFLint ndat and CFLrealm prodcost for an explanation) (by TRLC)
AUTHOR    : Simon Goertz 

SUBJECT   : Read discrete location problems from structured files
            Allocate memeory for data strucures

=========================================================================*/

#ifndef CFLrdlp_h
#define CFLrdlp_h 1

#ifdef __cplusplus 
extern "C" {
#endif

int FindEntry ( FILE* , char* );

int CountEntries( char*, char* );

int ReadDLPdepots(char* , char* , int , int *, int *, double *, int *);

int ReadDLPcustomers(char*, char* , int , int * );

int ReadDLPcost(char* , char* , int , int , double**  ); 
 
int ReadDLPprodcost(char* , char* , int , int , double**  );

int ReadDLocProb( char*, int*, int*, int*, double*, int**, int**, 
                  int**, double**, int**, double**, int**, double**, 
                  double***, double***, int*, double***);


#ifdef __cplusplus 
}
#endif

#endif /* END OF FILE  CFLrdlp.h */
