/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void SPdRFFT (double x[], int N, int Ifn)

Purpose:
  Fast Fourier transform of a real sequence

Description:
  If Ifn is greater than zero, this routine computes the discrete Fourier
  transform of N real data points.  The first step in this process is to
  rearrange the data in-place.  Let the input real sequence be x[0], x[1], ...,
  These values are rearranged to have the even-numbered coefficients first,
  followed by the odd-numbered coefficients.  These rearranged values are
  considered to be the real and imaginary components of a complex sequence.
  This complex sequence is passed to an N/2 point fast Fourier transform.
  The final step is to post-process the output of the FFT to form the output
  values.  Let the N/2+1 complex output values be
    (ar(0),ar(0)), ... , (ar(N/2), ai(N/2))
  These output values are stored as follows.
      ar(0)     <->  x[0]
      ar(1)     <->  x[1]
       ...           ...
      ar(N/2-1) <->  x[N/2-1]
      ar(N/2)   <->  x[N/2]
      ai(1)     <->  x[N/2+1]
      ai(2)     <->  x[N/2+2]
       ...           ...
      ai(N/2-1) <->  x[N-1]
  Note that ar(N/2) is stored in the place that would normally be reserved for
  ai(0).  This is possible since ai(0) (and ai(N/2)) are known a priori to be
  zero.

  If Ifn is less than zero, this routine computes a inverse discrete Fourier
  transform that results in N real data values.  Let the N/2+1 complex input
  values be (ar(0),ai(0)), ... , (ar(N/2),ai(N/2)).  If these values are to
  result in a real sequence after the inverse discrete Fourier transform, ai(0)
  and ai(N/2) must both be zero.  The remaining N data values (N/2+1 real
  components and N/2-1 imaginary components) are stored in the array x as
  follows.
      ar(0)     <->  x[0]
      ar(1)     <->  x[1]
       ...           ...
      ar(N/2-1) <->  x[N/2-1]
      ar(N/2)   <->  x[N/2]
      ai(1)     <->  x[N/2+1]
      ai(2)     <->  x[N/2+2]
       ...           ...
      ai(N/2-1) <->  x[N-1]
  Note that ar(N/2) is stored in the place that would normally be reserved for
  ai(0).  The output of the inverse transform contains the N dreal data points
  stored sequentially in array x.

Parameters:
  <-> double x[]
      Data array (N elements)
   -> int N
      Number of elements in x.  N must be a power of 2.
   -> int Ifn
      Parameter specifying the direction of the transform.  If Ifn > 0, x is
      the time series on input and the frequency response on output.  If
      Ifn < 0, x is the frequency response on input and the time series on
      output.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.3 $  $Date: 2003/05/09 02:45:14 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/nucleus.h>


void SPdRFFT (double x[], int N, int Ifn)

{
  if (N != 1) {

    if (Ifn >= 0) {

      /* Forward transform*/
      SPdTrMat2 (x, N, 1);
      SPdCFFT (x, &x[N/2], N / 2, 1);
      SPdPreFFT (x, &x[N/2], N / 2, 1);
    }

    else {

      /* Inverse transform*/
      SPdPreFFT (x, &x[N/2], N / 2, -1);
      SPdCFFT (x, &x[N/2], N / 2, -1);
      SPdTrMat2 (x, N, -1);
    }
  }
  return;
}
