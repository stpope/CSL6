/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void VRdRFFTMSq (const double X[], double X2[], int N)

Purpose:
  Calculate the magnitude squared spectrum for the DFT of a real sequence

Description:
  This routine calculates the magnitude squared of the DFT of a real sequence.
  The output of the DFT (for instance SPdRFFT) is stored in an array with N
  elements, with the real part preceding the imaginary part. This DFT output is
  the input to this routine.

  The input values to this routine are ar(.), the real part of the DFT, and
  ai(.), the imaginary part of the DFT. For N even, the values are stored as
  follows.
      ar(0)     <->  X[0]
      ar(1)     <->  X[1]
       ...           ...
      ar(N/2-1) <->  X[N/2-1]
      ar(N/2)   <->  X[N/2]
      ai(1)     <->  X[N/2+1]
      ai(2)     <->  X[N/2+2]
       ...           ...
      ai(N/2-1) <->  X[N-1]
  The values ai(0) and ai(N/2) are known, a priori, to be zero. The output of
  this routine is a vector of N/2+1 values calculated as follows.
      X2[k] = ar(k)^2 + ai(k)^2   0 <= k <= N/2.

  This routine also works for N odd, in which case there are (N+1)/2 output
  values.

Parameters:
   -> const double X[]
      Input array (N elements)
  <-  double X2[]
      Output array (N/2+1 (N even) or (N+1)/2 (N odd) elements).  The output
      array can be the same as the input array.
   -> int N
      Number of elements in the input array (may be zero)

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.3 $  $Date: 2003/05/09 03:33:10 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


void
VRdRFFTMSq (const double X[], double X2[], int N)

{
  int k, Ns, Nv;

/* Handle both N even and N odd */
/* For N even
             real     imaginary
   k = 0     X[0]      --
   k = 1     X[1]     X[Ns+1]     Ns = N/2
   k         X[k]     X[Ns+k]
   k = Nv-1  X[Nv-1]  X[Ns+Nv-1]  Nv = N - Ns
   k = Nv    X[Nv]     --

   For N odd
             real     imaginary
   k = 0     X[0]      --
   k = 1     X[1]     X[Ns+1]     Ns = (N-1)/2
   k         X[k]     X[Ns+k]
   k = Nv-1  X[Nv-1]  X[Ns+Nv-1]  Nv = N - Ns
*/

  if (N > 0)
    X2[0] = X[0] * X[0];
  Ns = N/2;
  Nv = N - Ns;
  for (k = 1; k < Nv; ++k)
    X2[k] = X[k] * X[k] + X[Ns+k] * X[Ns+k];
  if (k == N - k)
    X2[k] = X[k] * X[k];

  return;
}
