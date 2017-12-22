/*=========================================================================

FILE      : CFLrdlp.c

VERSION   : 1.11
DATE      : 07/09/2007 
            06/08/2008 (Adjusted so that also instances of the multi-type
                        facility location problem can be read )
            25/01/2011 (optional lower limit on throughput of facilities introduced)
AUTHOR    : Simon Goertz (Andreas Klose)

SUBJECT   : Read discrete location problems from structured files
            Allocate memeory for data strucures

=========================================================================*/
#ifdef CFL_ORLIB
#define CFL_KLOSE 1
#endif

 #ifdef __cplusplus 
 extern "C" {
 #endif 

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "CFLrdlp.h"


/* If costmatrix is written in format where rows correspond to depots enable next line */
// #define CFL_MATRIX_ROW_DEPOT_FORMAT

int FindEntry ( FILE* DatFile, char* EntryName ) {
/* Look in DatFile where data entitled EntryName start *
   Return 1 if found, and return 0 if not found
*/

  char aline[500];
  char ok, found=0;
  char achr;

  do {
    ok = ( fscanf( DatFile, "%s", aline ) == 1 );
    if ( ok ) {
      found = ( strcasecmp( aline, EntryName ) == 0 );
      /* Read until next line */
      do {
        achr = (char) fgetc( DatFile );
      } while ( ( (int)achr >= 0 ) && ( achr != '\n' ) );
    }
  } while ( (!(found)) && (ok) );

  return( found );

}

int CountEntries( char* DatFileName, char* EntryName ) {
/* Count number of data entries in a data block entitled
   Entryname */

  FILE *DatFile;
  int  num=0, len=10000, numread;
  char achr, ok=1;
  char *buffer=NULL;

  if ( DatFileName == NULL ) return( 0 );
  DatFile = fopen( DatFileName, "r" );
  if ( DatFile == NULL ) return( 0 );

  if ( FindEntry( DatFile, EntryName ) == 0 ) {
    fclose( DatFile );
    return( 0 );
  }

  /* Read the comment line */
  do {
    achr = (char) fgetc( DatFile );
    ok   = ( (int)achr >= 0 );
  } while ( (ok) && ( achr != '\n' ) );

  if ( ok ) buffer = ( char* ) malloc(len);

  /* Count number of entries */
  while ( ok ) {
     numread = getline(&buffer,&len,DatFile);
     ok = ( (numread > 0) && (buffer[0] != '[') );
    if (ok) num++;
  }

  num--; // substract empty line before next tag [...]

  if ( buffer ) free( buffer );
  fclose( DatFile );
  return( num );

}

double ReadRealEntry( char* DatFileName, char* EntryName ) {
/* Read a single real number of section Entryname */

  FILE   *DatFile;
  double rnum=0.0;
  char   achr, ok=1;

  if ( DatFileName == NULL ) return( rnum );
  DatFile = fopen( DatFileName, "r" );
  if ( DatFile == NULL ) return( rnum);

  if ( FindEntry( DatFile, EntryName ) == 0 ) {
    fclose( DatFile );
    return( rnum );
  }

  /* Read the comment line */
  do {
    achr = (char) fgetc( DatFile );
    ok   = ( (int)achr >= 0 );
  } while ( (ok) && ( achr != '\n' ) );
  if ( ok ) if ( fscanf( DatFile, "%lf", &rnum ) != 1 ) rnum=0.0;
  fclose( DatFile );
  return( rnum );

}

int ReadNDATEntry( char* DatFileName, char* EntryName ) {
/* Read a single real number of section Entryname */
FILE *DatFile;
 
  if ( DatFileName == NULL ) return( 0 );
  DatFile = fopen( DatFileName, "r" );
  if ( DatFile == NULL ) return( 0 );

  if ( FindEntry( DatFile, EntryName ) == 0 ) {
    fclose( DatFile );
    return( 0 );
  }

  char ndatstring [80];
  int nocol;
  
  int check=0;

  check = fscanf(DatFile, "%s", ndatstring);
  if(check != 1) exit(1);
  check =0;
  check = fscanf(DatFile, "%d", &nocol);
  if(check != 1) exit(1);

  //printf("i read %s and %d\n", ndatstring, nocol);
  
  fclose( DatFile );
  
  return nocol;
  
}


int ReadDLPdepots(char* DatFileName, char* EntryName, int n, 
                  int* mincap, int *maxcap, double *fixcost,
                  int *depclass ) {

  FILE *DatFile;
  int  len=1000;
  int  line=0;
  char achr, ok=1;
  char *buffer=NULL;
  double zahl;

  if ( DatFileName == NULL ) return( 0 );
  DatFile = fopen( DatFileName, "r" );
  if ( DatFile == NULL ) return( 0 );

  if ( FindEntry( DatFile, EntryName ) == 0 ) {
    fclose( DatFile );
    return( 0 );
  }

  /* Read the comment line */
  do {
    achr = (char) fgetc( DatFile );
    ok   = ( (int)achr >= 0 );
  } while ( (ok) && ( achr != '\n' ) );

  if ( ok ) buffer = ( char* ) malloc(len);

  ok = ( buffer != NULL );

  /* read line per line */
  for ( line = 0; (ok) && (line < n); line++ ) {
    buffer = fgets(buffer,len,DatFile);
    if ( buffer != NULL) {
        if ( depclass == NULL ) {
               if ( mincap == NULL )
	         ok = (sscanf(buffer,"%d %lf %*s",maxcap+line,&zahl) > 0);
               else
                 ok = (sscanf(buffer,"%d %d %lf %*s",mincap+line,maxcap+line,&zahl) > 0);
        }
        else {
               if ( mincap == NULL )
                 ok = (sscanf(buffer,"%d %lf %d %*s",maxcap+line,&zahl,depclass+line) > 0);
               else
                 ok = (sscanf(buffer,"%d %d %lf %d %*s",mincap+line,maxcap+line,&zahl,depclass+line) > 0);
        }
	fixcost[line] = zahl;
    } else ok = 0;
  }

  if ( buffer ) free( buffer );
  fclose( DatFile );
  return( (line==n) && ok );
}


int ReadDLPcustomers(char* DatFileName, char* EntryName,
                     int m, int *demand ) {

  FILE *DatFile;
  int  len=1000;
  int  line=0;
  char achr, ok=1;
  char *buffer=NULL;

  if ( DatFileName == NULL ) return( 0 );
  DatFile = fopen( DatFileName, "r" );
  if ( DatFile == NULL ) return( 0 );

  if ( FindEntry( DatFile, EntryName ) == 0 ) {
    fclose( DatFile );
    return( 0 );
  }

  /* Read the comment line */
  do {
    achr = (char) fgetc( DatFile );
    ok   = ( (int)achr >= 0 );
  } while ( (ok) && ( achr != '\n' ) );

  if ( ok ) buffer = ( char* ) malloc(len);

  ok = ( buffer != NULL );

  /* read line per line */
  for ( line = 0; (ok) && (line < m); line++ ) {
    buffer = fgets(buffer,len,DatFile);
    if ( buffer != NULL) {
	ok = (sscanf(buffer,"%d %*s",&demand[line]) > 0);
    } else ok = 0;
  }

  if ( buffer ) free( buffer );
  fclose( DatFile );
  return( (line==m) && ok );
}

#ifndef CFL_ORLIB
int ReadDLPcost(char* DatFileName, char* EntryName,
                int n, int m, double** cost ) {

  FILE *DatFile;
  int  line=0;
  char achr, ok=1;
  char *buffer=NULL;
  int  col=0;
  double zahl=0.0;
  int c;

  if ( DatFileName == NULL ) return( 0 );
  DatFile = fopen( DatFileName, "r" );
  if ( DatFile == NULL ) return( 0 );

  if ( FindEntry( DatFile, EntryName ) == 0 ) {
    fclose( DatFile );
    return( 0 );
  }

  /* Read the comment line */
  do {
    achr = (char) fgetc( DatFile );
    ok   = ( (int)achr >= 0 );
  } while ( (ok) && ( achr != '\n' ) );


/* LAENGE ANPASSSEN AN GROESSEE DER KOSTEN */
  if ( ok ) buffer = ( char* ) malloc( 14 );
  ok = ( buffer != NULL );

#ifndef CFL_MATRIX_ROW_DEPOT_FORMAT
  /* read line per line: lines correspond to customer / cols correspond to depots */
  for ( line = 0; ok && ( line < m ) ; line++ ) {
	for ( col = 0; (ok) && ( col < n ) ; col++) {
		c = 0;
		do {
			achr = (char) fgetc( DatFile );
			buffer[c] = achr;
			c++;
//		} while ((c < 11) && ( achr != ' ' ));
		} while ((c < 14) && ( achr != ' ' ));

		ok = (sscanf(buffer,"%lf",&zahl) > 0);

		cost[col][line] = zahl;
	}
  }
#else  
  /* read line per line: lines correspond to depots / cols correspond to customer */
  for ( line = 0; ok && ( line < n ) ; line++ ) {
	for ( col = 0; (ok) && ( col < m ) ; col++) {
		c = 0;
		do {
			achr = (char) fgetc( DatFile );
			buffer[c] = achr;
			c++;
//		} while ((c < 11) && ( achr != ' ' ));
		} while ((c < 14) && ( achr != ' ' ));

		ok = (sscanf(buffer,"%lf",&zahl) > 0);

		cost[line][col] = zahl;
	}
  }
#endif

  if ( buffer ) free( buffer );
  fclose( DatFile );
#ifndef CFL_MATRIX_ROW_DEPOT_FORMAT
  return( (col==n) && (line==m) && ok );
#else
  return( (col==m) && (line==n) && ok );  
#endif  
}
#else
int ReadDLPcost(char* DatFileName, char* EntryName,
                int n, int m, double** cost ) {

  FILE *DatFile;
  int  line=0;
  char achr, ok=1;
  char *buffer=NULL;
  char *buffer2;
  char buffer3[50];
  int  col=0;
  double zahl=0.0;
  int len = 20000 ;

  char *endp;


  int i;

  if ( DatFileName == NULL ) return( 0 );
  DatFile = fopen( DatFileName, "r" );
  if ( DatFile == NULL ) return( 0 );

  if ( FindEntry( DatFile, EntryName ) == 0 ) {
    fclose( DatFile );
    return( 0 );
  }

  /* Read the comment line */
  do {
    achr = (char) fgetc( DatFile );
    ok   = ( (int)achr >= 0 );
  } while ( (ok) && ( achr != '\n' ) );

  buffer = ( char* ) malloc(len*sizeof(char));
  buffer2=buffer;

  while ( fgets( buffer ,len,DatFile) ) {
        endp = buffer;
	for ( endp=buffer; *endp != '\n'; ) {
		/* Skip blanks */
		for( ; *endp == ' '; endp++);
                if (  *endp == '\n' ) break;
		/* Copy digits to buffer3 */
		i = 0;
		for ( i=0 ; (i<50) && (*endp != '\n'); ) {
			buffer3[i++] = *endp;
			if ( *++endp == ' ' ) break;
		};

		sscanf(buffer3,"%lf",&zahl);

//printf("Gelesene Zahl %12.2f \n",zahl);
		if ( m != n ) {
			cost[col][line] = (double)zahl;
		} else {
			cost[line][col] = (double)zahl;
		}
	if (++col==n) { if(++line==m) break ; col=0;}

	}
	if (line==m) break;
  }

  if ( buffer2 ) free( buffer2 );
  fclose( DatFile );
  return( (col==n) && (line==m) && ok );
}
#endif
  int ReadDLPprodcost(char* DatFileName, char* EntryName,
                int NumDepots, int NumDat, double** prodcostmatrix ) {

 FILE *DatFile;
  int  len=1000;
  int  line=0;
  char achr, ok=1;
  char *buffer=NULL;

  if ( DatFileName == NULL ) return( 0 );
  DatFile = fopen( DatFileName, "r" );
  if ( DatFile == NULL ) return( 0 );

  if ( FindEntry( DatFile, EntryName ) == 0 ) {
    fclose( DatFile );
    return( 0 );
  }

  /* Read the comment line */
  do {
    achr = (char) fgetc( DatFile );
    ok   = ( (int)achr >= 0 );
  } while ( (ok) && ( achr != '\n' ) );

  if ( ok ) buffer = ( char* ) malloc(len);

  ok = ( buffer != NULL );

  /* read line per line */
  for ( line = 0; (ok) && (line < NumDepots); line++ ) {
    buffer = fgets(buffer,len,DatFile);
    if ( buffer != NULL) {
      if(NumDat == 1)
	{
	  ok = (sscanf(buffer,"%lf", &prodcostmatrix[line][0])>0);
	  //	  printf("Set prodcostmatrix[%d][0] = %f  \n", line, prodcostmatrix[line][0]);
	}
      else if(NumDat ==2)
	{
	  ok = (sscanf(buffer,"%lf %lf", &prodcostmatrix[line][0], &prodcostmatrix[line][1])>0);
	  //  printf("Set prodcostmatrix[%d][0] = %f, and prodcostmatrix[%d][1] = %f \n", line, prodcostmatrix[line][0], line, prodcostmatrix[line][1]);
	}
        else if(NumDat ==3)
	{
	  ok = (sscanf(buffer,"%lf %lf %lf", &prodcostmatrix[line][0], &prodcostmatrix[line][1], &prodcostmatrix[line][2])>0);
	  //	  	  printf("Set prodcostmatrix[%d][0] = %f, and prodcostmatrix[%d][1] = %f, and and prodcostmatrix[%d][2] = %f \n", line, prodcostmatrix[line][0],line, prodcostmatrix[line][1],line, prodcostmatrix[line][2]);
	}
	else
	{
	  printf("The NumDat was not 0,1,2 or 3 \n"); 
	  exit(0);
	}
    } else ok = 0;
  }

  if ( buffer ) free( buffer );
  fclose( DatFile );
  return( (line==NumDepots) && ok ); 
}


int ReadDLocProb( char* DatFileName, int* p, int* n, int* m, double* waitcst,
                  int** pltcap, int**mincap, int** maxcap, double** fixcost, 
                  int** demand, double** depcost, int** depclass, double** inbound, 
		  double*** C1, double*** C2, int* ndat, double*** prodcost) {
/* Read in the data of a Discrete Location Problem from file DatFileName */

  int     NumPlants=0, NumDepots=0, NumCusts=0, NumDats=0, MemOk=1;
  double *fc=NULL, *dc=NULL, *inbnd=NULL, **costmat1=NULL, **costmat2=NULL, **prodcostmatrix=NULL;
  int    *pcap=NULL, *capmin=NULL,*capmax=NULL, *dcust=NULL, *dclass=NULL;
  int    i;

#ifdef CFL_AVELLA
  int    j;
  FILE *DatFile;
  float  zahl;

  DatFile = NULL;

  /* READ FILE WITH INSTANCE FORMAT AVELLA */
  if ( DatFileName == NULL ) return( 0 );
  DatFile = fopen( DatFileName, "r" );
  if ( DatFile == NULL ) return( 0 );
  fscanf(DatFile, "%d", &NumCusts);
  fscanf(DatFile, "%d", &NumDepots);
  if ( waitcst ) fscanf(DatFile, "%lf", waitcst );
#endif

#ifdef CFL_KLOSE
  /* READ FILE WITH INSTANCE FORMAT KLOSE */
  /* Obtain number of plants, depots and customers in data file */
  if ( p != NULL ) NumPlants = CountEntries( DatFileName, "[PLANTS]" );
  NumDepots = CountEntries( DatFileName, "[DEPOTS]" );
  NumCusts  = CountEntries( DatFileName, "[CUSTOMERS]");
  if( prodcost != NULL) NumDats = ReadNDATEntry(DatFileName, "[NDAT]");     
  // if( prodcost != NULL) NumDats = ReadRealEntry(DatFileName, "[NDAT]");
#endif

#ifdef CFL_INFORM
  printf("NumDepots %d\n",NumDepots);
  printf("NumCusts %d\n",NumCusts);
  if( prodcost != NULL) printf("NumDats %d\n", NumDats);
#endif

  /* Allocate memory for depot data and read depot data from file */
  if ( NumDepots > 0 ) {
    fc = (double*) calloc( NumDepots, sizeof( double ) );
    capmax = (int*) calloc( NumDepots, sizeof( int ) );
    dcust = (int*) calloc( NumCusts, sizeof( int ) );
    costmat2 = (double **) calloc(NumDepots, sizeof( double* ) );
    prodcostmatrix = (double **) calloc(NumDepots, sizeof( double* ) );
    MemOk = ( (fc) && (capmax) && (dcust) && (costmat2) && (prodcostmatrix) );
    if ( (MemOk) && (depcost) ) { 
      dc = (double*) calloc( NumDepots, sizeof(double) );
      MemOk = ( dc != NULL );
    }  
    if ( (MemOk) && (inbound) ) { 
      inbnd = (double*) calloc( NumDepots, sizeof(double) );
      MemOk = ( inbnd != NULL );
    }  
    if ( (MemOk) && (depclass) ) { 
      dclass = (int*) calloc( NumDepots, sizeof(int) );
      MemOk = ( dclass != NULL );
    }  
    if ( (MemOk) && (mincap) ) {
      capmin = (int*) calloc( NumDepots, sizeof(int) );
      MemOk = ( capmin != NULL );
    }
    for ( i=0; (MemOk) && (i<NumDepots); i++) {
        costmat2[i] = (double *) calloc( NumCusts, sizeof( double ));
	MemOk = (costmat2[i] != NULL);
    }
    //now allocate memory for the parameters of the production cost function
    if(prodcost !=NULL){
      for(i=0; (MemOk) && (i<NumDepots); i++){
        prodcostmatrix[i] = (double *) calloc( NumDats, sizeof( double ));
        MemOk = (prodcostmatrix[i] != NULL);
      }
    }
  }

#ifdef CFL_AVELLA
  /* READ FILE WITH INSTANCE FORMAT AVELLA */
  for (i= 0; i< NumCusts; i++)
    fscanf(DatFile,"%d",&dcust[i]);
  for (j= 0; j< NumDepots; j++)
    fscanf(DatFile,"%d",&capmax[j]);
  if ( depclass != NULL ) for (j= 0; j< NumDepots; j++)
    fscanf(DatFile,"%d",&dclass[j]); 
  for (j= 0; j< NumDepots; j++) {
    fscanf(DatFile,"%f",&zahl);
    fc[j] = (double) zahl;
  }
  for(j=0; j<NumDepots;j++) {
    for(i=0;i<NumCusts;i++) {
      fscanf(DatFile,"%f",&zahl);
      costmat2[j][i]= ((double)zahl)*dcust[i];
    }
  }
  if ( prodcost != NULL ) // Avella Data. Use with quadratic prod. cost
  {
    NumDats = 2;
    for ( i=0; i < NumDepots; i++ )
    {
      prodcostmatrix[i][0] = 0.75;//0.0;
      prodcostmatrix[i][1] = 0.75;//0.77595;
    }    
  }
#endif

#ifdef CFL_KLOSE
  /* READ FILE WITH INSTANCE FORMAT KLOSE */
  if (MemOk) MemOk = ReadDLPdepots(DatFileName, "[DEPOTS]", NumDepots, capmin, capmax, fc, dclass  );
  if (MemOk) MemOk = ReadDLPcustomers(DatFileName, "[CUSTOMERS]", NumCusts, dcust );
  if (MemOk) MemOk = ReadDLPcost(DatFileName, "[MATRIX]", NumDepots, NumCusts, costmat2 );
  if ( (MemOk) && ( waitcst ) ) *waitcst = ReadRealEntry( DatFileName, "[WAITCOST]" );
  if ( (MemOk) && ( prodcost ) ) ReadDLPprodcost(DatFileName, "[NDAT]", NumDepots, NumDats, prodcostmatrix ); 
#endif

  if ( ! (MemOk) ) {
    if ( pcap ) free( pcap );
    if ( capmin ) free( capmin );
    if ( capmax ) free( capmax );
    if ( dclass ) free( dclass );
    if ( fc ) free( fc );
    if ( dc ) free (dc );
    if ( dcust ) free (dcust );
    if ( inbnd ) free( inbnd );
    if ( costmat1 )
      for ( i=0; i < NumPlants; i++ ) if( costmat1[i] ) free( costmat1[i] );
    if ( costmat2 )
      for ( i=0; i < NumDepots; i++ ) if ( costmat2[i] ) free( costmat2[i] );
    if (prodcostmatrix )
      for(i=0; i < NumDepots; i++) if( prodcostmatrix[i] ) free( prodcostmatrix[i] );
    NumDepots = 0, NumPlants = 0, NumCusts  = 0, NumDats=0;
    fc = dc = inbnd = NULL;
    pcap = capmin = capmax = NULL;
    costmat1 = costmat2 = prodcostmatrix = NULL;
  }

#ifdef CFL_AVELLA
  fclose( DatFile );
#endif

  if ( pltcap   != NULL ) *pltcap  = pcap;
  if ( maxcap   != NULL ) *maxcap  = capmax;
  if ( mincap   != NULL ) *mincap  = capmin;
  if ( fixcost  != NULL ) *fixcost = fc;
  if ( demand   != NULL ) *demand  = dcust;
  if ( depcost  != NULL ) *depcost = dc;
  if ( depclass != NULL ) *depclass= dclass;
  if ( inbound  != NULL ) *inbound = inbnd;
  if ( C1       != NULL ) *C1      = costmat1;
  if ( C2       != NULL ) *C2      = costmat2;
  if ( p != NULL ) *p = NumPlants;
  if ( n != NULL ) *n = NumDepots;
  if ( m != NULL ) *m = NumCusts;
  if ( ndat != NULL ) *ndat = NumDats;
  if ( prodcost != NULL ) *prodcost = prodcostmatrix;

  return (MemOk == 0);
}

#ifdef __cplusplus 
}
#endif

/* END OF FILE  CFLrdlp.c */
