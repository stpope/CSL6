/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void FIfPreem (double a, float *Fmem, const float x[], float y[], int Nout)

Purpose:
  Preemphasize a signal using a first difference filter

Description:
  The procedure processes an input signal using a first difference filter.  The
  output is formed as follows.
    y[n] = x[n] - a * x[n-1] .
  The preemphasis factor pre is zero for no filtering and equal to one for a
  full first difference.  The filter memory Fmem is used to store the previous
  input value.  Before processing any data, it is typically set to zero.  On
  return, Fmem is equal to the last input data element processed.  For block
  processing of a signal, the value of Fmem from one call is suitable as input
  to the next call.

Parameters:
   -> double a
      Preemphasis factor
  <-> float *Fmem
      Filter memory value. Fmem should be set to the value of the data sample
      immediately preceding the data in the array x.  On return it is set to
      the last input value processed (x[Nout-1]).  For block processing of a
      signal, the value returned from one invocation is suitable as input for
      the next invocation.
   -> const float x[]
      Input array of Nout data values.
  <-  float y[]
      Output array of Nout values.  This array can share storage with x.
   -> int Nout
      Number of output samples to be calculated

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.9 $  $Date: 2003/05/09 01:29:45 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


void
FIfPreem (double a, float *Fmem, const float x[], float y[], int Nout)

{
  int i;
  float c, temp, Fm;

  /* Loop over the samples */
  c = (float) a;
  Fm = *Fmem;
  for (i = 0; i < Nout; ++i) {
    temp = x[i];
    y[i] = temp - c * Fm;
    Fm = temp;
  }

  /* New filter memory value */
  *Fmem = Fm;
}
