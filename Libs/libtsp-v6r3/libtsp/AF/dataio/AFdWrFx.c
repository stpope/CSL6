/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFdWrF4 (AFILE *AFp, const double Dbuff[], int Nval)
  int AFdWrF8 (AFILE *AFp, const double Dbuff[], int Nval)

Purpose:
  Write 32-bit float data to an audio file (double input values)
  Write 64-bit float data to an audio file (double input values)

Description:
  This routine writes a specified number of float samples to an audio file.
  The input to this routine is a buffer of double values.

Parameters:
  <-  int AFdWrFx
      Number of samples written.  If this value is less than Nval, an error
      has occurred.
   -> AFILE *AFp
      Audio file pointer for an audio file opened by AFopnWrite
   -> const double Dbuff[]
      Array of doubles with the samples to be written
   -> int Nval
      Number of samples to be written

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.2 $  $Date: 2003/05/09 01:11:34 $

-------------------------------------------------------------------------*/

#include <libtsp/AFdataio.h>
#include <libtsp/AFpar.h>
#include <libtsp/UTtypes.h>

#define LW4		FDL_FLOAT32
#define LW8		FDL_FLOAT64
#define MINV(a, b)	(((a) < (b)) ? (a) : (b))
#define NBBUF		8192

#define FWRITE(buf,size,nv,fp)	(int) fwrite ((const char *) buf, \
					      (size_t) size, (size_t) nv, fp)


int
AFdWrF4 (AFILE *AFp, const double Dbuff[], int Nval)

{
  int is, N, Nw, i;
  float4_t Buf[NBBUF/LW4];
  unsigned char *cp;
  unsigned char t;
  double g;

/* Write data to the audio file */
  is = 0;
  g = AFp->ScaleF;
  while (is < Nval) {
    N = MINV (NBBUF / LW4, Nval - is);
    for (i = 0; i < N; ++i) {
      Buf[i] = (float4_t) (g * Dbuff[i+is]);
      if (AFp->Swapb == DS_SWAP) {
	cp = (unsigned char *) &Buf[i];
	t = cp[3]; cp[3] = cp[0]; cp[0] = t;
	t = cp[2]; cp[2] = cp[1]; cp[1] = t;
      }
    }
    Nw = FWRITE (Buf, LW4, N, AFp->fp);
    is += Nw;
    if (Nw < N)
      break;
  }

  return is;
}

int
AFdWrF8 (AFILE *AFp, const double Dbuff[], int Nval)

{
  int is, N, Nw, i;
  double8_t Buf[NBBUF/LW8];
  unsigned char *cp;
  unsigned char t;
  double g;

/* Write data to the audio file */
  is = 0;
  g = AFp->ScaleF;
  while (is < Nval) {
    N = MINV (NBBUF / LW8, Nval - is);
    for (i = 0; i < N; ++i) {
      Buf[i] = (double8_t) (g * Dbuff[i+is]);
      if (AFp->Swapb == DS_SWAP) {
	cp = (unsigned char *) &Buf[i];
	t = cp[7]; cp[7] = cp[0]; cp[0] = t;
	t = cp[6]; cp[6] = cp[1]; cp[1] = t;
	t = cp[5]; cp[5] = cp[2]; cp[2] = t;
	t = cp[4]; cp[4] = cp[3]; cp[3] = t;
      }
    }
    Nw = FWRITE (Buf, LW8, N, AFp->fp);
    is += Nw;
    if (Nw < N)
      break;
  }

  return is;
}
