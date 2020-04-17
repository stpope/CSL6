/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFfWrU1 (AFILE *AFp, const float Dbuff[], int Nval)

Purpose:
  Write offset-binary 8-bit integer data to an audio file (float input values)

Description:
  This routine writes a specified number of offset-binary 8-bit integer samples
  to an audio file.  The input to this routine is a buffer of float values.

Parameters:
  <-  int AFfWrU1
      Number of samples written.  If this value is less than Nval, an error
      has occurred.
   -> AFILE *AFp
      Audio file pointer for an audio file opened by AFopnWrite
   -> const float Dbuff[]
      Array of floats with the samples to be written
   -> int Nval
      Number of samples to be written

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.2 $  $Date: 2003/05/09 01:11:34 $

-------------------------------------------------------------------------*/

#include <libtsp/AFdataio.h>
#include <libtsp/AFpar.h>
#include <libtsp/UTtypes.h>

#define LW		FDL_UINT8
#define MINV(a, b)	(((a) < (b)) ? (a) : (b))
#define NBBUF		8192

#define UINT1_OFFSET	((UINT1_MAX+1)/2)

#define FWRITE(buf,size,nv,fp)	(int) fwrite ((const char *) buf, \
					      (size_t) size, (size_t) nv, fp)


int
AFfWrU1 (AFILE *AFp, const float Dbuff[], int Nval)

{
  int is, N, Nw, i;
  uint1_t Buf[NBBUF/LW];
  double g, Dv;

/* Write data to the audio file */
  is = 0;
  g = (float) AFp->ScaleF;
  while (is < Nval) {
    N = MINV (NBBUF / LW, Nval - is);
    for (i = 0; i < N; ++i) {
      Dv = g * Dbuff[i+is] + UINT1_OFFSET + 0.5;
      if (Dv >= UINT1_MAX + 1) {
	++AFp->Novld;
	Dv = UINT1_MAX;
      }
      else if (Dv <= 0) {
	++AFp->Novld;
	Dv = 0;
      }
      Buf[i] = (uint1_t) Dv;
    }
    Nw = FWRITE (Buf, LW, N, AFp->fp);
    is += Nw;
    if (Nw < N)
      break;
  }

  return is;
}
