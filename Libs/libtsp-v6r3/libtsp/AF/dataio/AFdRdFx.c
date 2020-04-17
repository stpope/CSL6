/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFdRdF4 (AFILE *AFp, double Dbuff[], int Nreq)
  int AFdRdF8 (AFILE *AFp, double Dbuff[], int Nreq)

Purpose:
  Read 32-bit float data from an audio file (return double values)
  Read 64-bit float data from an audio file (return double values)

Description:
  This routine reads a specified number of float samples from an audio file.
  The data in the file is converted to double format on output.

Parameters:
  <-  int AFdRdFx
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

#define LW4		FDL_FLOAT32
#define LW8		FDL_FLOAT64
#define MINV(a, b)	(((a) < (b)) ? (a) : (b))
#define NBBUF		8192

#define FREAD(buf,size,nv,fp)	(int) fread ((char *) buf, (size_t) size, \
					     (size_t) nv, fp)


int
AFdRdF4 (AFILE *AFp, double Dbuff[], int Nreq)

{
  int is, N, i, Nr;
  float4_t Buf[NBBUF/LW4];
  double g;
  unsigned char *cp;
  unsigned char t;

  for (is = 0; is < Nreq; ) {

    /* Read data from the audio file */
    N = MINV (NBBUF / LW4, Nreq - is);
    Nr = FREAD (Buf, LW4, N, AFp->fp);

    /* Byte swap and convert to float */
    g = AFp->ScaleF;
    for (i = 0; i < Nr; ++i) {
      if (AFp->Swapb == DS_SWAP) {
	cp = (unsigned char *) &Buf[i];
	t = cp[3]; cp[3] = cp[0]; cp[0] = t;
	t = cp[2]; cp[2] = cp[1]; cp[1] = t;
      }
      Dbuff[is] = g * Buf[i];
      ++is;
    }

    if (Nr < N)
      break;
  }

  return is;
}

int
AFdRdF8 (AFILE *AFp, double Dbuff[], int Nreq)

{
  int is, N, i, Nr;
  double8_t Buf[NBBUF/LW8];
  double g;
  unsigned char *cp;
  unsigned char t;

  for (is = 0; is < Nreq; ) {

    /* Read data from the audio file */
    N = MINV (NBBUF / LW8, Nreq - is);
    Nr = FREAD (Buf, LW8, N, AFp->fp);

    /* Byte swap and convert to float */
    g = AFp->ScaleF;
    for (i = 0; i < Nr; ++i) {
      if (AFp->Swapb == DS_SWAP) {
	cp = (unsigned char *) &Buf[i];
	t = cp[7]; cp[7] = cp[0]; cp[0] = t;
	t = cp[6]; cp[6] = cp[1]; cp[1] = t;
	t = cp[5]; cp[5] = cp[2]; cp[2] = t;
	t = cp[4]; cp[4] = cp[3]; cp[3] = t;
      }
      Dbuff[is] = g * Buf[i];
      ++is;
    }
    if (Nr < N)
      break;
  }

  return is;
}
