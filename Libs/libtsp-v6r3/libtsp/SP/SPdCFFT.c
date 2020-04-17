/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void SPdCFFT (double x[], double y[], int N, int Ifn)

Purpose:
  Fast Fourier transform (complex data)

Description:
  This subroutine calculates the discrete Fourier transform or the inverse
  discrete Fourier transform of a set of complex numbers using the fast Fourier
  transform algorithm developed by G. Sande (mixed radix 4 and radix 2).
  Reference:
      W. M. Gentleman and G. Sande, "Fast Fourier Transforms - For Fun and
      Profit", 1966 Fall Joint Computer Conference.
  The calculation is done in place, that is the output data replaces the input
  data.  The k-th complex output data point of the N point discrete Fourier
  transform is

           N-1           nk
    G(k) = SUM  g(n) * W     ,  where W = exp(-j*2*pi/N),
           n=0

  where j is the imaginary operator, and g(n) is the n-th complex input data
  point.  The k-th complex output data point of the N point inverse discrete
  Fourier transform is

            1  N-1          -nk
    g(n) =  -  SUM  G(k) * W   .
            N  k=0

Parameters:
  <-> double x[]
      Array of length N containing the real part of the data
  <-> double y[]
      Array of length N containing the imaginary part of the data
   -> int  N
      Number of elements in each of the arrays x and y.  N must be a power of
      two.
   -> int Ifn
      Input parameter, positive for the forward transform and negative for the
      inverse transform.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.4 $  $Date: 2003/05/09 02:55:11 $

-------------------------------------------------------------------------*/

#include <math.h>
#include <libtsp.h>
#include <libtsp/SPmsg.h>

#define PIx2		6.28318530717958647693


void
SPdCFFT (double x[], double y[], int N, int Ifn)

{
  int np, log2N;
  int i, j, k, m, m4;
  int j0, j1, j2, j3, jmm4;
  double arg, fm4;
  double temp;
  double c1, s1, c2, s2, c3, s3;
  double ar0, ar1, ar2, ar3;
  double ai0, ai1, ai2, ai3;

/* Find the logarithm (base 2) of the number of points */
  for (np = 1, log2N = 0; np < N; ++log2N)
    np = 2 * np;
  if (N != np)
    UThalt ("SPdCFFT: %s: %d", SPM_BadTrLen, N);

/* For the inverse transform, form the complex conjugate of the input data */
  if (Ifn < 0) {
    for (i = 0; i < N; ++i)
      y[i] = -y[i];
  }

  /* Stop compiler warnings */
  c1 = 0.0;
  s1 = 0.0;
  c2 = 0.0;
  s2 = 0.0;
  c3 = 0.0;
  s3 = 0.0;

/* Calculate the radix 4 fast Fourier transform */
  m4 = N;
  for (k = 0; k < log2N / 2; ++k) {
    m = m4 / 4;
    fm4 = PIx2 / m4;
    for (j = 0; j < m; ++j) {

      /* Calculate the twiddle factors */
      if (j != 0) {
	arg = j * fm4;
	c1 = cos (arg);
	s1 = sin (arg);
	c2 = c1 * c1 - s1 * s1;		/* cos (2*arg) */
	s2 = c1 * s1 + c1 * s1;		/* sin (2*arg) */
	c3 = c2 * c1 - s2 * s1;		/* cos (3*arg) */
	s3 = c2 * s1 + s2 * c1;		/* sin (3*arg) */
      }
      jmm4 = j - m4;
      for (i = m4; i <= N; i += m4) {
	j0 = i + jmm4;
	j1 = j0 + m;
	j2 = j1 + m;
	j3 = j2 + m;
	ar0 = x[j0] + x[j2];
	ar1 = x[j0] - x[j2];
	ai0 = y[j0] + y[j2];
	ai1 = y[j0] - y[j2];
	ar2 = x[j1] + x[j3];
	ar3 = x[j1] - x[j3];
	ai2 = y[j1] + y[j3];
	ai3 = y[j1] - y[j3];
	x[j0] = ar0 + ar2;
	y[j0] = ai0 + ai2;

	/* Multiply by the twiddle factors */
	if (j != 0) {
	  x[j2] = c1 * (ar1 + ai3) + s1 * (ai1 - ar3);
	  y[j2] = c1 * (ai1 - ar3) - s1 * (ar1 + ai3);
	  x[j1] = c2 * (ar0 - ar2) + s2 * (ai0 - ai2);
	  y[j1] = c2 * (ai0 - ai2) - s2 * (ar0 - ar2);
	  x[j3] = c3 * (ar1 - ai3) + s3 * (ai1 + ar3);
	  y[j3] = c3 * (ai1 + ar3) - s3 * (ar1 - ai3);
	}
	else {
	  /* Twiddle factors are one and zero */
	  x[j2] = ar1 + ai3;
	  y[j2] = ai1 - ar3;
	  x[j1] = ar0 - ar2;
	  y[j1] = ai0 - ai2;
	  x[j3] = ar1 - ai3;
	  y[j3] = ai1 + ar3;
	}
      }
    }
    m4 = m;
  }

/* Pick up any extra factors of two to complete the transform */
  if (log2N % 2 != 0) {
    for (i = 0; i < N; i += 2) {
      j = i + 1;
      ar0 = x[i] + x[j];
      ai0 = y[i] + y[j];
      x[j] = x[i] - x[j];
      y[j] = y[i] - y[j];
      x[i] = ar0;
      y[i] = ai0;
    }
  }

/* Unscramble the output arrays
   This loop implements a bit-reversed counter: for each i, j is the
   bit-reversed index
*/
  j = 0;
  for (i = 1; i <= N - 2; ++i) {
    np = N / 2;
    while (j >= np) {
      j = j - np;
      np = np / 2;
    }
    j = j + np;
    if (j > i) {
      temp = x[i];
      x[i] = x[j];
      x[j] = temp;
      temp = y[i];
      y[i] = y[j];
      y[j] = temp;
    }
  }

/* Inverse transform: complex conjugate of the scaled output array */
  if (Ifn < 0) {
    for (i = 0; i < N; ++i) {
      x[i] =  x[i] / N;
      y[i] = -y[i] / N;
    }
  }

  return;
}
