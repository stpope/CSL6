/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  AFdataio.h

Description:
  Function prototypes for the TSP library AF data I/O internal routines

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.9 $  $Date: 2003/05/09 00:02:43 $

----------------------------------------------------------------------*/

#ifndef AFdataio_h_
#define AFdataio_h_

#include <stdio.h>	/* typedef for FILE */

#ifndef	AFILE_t_
#  define AFILE_t_
typedef struct AF_filepar AFILE;	/* Audio file parameters */
#endif

#include <limits.h>
#define AF_SEEK_END	LONG_MIN	/* Used as a flag to AFseek */

#ifdef __cplusplus
extern "C" {
#endif

int
AFdRdAlaw (AFILE *AFp, double Dbuff[], int Nreq);
int
AFdRdF4 (AFILE *AFp, double Dbuff[], int Nreq);
int
AFdRdF8 (AFILE *AFp, double Dbuff[], int Nreq);
int
AFdRdI1 (AFILE *AFp, double Dbuff[], int Nreq);
int
AFdRdI2 (AFILE *AFp, double Dbuff[], int Nreq);
int
AFdRdI3 (AFILE *AFp, double Dbuff[], int Nreq);
int
AFdRdI4 (AFILE *AFp, double Dbuff[], int Nreq);
int
AFdRdMulaw (AFILE *AFp, double Dbuff[], int Nreq);
int
AFdRdTA (AFILE *AFp, double Dbuff[], int Nreq);
int
AFdRdU1 (AFILE *AFp, double Dbuff[], int Nreq);
int
AFdWrAlaw (AFILE *AFp, const double Dbuff[], int Nval);
int
AFdWrF4 (AFILE *AFp, const double Dbuff[], int Nval);
int
AFdWrF8 (AFILE *AFp, const double Dbuff[], int Nval);
int
AFdWrI1 (AFILE *AFp, const double Dbuff[], int Nval);
int
AFdWrI2 (AFILE *AFp, const double Dbuff[], int Nval);
int
AFdWrI3 (AFILE *AFp, const double Dbuff[], int Nval);
int
AFdWrI4 (AFILE *AFp, const double Dbuff[], int Nval);
int
AFdWrMulaw (AFILE *AFp, const double Dbuff[], int Nval);
int
AFdWrTA (AFILE *AFp, const double Dbuff[], int Nval);
int
AFdWrU1 (AFILE *AFp, const double Dbuff[], int Nval);
int
AFfRdAlaw (AFILE *AFp, float Dbuff[], int Nreq);
int
AFfRdF4 (AFILE *AFp, float Dbuff[], int Nreq);
int
AFfRdF8 (AFILE *AFp, float Dbuff[], int Nreq);
int
AFfRdI1 (AFILE *AFp, float Dbuff[], int Nreq);
int
AFfRdI2 (AFILE *AFp, float Dbuff[], int Nreq);
int
AFfRdI3 (AFILE *AFp, float Dbuff[], int Nreq);
int
AFfRdI4 (AFILE *AFp, float Dbuff[], int Nreq);
int
AFfRdMulaw (AFILE *AFp, float Cbuff[], int Nreq);
int
AFfRdTA (AFILE *AFp, float Dbuff[], int Nreq);
int
AFfRdU1 (AFILE *AFp, float Dbuff[], int Nreq);
int
AFfWrAlaw (AFILE *AFp, const float Dbuff[], int Nval);
int
AFfWrF4 (AFILE *AFp, const float Dbuff[], int Nval);
int
AFfWrF8 (AFILE *AFp, const float Dbuff[], int Nval);
int
AFfWrI1 (AFILE *AFp, const float Dbuff[], int Nval);
int
AFfWrI2 (AFILE *AFp, const float Dbuff[], int Nval);
int
AFfWrI3 (AFILE *AFp, const float Dbuff[], int Nval);
int
AFfWrI4 (AFILE *AFp, const float Dbuff[], int Nval);
int
AFfWrMulaw (AFILE *AFp, const float Dbuff[], int Nval);
int
AFfWrTA (AFILE *AFp, const float Dbuff[], int Nval);
int
AFfWrU1 (AFILE *AFp, const float Dbuff[], int Nval);
char *
AFgetLine (FILE *fp, int *ErrCode);
int
AFposition (AFILE *AFp, long int offs);
int
AFrdAlaw (AFILE *AFp, float Fbuff[], int Nreq);
int
AFseek (FILE *fp, long int pos, int *ErrCode);
long int
AFtell (FILE *fp, int *ErrCode);

#ifdef __cplusplus
}
#endif

#endif /* AFdataio_h_ */
