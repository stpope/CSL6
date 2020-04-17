/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFdWrI1 (AFILE *AFp, const double Dbuff[], int Nval)
  int AFdWrI2 (AFILE *AFp, const double Dbuff[], int Nval)
  int AFdWrI3 (AFILE *AFp, const double Dbuff[], int Nval)
  int AFdWrI4 (AFILE *AFp, const double Dbuff[], int Nval)

Purpose:
  Write 8-bit integer data to an audio file (double input values)
  Write 16-bit integer data to an audio file (double input values)
  Write 24-bit integer data to an audio file (double input values)
  Write 32-bit integer data to an audio file (double input values)

Description:
  This routine writes a specified number of integer samples to an audio file.
  The input to this routine is a buffer of double values.

Parameters:
  <-  int AFdWrIx
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
#define INT3_MAX	8388607
#define INT3_MIN	-8388608

#define FWRITE(buf,size,nv,fp)	(int) fwrite ((const char *) buf, \
					      (size_t) size, (size_t) nv, fp)


int
AFdWrI1 (AFILE *AFp, const double Dbuff[], int Nval)

{
  int is, N, Nw, i;
  int1_t Buf[NBBUF/LW1];
  double g, Dv;

/* Write data to the audio file */
  is = 0;
  g = AFp->ScaleF;
  while (is < Nval) {
    N = MINV (NBBUF / LW1, Nval - is);
    for (i = 0; i < N; ++i) {
      Dv = g * Dbuff[i+is];
      if (Dv >= 0.0) {
	Dv += 0.5;
	if (Dv >= INT1_MAX + 1) {
	  ++AFp->Novld;
	  Dv = INT1_MAX;
	}
      }
      else {
	Dv += -0.5;
	if (Dv <= INT1_MIN - 1) {
	  ++AFp->Novld;
	  Dv = INT1_MIN;
	}
      }
      Buf[i] = (int1_t) Dv;
    }
    Nw = FWRITE (Buf, LW1, N, AFp->fp);
    is += Nw;
    if (Nw < N)
      break;
  }

  return is;
}

int
AFdWrI2 (AFILE *AFp, const double Dbuff[], int Nval)

{
  int is, N, Nw, i;
  int2_t Buf[NBBUF/LW2];
  double g, Dv;
  unsigned char *cp;
  unsigned char t;

/* Write data to the audio file */
  is = 0;
  g = AFp->ScaleF;
  while (is < Nval) {
    N = MINV (NBBUF / LW2, Nval - is);
    for (i = 0; i < N; ++i) {
      Dv = g * Dbuff[i+is];
      if (Dv >= 0.0) {
	Dv += 0.5;
	if (Dv >= INT2_MAX + 1) {
	  ++AFp->Novld;
	  Dv = INT2_MAX;
	}
      }
      else {
	Dv += -0.5;
	if (Dv <= INT2_MIN - 1) {
	  ++AFp->Novld;
	  Dv = INT2_MIN;
	}
      }
      Buf[i] = (int2_t) Dv;
      if (AFp->Swapb == DS_SWAP) {
	cp = (unsigned char *) &Buf[i];
	t = cp[1]; cp[1] = cp[0]; cp[0] = t;
      }
    }
    Nw = FWRITE (Buf, LW2, N, AFp->fp);
    is += Nw;
    if (Nw < N)
      break;
  }

  return is;
}


int
AFdWrI3 (AFILE *AFp, const double Dbuff[], int Nval)

{
  int is, N, Nw, i, j, Hbo;
  int4_t Iv;
  unsigned char Buf[NBBUF];
  double g, Dv;
  unsigned char *cp;

/* Write data to the audio file */
  Hbo = UTbyteOrder ();
  cp = (unsigned char *) &Iv;
  is = 0;
  g = AFp->ScaleF;
  while (is < Nval) {
    N = MINV (NBBUF / LW3, Nval - is);
    for (i = 0, j = is; i < LW3*N; i += LW3, ++j) {
      Dv = g * Dbuff[j];
      if (Dv >= 0.0) {
	Dv += 0.5;
	if (Dv >= INT3_MAX + 1) {
	  ++AFp->Novld;
	  Dv = INT3_MAX;
	}
      }
      else {
	Dv += -0.5;
	if (Dv <= INT3_MIN - 1) {
	  ++AFp->Novld;
	  Dv = INT3_MIN;
	}
      }
      if (Hbo == DS_EL)
	Iv = (int4_t) Dv;		/* DS_EL:  X  2  1  0  */
      else				/*        MSB      LSB */
	Iv = 256 * ((int4_t) Dv);       /* DS_EB:  0  1  2  X  */
      if (AFp->Swapb == DS_SWAP) {
	Buf[i] = cp[2];
	Buf[i+1] = cp[1];
	Buf[i+2] = cp[0];
      }
      else {
	Buf[i] = cp[0];
	Buf[i+1] = cp[1];
	Buf[i+2] = cp[2];
      }
    }

    Nw = FWRITE (Buf, LW3, N, AFp->fp);
    is += Nw;
    if (Nw < N)
      break;
  }

  return is;
}

int
AFdWrI4 (AFILE *AFp, const double Dbuff[], int Nval)

{
  int is, N, Nw, i;
  int4_t Buf[NBBUF/LW4];
  double g, Dv;
  unsigned char *cp;
  unsigned char t;

/* Write data to the audio file */
  is = 0;
  g = AFp->ScaleF;
  while (is < Nval) {
    N = MINV (NBBUF / LW4, Nval - is);
    for (i = 0; i < N; ++i) {
      Dv = g * Dbuff[i+is];
      if (Dv >= 0.0) {
	Dv += 0.5;
	if (Dv >= (double) INT4_MAX + 1.) {
	  ++AFp->Novld;
	  Dv = INT4_MAX;
	}
      }
      else {
	Dv += -0.5;
	if (Dv <= (double) (INT4_MIN) - 1.) {
	  ++AFp->Novld;
	  Dv = INT4_MIN;
	}
      }
      Buf[i] = (int4_t) Dv;
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
