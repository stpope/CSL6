/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFdRdU1 (AFILE *AFp, double Dbuff[], int Nreq)

Purpose:
  Read offset-binary 8-bit integer data from an audio file (return double)

Description:
  This routine reads a specified number of offset-binary 8-bit integer samples
  from an audio file.  The data in the file is converted to double format on
  output.

Parameters:
  <-  int AFdRdU1
      Number of data values transferred from the file.  On reaching the end of
      the file, this value may be less than Nreq.
   -> AFILE *AFp
      Audio file pointer for an audio file opened by AFopnRead
  <-  double Dbuff[]
      Array of doubles to receive the samples
   -> int Nreq
      Number of samples requested.  Nreq may be zero.

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

#define FREAD(buf,size,nv,fp)	(int) fread ((char *) buf, (size_t) size, \
					     (size_t) nv, fp)


int
AFdRdU1 (AFILE *AFp, double Dbuff[], int Nreq)

{
  int is, N, i, Nr;
  uint1_t Buf[NBBUF/LW];
  double g;

  for (is = 0; is < Nreq; ) {

    /* Read data from the audio file */
    N = MINV (NBBUF / LW, Nreq - is);
    Nr = FREAD (Buf, LW, N, AFp->fp);

    /* Convert to double */
    /* For offset-binary 8-bit data, the zero-point is the value 128 */
    g = AFp->ScaleF;
    for (i = 0; i < Nr; ++i) {
      Dbuff[is] = g * (((int) Buf[i]) - UINT1_OFFSET);
      ++is;
    }

    if (Nr < N)
      break;
  }

  return is;
}
