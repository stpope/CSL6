/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFdRdI1 (AFILE *AFp, double Dbuff[], int Nreq)
  int AFdRdI2 (AFILE *AFp, double Dbuff[], int Nreq)
  int AFdRdI3 (AFILE *AFp, double Dbuff[], int Nreq)
  int AFdRdI4 (AFILE *AFp, double Dbuff[], int Nreq)

Purpose:
  Read 8-bit integer data from an audio file (return double values)
  Read 16-bit integer data from an audio file (return double values)
  Read 24-bit integer data from an audio file (return double values)
  Read 32-bit integer data from an audio file (return double values)

Description:
  This routine reads a specified number of integer samples from an audio file.
  The data in the file is converted to double format on output.

Parameters:
  <-  int AFdRdIx
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

#include <libtsp/nucleus.h>
#include <libtsp/AFdataio.h>
#include <libtsp/AFpar.h>
#include <libtsp/UTtypes.h>

#define LW1		FDL_INT8
#define LW2		FDL_INT16
#define LW3		FDL_INT24
#define LW4		FDL_INT32
#define MINV(a, b)	(((a) < (b)) ? (a) : (b))
#define NBBUF		8192

#define FREAD(buf,size,nv,fp)	(int) fread ((char *) buf, (size_t) size, \
					     (size_t) nv, fp)


int
AFdRdI1 (AFILE *AFp, double Dbuff[], int Nreq)

{
  int is, N, i, Nr;
  int1_t Buf[NBBUF/LW1];
  double g;

  for (is = 0; is < Nreq; ) {

    /* Read data from the audio file */
    N = MINV (NBBUF / LW1, Nreq - is);
    Nr = FREAD (Buf, LW1, N, AFp->fp);

    /* Convert to double */
    g = AFp->ScaleF;
    for (i = 0; i < Nr; ++i) {
      Dbuff[is] = g * Buf[i];
      ++is;
    }
    if (Nr < N)
      break;
  }

  return is;
}

int
AFdRdI2 (AFILE *AFp, double Dbuff[], int Nreq)

{
  int is, N, i, Nr;
  int2_t Buf[NBBUF/LW2];
  unsigned char *cp;
  unsigned char t;
  double g;

  for (is = 0; is < Nreq; ) {

    /* Read data from the audio file */
    N = MINV (NBBUF / LW2, Nreq - is);
    Nr = FREAD (Buf, LW2, N, AFp->fp);

    /* Byte swap and convert to double */
    g = AFp->ScaleF;
    for (i = 0; i < Nr; ++i) {
      if (AFp->Swapb == DS_SWAP) {
	cp = (unsigned char *) &Buf[i];
	t = cp[1]; cp[1] = cp[0]; cp[0] = t;
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
AFdRdI3 (AFILE *AFp, double Dbuff[], int Nreq)

{
  int is, N, i, Nr, Hbo;
  int4_t Iv;
  unsigned char Buf[NBBUF];
  unsigned char *cp;
  unsigned char t;
  double g;

  Hbo = UTbyteOrder ();
  cp = (unsigned char *) &Iv;
  for (is = 0; is < Nreq; ) {

    /* Read data from the audio file */
    N = MINV (NBBUF/LW3, Nreq - is);
    Nr = FREAD (Buf, LW3, N, AFp->fp);

    /* Byte swap and convert to double */
    g = AFp->ScaleF;
    for (i = 0; i < LW3*Nr; i += LW3) {
      if (AFp->Swapb == DS_SWAP) {
	t = Buf[i+2]; Buf[i+2] = Buf[i]; Buf[i] = t;
      }
      if (Hbo == DS_EL) {
	cp[0] = 0;
	cp[1] = Buf[i];
	cp[2] = Buf[i+1];
	cp[3] = Buf[i+2];	/* Most significant byte */
      }
      else {
	cp[0] = Buf[i];		/* Most significant byte */
	cp[1] = Buf[i+1];
	cp[2] = Buf[i+2];
	cp[3] = 0;
      }
      Dbuff[is] = g * (Iv / 256);
      ++is;
    }

    if (Nr < N)
      break;
  }

  return is;
}

int
AFdRdI4 (AFILE *AFp, double Dbuff[], int Nreq)

{
  int is, N, i, Nr;
  int4_t Buf[NBBUF/LW4];
  unsigned char *cp;
  unsigned char t;
  double g;

  for (is = 0; is < Nreq; ) {

    /* Read data from the audio file */
    N = MINV (NBBUF / LW4, Nreq - is);
    Nr = FREAD (Buf, LW4, N, AFp->fp);

    /* Byte swap and convert to double */
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
