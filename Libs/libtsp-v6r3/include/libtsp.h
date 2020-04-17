/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  libtsp.h

Description:
  Function prototypes and declarations for the TSP library routines

Author / revision:
  P. Kabal  Copyright (C) 2005
  $Revision: 2.104 $  $Date: 2005/02/01 13:22:34 $

----------------------------------------------------------------------*/

#ifndef libtsp_h_
#define libtsp_h_

#include <stdio.h>	/* typedef for FILE */

/* Audio file parameter structure */
#ifndef	AFILE_t_
#  define AFILE_t_
typedef struct AF_filepar AFILE;	/* Audio file parameters */
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ----- AF Prototypes ----- */
void
AFclose (AFILE *AFp);
int
AFdReadData (AFILE *AFp, long int offs, double Dbuff[], int Nreq);
int
AFdWriteData (AFILE *AFp, const double Dbuff[], int Nval);
int
AFfReadData (AFILE *AFp, long int offs, float Dbuff[], int Nreq);
int
AFfWriteData (AFILE *AFp, const float Dbuff[], int Nval);
AFILE *
AFopnRead (const char Fname[], long int *Nsamp, long int *Nchan, double *Sfreq,
	   FILE *fpinfo);
AFILE *
AFopnWrite (const char Fname[], int Ftype, int Dformat, long int Nchan,
	    double Sfreq, FILE *fpinfo);
int
AFsetFileType (const char String[]);
void
AFsetInfo (const char Info[]);
int
AFsetNHpar (const char String[]);
int
AFsetSpeaker(const char String[]);

  /* Deprecated */
AFILE *
AFopenRead (const char Fname[], long int *Nsamp, long int *Nchan, float *Sfreq,
	    FILE *fpinfo);
AFILE *
AFopenWrite (const char Fname[], int Fformat, long int Nchan, double Sfreq,
	     FILE *fpinfo);
int
AFreadData (AFILE *AFp, long int offs, float Dbuff[], int Nreq);
int
AFwriteData (AFILE *AFp, const float Dbuff[], int Nval);


/* ----- FI Prototypes ----- */
void
FIdBiquad (const double x[], double y[], int Nout, const double h[5]);
void
FIdConvSI (const double x[], double y[], int Nout, const double h[], int Ncof,
	  int mr, int Nsub, int Ir);
void
FIdConvol (const double x[], double y[], int Nout, const double h[], int Ncof);
void
FIdFiltAP (const double x[], double y[], int Nout, const double p[], int Np);
void
FIdFiltIIR (const double x[], double y[], int Nout, const double h[][5],
	    int Nsec);
void
FIdKaiserLPF (double h[], int N, double Fc, double alpha);
int
FIdReadFilt (const char Fname[], int MaxNcof, double h[], int *Ncof,
	     FILE *fpinfo);
void
FIdWinHamm (double win[], int N, double a);
void
FIdWinKaiser (double win[], int N, double alpha);
void
FIfBiquad (const float x[], float y[], int Nout, const float h[5]);
void
FIfConvSI (const float x[], float y[], int Nout, const float h[], int Ncof,
	  int mr, int Nsub, int Ir);
void
FIfConvol (const float x[], float y[], int Nout, const float h[], int Ncof);
void
FIfDeem (double a, float *Fmem, const float x[], float y[], int Nout);
void
FIfFiltAP (const float x[], float y[], int Nout, const float p[], int Np);
void
FIfFiltIIR (const float x[], float y[], int Nout, const float h[][5],
	    int Nsec);
void
FIfKaiserLPF (float h[], int N, double Fc, double alpha);
void
FIfPreem (double a, float *Fmem, const float x[], float y[], int Nout);
int
FIfReadFilt (const char Fname[], int MaxNcof, float h[], int *Ncof,
	     FILE *fpinfo);
void
FIfWinHCos (float win[], double T1, double T2, double T3, double a);
void
FIfWinHamm (float win[], int N, double a);
void
FIfWinKaiser (float win[], int N, double alpha);
void
FIfWinRCos (float win[], int N, int Nadv);
double
FIgdelFIR (double w, const float h[], int Ncof);
double
FIgdelIIR (double w, const float h[][5], int Nsec);

/* ----- FL Prototypes ----- */
void
FLbackup (const char Fname[]);
int
FLbaseName (const char Fname[], char Bname[]);
int
FLdReadData (const char Fname[], int MaxNval, double x[]);
void
FLdWriteData (const char Fname[], const double x[], int N);
char *
FLdate (const char Fname[], int format);
int
FLdefName (const char Fi[], const char Fd[], char Fo[]);
int
FLdirName (const char Fname[], char Bname[]);
int
FLexpHome (const char Fname[], char Ename[]);
int
FLextName (const char Fname[], char Ext[]);
int
FLfReadData (const char Fname[], int MaxNval, float x[]);
void
FLfWriteData (const char Fname[], const float x[], int N);
char *
FLfileDate (FILE *fp, int format);
int
FLfullName (const char Fname[], char Fullname[]);
char *
FLgetLine (FILE *fp);
char *
FLgetRec (FILE *fp, const char CommChar[], const char ContChar[], int echo);
int
FLhomeDir (const char User[], char Home[]);
int
FLpathList (const char Fi[], const char Dlist[], char Fo[]);
int
FLpreName (const char Fname[], char Pname[]);
void
FLprompt (const char Prompt[]);
char *
FLreadLine (const char Prompt[]);

/* ----- FN Prototypes ----- */
double
FNbessI0 (double x);
double
FNevChebP (double x, const float c[], int N);
long int
FNgcd (long int u, long int v);
int
FNiLog2 (int a);
double
FNsinc (double x);

/* ----- MA Prototypes ----- */
double **
MAdAllocMat (int Nrow, int Ncol);
void
MAdFreeMat (double *A[]);
void
MAdPrint (FILE *fp, const char Header[], const double *A[], int Nrow,
	  int Ncol);
float **
MAfAllocMat (int Nrow, int Ncol);
int
MAfChSolve (const float *A[], const float b[], float x[], int N);
void
MAfFreeMat (float *A[]);
void
MAfPrint (FILE *fp, const char Header[], const float *A[], int Nrow, int Ncol);
float **
MAfSubMat (const float *A[], int n, int m, int Nrow);
double
MAfSyBilin (const float *A[], const float x[], const float y[], int N);
double
MAfSyQuad (const float *A[], const float x[], int N);
double
MAfTpQuad (const float R[], const float x[], int N);
int
MAfTpSolve (const float R[], const float g[], float c[], int N);
void
MAiPrint (FILE *fp, const char Header[], const int *A[], int Nrow,
	  int Ncol);
void
MAlPrint (FILE *fp, const char Header[], const long int *A[], int Nrow,
	  int Ncol);

/* ----- MS Prototypes ----- */
int
MSdConvCof (const double x[], int Nx, const double y[], int Ny, double z[]);
int
MSdDeconvCof (const double u[], int Nu, const double v[], int Nv,
	      double q[], double r[]);
void
MSdIntLin (const double x[], const double y[], int N, const double xi[],
	   double yi[], int Ni);
void
MSdIntMC (const double x[], const double y[], int N, const double xi[],
	  double yi[], int Ni);
int
MSdLocate (double x, const double T[], int N);
double
MSdNint (double x);
double
MSdPolyInt (double x, const double xa[], const double ya[], int N,
	    double buff[]);
int
MSfConvCof (const float x[], int Nx, const float y[], int Ny, float z[]);
double
MSfGaussRand (double rms);
void
MSfIntLin (const float x[], const float y[], int N, const float xi[],
	   float yi[], int Ni);
void
MSfIntMC (const float x[], const float y[], int N, const float xi[],
	  float yi[], int Ni);
double
MSfUnifRand (void);
int
MSiCeil (int n, int m);
int
MSiFloor (int n, int m);
int
MSiPower (int i, int n);
long int
MSlCeil (long int n, long int m);
long int
MSlFloor (long int n, long int m);
void
MSrandSeed (int seed);
void
MSratio (double Val, long int *N, long int *D, double tol, long int MaxN,
	 long int MaxD);

/* ----- SP Prototypes ----- */
void
SPautoc (const float x[], int Nx, float cor[], int Nt);
void
SPcFFT (float x[], float y[], int N, int Ifn);
void
SPcepXpc (const float cep[], float pc[], int Np);
void
SPcorBWexp (double bwexp, const float rxx[], float rxxb[], int Nc);
double
SPcorFilt (double Ed, const float rxx[], const float r[], float h[], int N);
double
SPcorFmse (const float h[], double Ed, const float rxx[], const float r[],
	   int N);
double
SPcorPmse (const float pc[], const float rxx[], int Np);
double
SPcorXpc (const float rxx[], float pc[], int Np);
double
SPcovCXpc (const float *Cov[], float pc[], int Np);
double
SPcovFilt (double Ed, const float *R[], const float r[], float h[], int N);
double
SPcovFmse (const float h[], double Ed, const float *R[], const float r[],
	   int N);
double
SPcovLXpc (const float *Cov[], float pc[], int Np);
double
SPcovMXpc (const float *Cov[], float pc[], int Np);
double
SPcovPmse (const float pc[], const float *Cov[], int Np);
double
SPcovXpc (const float *Cov[], float pc[], int Np);
void
SPcovar (const float x[], int Nx, float *Cov[], int Np);
void
SPdCFFT (double x[], double y[], int N, int Ifn);
int
SPdQuantL (double x, const double Xq[], int Nreg);
void
SPdRFFT (double x[], int N, int Ifn);
void
SPecXpc (const float ec[], float pc[], int Np);
void
SPfCFFT (float x[], float y[], int N, int Ifn);
void
SPfDCT (float x[], int N, int Ifn);
int
SPfQuantL (double x, const float Xq[], int Nreg);
int
SPfQuantU (double x, const float Xq[], int Nreg);
void
SPfRFFT (float x[], int N, int Ifn);
double
SPlpcISdist (const float pc1[], const float pc2[], int Np);
double
SPlpcLSdist (const float pc1[], const float pc2[], int Np, int Ncep);
void
SPlsfXpc (const float lsf[], float pc[], int Np);
void
SPpcBWexp (double bwexp, const float pc[], float pcb[], int Np);
void
SPpcXcep (const float pc[], int Np, float cep[], int Ncep);
double
SPpcXcor (const float pc[], int Np, float rxx[], int Ncor);
void
SPpcXec (const float pc[], float ec[], int Np);
void
SPpcXlsf (const float pc[], float lsf[], int Np);
double
SPpcXrc (const float pc[], float rc[], int Np);
void
SPfRFFT (float x[], int N, int Ifn);
double
SPrcXpc (const float rc[], float pc[], int Np);

/* ----- ST Prototypes ----- */
int
STcatMax (const char Si[], char So[], int Maxchar);
int
STcopyMax (const char Si[], char So[], int Maxchar);
int
STcopyNMax (const char Si[], char So[], int N, int Maxchar);
int
STdec1double (const char String[], double *Dval);
int
STdec1float (const char String[], float *Fval);
int
STdec1int (const char String[], int *Ival);
int
STdec1long (const char String[], long int *Lval);
int
STdecDfrac (const char String[], double *Dval1, double *Dval2);
int
STdecIfrac (const char String[], int *Ival1, int *Ival2);
int
STdecIrange (const char String[], int *Ival1, int *Ival2);
int
STdecLrange (const char String[], long int *Lval1, long int *Lval2);
int
STdecNdouble (const char String[], int Nmin, int Nmax, double Dval[], int *N);
int
STdecNfloat (const char String[], int Nmin, int Nmax, float Fval[], int *N);
int
STdecNint (const char String[], int Nmin, int Nmax, int Ival[], int *N);
int
STdecNlong (const char String[], int Nmin, int Nmax, long int Lval[],
	    int *N);
char *
STfindToken (const char String[], const char Delims[], const char Quotes[],
	     char Token[], int WSFlag, int Maxchar);
int
STkeyMatch (const char String[], const char *KeyTable[]);
int
STkeyXpar (const char Line[], const char Delims[], const char Quotes[],
	   const char *Keytable[], char Par[]);
int
STstrLC (const char Si[], char So[]);
int
STtrim (const char Si[], char So[]);
int
STunQuote (const char Si[], const char Quotes[], char So[]);

/* ----- UT Prototypes ----- */
char *
UTdate (int format);
void
UTerror (const char Errmsg[], ...);
void
UTfree (void *ptr);
int
UTgetOption (int *Index, const int argc, const char *argv[],
	     const char *OptTable[], const char **OptArg);
char *
UTgetProg (void);
void
UThalt (const char Errmsg[], ...);
void *
UTmalloc (int size);
void *
UTrealloc (void *ptr, int size);
void
UTsetProg (const char Program[]);
void
UTsysMsg (const char Warnmsg[], ...);
char *
UTuserName (void);
void
UTwarn (const char Warnmsg[], ...);

/* ----- VR Prototypes ----- */
void
VRdAdd (const double x1[], const double x2[], double y[], int N);
void
VRdCopy (const double x[], double y[], int N);
double
VRdCorSym (const double x[], int N);
double
VRdDotProd (const double x1[], const double x2[], int N);
void
VRdLinInc (double xmin, double xmax, double x[], int N);
void
VRdMult (const double x1[], const double x2[], double y[], int N);
void
VRdPrint (FILE *fp, const char Header[], const double x[], int N);
void
VRdRFFTMSq (const double X[], double X2[], int N);
void
VRdScale (double a, const double x[], double y[], int N);
void
VRdSet (double a, double y[], int N);
void
VRdShift (double x[], int Nkeep, int Nshift);
void
VRdSymPart (int Sym, const double x[], double y[], int N);
void
VRdZero (double y[], int N);
void
VRfAdd (const float x1[], const float x2[], float y[], int N);
void
VRfAmpldB (const float ampl[], float dB[], int N);
void
VRfCopy (const float x[], float y[], int N);
double
VRfCorSym (const float x[], int N);
void
VRfDiff (const float x1[], const float x2[], float y[], int N);
double
VRfDiffSq (const float x1[], const float x2[], int N);
double
VRfDotProd (const float x1[], const float x2[], int N);
void
VRfLog10 (const float x[], float y[], int N);
double
VRfMax (const float x[], int N);
double
VRfMin (const float x[], int N);
void
VRfMult (const float x1[], const float x2[], float y[], int N);
void
VRfPow10 (const float x[], float y[], int N);
void
VRfPrint (FILE *fp, const char Header[], const float x[], int N);
void
VRfRev (const float x[], float y[], int N);
void
VRfScale (double a, const float x[], float y[], int N);
void
VRfSet (double a, float y[], int N);
void
VRfShift (float x[], int Nkeep, int Nshift);
double
VRfSum (const float x[], int N);
void
VRfZero (float y[], int N);
void
VRiPrint (FILE *fp, const char Header[], const int x[], int N);
void
VRiZero (int y[], int N);
void
VRlPrint (FILE *fp, const char Header[], const long int x[], int N);

#ifdef __cplusplus
}
#endif

#endif /* libtsp_h_ */
