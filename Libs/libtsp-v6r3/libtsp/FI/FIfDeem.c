/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void FIfDeem (double a, float *Fmem, const float x[], float y[], int Nout)

Purpose:
  Deemphasize a signal using a first order recursive filter

Description:
  The procedure processes an input signal using a first order recursive filter.
  The output is formed as follows.
    y[n] = x[n] + a * y[n-1] .
  The deemphasis factor a is zero for no filtering and equal to one for a full
  digital integrator.  The filter memory Fmem is used to store the previous
  output value.  Before processing any data, it is typically set to zero.  On
  return Fmem is equal to the last output value.  For block processing of a
  signal, the value of Fmem from one call is suitable as input to the next
  call.

Parameters:
   -> double a
      Deemphasis factor
  <-> float *Fmem
      Filter memory value. Fmem is the last output value from the last block
      processed (initially set to zero).  On return it is set to the last
      output value (y[Nout-1]).  For block processing of a signal, the value
      returned from one invocation is suitable as input for the next
      invocation.
   -> const float x[]
      Input array of Nout data values.
  <-  float y[]
      Output array of Nout values.  This array can share storage with x.
   -> int Nout
      Number of output samples to be calculated

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.7 $  $Date: 2003/05/09 01:29:44 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


void
FIfDeem (double a, float *Fmem, const float x[], float y[], int Nout)

{
  int i;
  float c, temp;

  /* Loop over the samples */
  c = (float) a;
  temp = *Fmem;
  for (i = 0; i < Nout; ++i) {
    temp = x[i] + c * temp;
    y[i] = temp;
  }

  /* New filter memory value */
  *Fmem = temp;
}
