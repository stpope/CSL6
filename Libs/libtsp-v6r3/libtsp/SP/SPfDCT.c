/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void SPfDCT (float x[], int N, int Ifn)

Purpose:
  Calculate the discrete cosine transform

Description:
  This routine calculates the discrete cosine transform (DCT) or the inverse
  DCT of N data values using a complex N/2 point fast Fourier transform
  algorithm.  This routine performs the calculation in place, that is the
  output data replaces the input data.  The DCT is defined as,

                N-1            pi (2n+1) k
    X(k) = C(k) SUM x(n) * cos(-----------) ,
                n=0               2N

    where C(k) = 1       for k=0,
               = sqrt(2) for k=1,2,...,N-1.

  The inverse DCT is defined as,

           1  N-1                 pi (2n+1) k
    x(n) = -  SUM C(k)*X(k) * cos(-----------) .
           N  k=0                     2N

  Reference:
      M. J. Narasimha and A. M. Peterson, "On the computation of the Discrete
      Cosine Transform", IEEE Trans. Communications, vol. COM-26, June 1978,
      pp. 934-936.

Parameters:
  <-> float x[]
      For the forward transform, this is an array of data samples on input and
      the array of DCT coefficients on output.  For the inverse transform, this
      is an array of DCT coefficients on input and the data samples on output.
   -> int N
      Number of points in the transform (a power of 2)
   -> int Ifn
      Input parameter, equal to +1 for the DCT, equal to -1 for the inverse DCT

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.13 $  $Date: 2003/05/09 02:45:03 $

-------------------------------------------------------------------------*/

#include <math.h>
#include <libtsp.h>
#include <libtsp/nucleus.h>

#define PI_2		1.57079632679489661923F
#define SQRT2F		1.41421356237309504880F
#define SQRT1_2F	0.70710678118654752440F


void
SPfDCT (float x[], int N, int Ifn)

{
  int i;
  float tmp, xi, yi, cc, ss;
  double piby2N;

  if (Ifn >= 0) {

/* Forward transform */
/*
   Permute the input array:
   First, the odd-numbered coefficients are placed in the bottom half of the
   array, while even-numbered coefficients are placed in the top half of the
   array.  Second, the order of the elements in the top half of the array is
   inverted.
    x'(i)     = x(2i)
    x'(N-1-i) = x(2i+1)
*/
    SPfTrMat2 (x, N, 1);
    for (i = 0; i < N/4; ++i) {
      tmp      = x[N/2+i];
      x[N/2+i] = x[N-i-1];
      x[N-i-1] = tmp;
    }

/* FFT of a real sequence */
    SPfRFFT (x, N, 1);

/* Complex multiply and scaling
   - The real part of the FFT is x(k), k=0,...,N/2.  The imaginary part of the
     FFT is x(N/2+k), k=1,...,N/2-1, (the first and last imaginary components
     are known to be zero).
   - Multiply the FFT response by exp(-j*pi*k/2N)
     Find the real and negative of the imaginary parts of the result,
       x'(k)    = Re[exp(-j*pi*k/2N) * (x(k)+jx(N/2+k))],
       x'(N/2+k)=-Im[exp(-j*pi*k/2N) * (x(k)+jx(N/2+k))],
     with the special cases,
       x'(0)    = x(0),
       x'(N/2)  = cos(pi/4)*x(N/2).
   - Multiply the results (except x'(0)) by Rt(2).  Note that x'(N/2) remains
     unchanged since cos(pi/4)=1/Rt(2).
*/
    piby2N = PI_2 / N;
    for (i = 1; i < N/2; ++i) {
      xi = x[i];
      yi = x[N/2+i];
      cc = (float) cos (i * piby2N);
      ss = (float) sin (i * piby2N);
      x[i]    =  SQRT2F * (cc * xi + ss * yi);
      x[N/2+i]= -SQRT2F * (cc * yi - ss * xi);
    }

/* Permute the output array
     x'(N/2+i)=x(N-i),
     x'(N-i)  =x(N/2+i),  i=1,...,N/4-1.
*/
    for (i = 1; i < N/4; ++i) {
      tmp     = x[N/2+i];
      x[N/2+i]= x[N-i];
      x[N-i]  = tmp;
    }
  }

  else {

/* Inverse transform */
/*
   The steps in the inverse DCT can be viewed as undoing the permutations
   performed by the DCT
     x'(N/2+i) = x(N-i),
     x'(N-i)   = x(N/2+i), i=1,...,N/4-1.
*/
    for (i = 1; i < N/4; ++i) {
      tmp      = x[N/2+i];
      x[N/2+i] = x[N-i];
      x[N-i]   = tmp;
    }

/* Complex multiply and scaling */
/*
   Multiply the response by exp(j*pi*k/2N), then take the real part of
   the result.
*/
    piby2N = PI_2 / N;
    for (i = 1; i < N/2; i++) {
      xi = x[i];
      yi = x[N/2+i];
      cc = (float) cos (i * piby2N);
      ss = (float) sin (i * piby2N);
      x[i]     = SQRT1_2F * (xi * cc + yi * ss);
      x[N/2+i] = SQRT1_2F * (xi * ss - yi * cc);
    }

/* FFT corresponding to a real sequence */
    SPfRFFT (x, N, -1);

/* Permute the output array
   First, the order of the elements in the top half of the array is inverted.
   Second, the odd numbered coefficients are placed in the bottom half of the
   array, while even numbered coefficients are placed in the top half of the
   array.
     x'(i)     = x(2i)
     x'(N-1-i) = x(2i+1)
*/
    for (i = 0; i < N/4; ++i) {
      tmp      = x[N/2+i];
      x[N/2+i] = x[N-i-1];
      x[N-i-1] = tmp;
    }
    SPfTrMat2 (x, N, -1);
  }
  return;
}
