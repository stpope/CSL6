/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void FIfBiquad (const float x[], float y[], int Nout, const float h[5])

Purpose:
  Filter a signal using a biquadratic IIR filter

Description:
  This procedure forms the output of an IIR filter.  The output
  is a biquadratic section with the z-transform,

           h[0]*z^2 + h[1]*z + h[2]
    H(z) = ------------------------ .
             z^2 + h[3]*z + h[4]

  The output is formed using the following relationship,

    y[i] = h[0]*x[i] + h[1]*x[i-1] + h[2]*x[i-2]
                                   - h[3]*y[i-1] - h[4]*y[i-2] .

  The filtering procedure needs the present and past two input samples and the
  past two output samples to form a new output sample.  These are supplied as
  extensions to the input and output arrays.

  The input array x and the output array y can share storage.  Consider an
  array A with Nout+4 elements.  Let the first two elements (A[0] and A[1])
  represent past output values.  The next two elements (A[2] and A[3]) are
  past input values.  The next Nout elements starting with A[4] are new input
  values.  In this case, this routine can be invoked as
    FIbiquad (&A[2], &A[0], Nout, h)
  On output Nout new output values are placed starting at A[2], overlaying the
  input values.  The last two input values (A[Nout+2] and A[Nout+3]) remain
  intact.  These along with the last two output values (A[Nout] and A[Nout+1])
  are suitable for priming the array for processing the next group of samples.
  This can be achieved by moving the top 4 elements of the array to the bottom
  of the array and appending Nout new input values.

Parameters:
   -> const float x[]
      Input array with Nout+2 elements.  The first two elements, x[0] and x[1],
      represent past input samples.  The next Nout samples are the current
      input samples.
  <-> float y[]
      Output array with Nout+2 elements.  The first two elements, y[0] and
      y[1], represent past outputs and must be supplied as input. The remaining
      Nout elements are defined by this routine.
   -> int Nout
      Number of output samples
   -> const float h[5]
      Array of 5 filter coefficients defining the biquadratic filter section

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.12 $  $Date: 2003/05/09 01:29:44 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


void
FIfBiquad (const float x[], float y[], int Nout, const float h[5])

{
  int i;

  for (i = 2; i < Nout + 2; ++i) {
    y[i] = h[0] * x[i] + h[1] * x[i-1] + h[2] * x[i-2]
                       - h[3] * y[i-1] - h[4] * y[i-2];
  }

  return;
}
