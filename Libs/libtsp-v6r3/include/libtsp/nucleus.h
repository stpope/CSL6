/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  nucleus.h

Description:
  Function prototypes for the TSP library internal (nucleus) routines

Author / revision:
  P. Kabal  Copyright (C) 2005
  $Revision: 2.56 $  $Date: 2005/02/01 05:24:47 $

----------------------------------------------------------------------*/

#ifndef nucleus_h_
#define nucleus_h_

#include <stdio.h>	/* typedef for FILE */
#include <time.h>	/* typedef for time_t */

#ifdef __cplusplus
extern "C" {
#endif

/* ----- FI Prototypes ----- */
double
FIrCosF (double x, double alpha);
double
FIxKaiser (double x, double alpha);

/* ----- FL Prototypes ----- */
int
FLdReadTF (FILE *fp, int MaxNval, int cc, double x[]);
int
FLexist (const char Fname[]);
int
FLfReadTF (FILE *fp, int MaxNval, int cc, float x[]);
long int
FLfileSize (FILE *fp);
int
FLjoinNames (const char Dname[], const char Bname[], char Fname[]);
int
FLseekable (FILE *fp);
int
FLterm (FILE *fp);

/* ----- MA Prototypes ----- */
int
MAfChFactor (const float *A[], float *L[], int N);
void
MAfLTSolve (const float *L[], const float b[], float x[], int N);
void
MAfTTSolve (const float *L[], const float b[], float x[], int N);

/* ----- MS Prototypes ----- */
double *
MScoefMC (double x1, double x2, double y1, double y2, double d1, double d2);
double
MSdSlopeMC (int k, const double x[], const double y[], int N);
double
MSevalMC (double x, double x1, double x2, double y1, double y2, double d1,
	  double d2);
double
MSfSlopeMC (int k, const float x[], const float y[], int N);

/* ----- SP Prototypes ----- */
void
SPdPreFFT (double x[], double y[], int N, int Ifn);
void
SPdTrMat2 (double A[], int N, int Ifn);
void
SPfPreFFT (float x[], float y[], int N, int Ifn);
void
SPfTrMat2 (float A[], int N, int Ifn);

/* ----- ST Prototypes ----- */
int
STdec1val (const char String[], int Type, void *Val);
int
STdecNval (const char String[], int Nmin, int Nmax, int Type, void *Val,
	   int *N);
int
STdecPair (const char String[], const char Delim[], int Type, void *Val1,
	   void *Val2);
char *
STstrDots (const char Si[], int Maxchar);
char *
STstrstrNM (const char Si[], const char Ti[], int N, int M);
char *
STtrimIws (const char Si[]);
int
STtrimNMax (const char Si[], char So[], int N, int Maxchar);

/* ----- UT Prototypes ----- */
int
UTbyteCode (int Dbo);
int
UTbyteOrder (void);
int
UTcheckIEEE (void);
char *
UTctime (time_t *timer, int format);
double
UTdIEEE80 (const unsigned char b[10]);
int
UTdecOption (const char String[], const char Option[], const char **Arg);
void
UTeIEEE80 (double V, unsigned char b[10]);
char *
UTgetHost (void);
int
UTswapCode (int Dbo);
char *
UTgetUser (void);

/* ----- VR Prototypes ----- */
void
VRswapBytes (const void *BuffI, void *BuffO, int Size, int Nelem);

#ifdef __cplusplus
}
#endif

#endif /* nucleus_h_ */
