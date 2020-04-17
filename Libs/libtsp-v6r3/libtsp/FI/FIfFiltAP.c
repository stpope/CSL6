/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void FIfFiltAP (const float x[], float y[], int Nout, const float h[],
		  int Ncof)

Purpose:
  Filter a signal with an all-pole IIR filter

Description:
  The procedure filters an input signal using an all-pole filter.  The filter
  is specified by its direct-form feeback coefficients h[i],
            1                    N-1       -i
    H(z) = ----  ,  where C(z) = SUM h[i] z    .
           C(z)                  i=0

  The output is calculated as
    h[0]*y[k] = x[k] - h[1]*y[k-1] - h[2]*y[k-2] - ...
                                   - h[Ncof-1]*y[k-Ncof+1].
  Normally, the first coefficient is unity, i.e. h[0]=1.  A non-unity value
  will result in a gain scaling of the response.

  The output array y[.] must be primed with mem=Ncof-1 previous output values.
  On return the top mem values in y[.] can serve as the memory values for the
  next invocation of this routine.

  This routine can also be called with a single array x[.] of length mem+Nout,
    FIfFiltAP (&x[mem], x, Nout, h, Ncof)
  Before entering this routine, the first mem values of x[.] are set to
  previous output values, while the next Nout values are new input values.
  On return, the Nout new output values replace the input values, starting at
  x[mem].  If the top mem elements are shifted down to the bottom of the array,
  these can serve as the previous output values for the next call to this
  routine.

Parameters:
   -> const float x[]
      Input array of data, Nout values
  <-> float y[]
      Output array of samples.  This array is of length Ncof-1+Nout.  On input,
      the first Ncof-1 values should represent past output values.  On output,
      Nout new output samples are appended to this array.
   -> int Nout
      Number of new output samples to be calculated
   -> const float h[]
      Array of Ncof all-pole filter coefficients.  The first coefficient must
      be non-zero.
   -> int Ncof
      Number of filter coefficients

Author / revision:
  P. Kabal  Copyright (C) 2005
  $Revision: 1.16 $  $Date: 2005/02/01 13:21:48 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/FImsg.h>


void
FIfFiltAP (const float x[], float y[], int Nout, const float h[], int Ncof)

{
  int m;
  int j;
  double sum;
  float *yp;

  if (h[0] == 0.0F)
    UThalt ("FIfFiltAP: %s", FIM_NonCausal);

  /* Loop over output points */
  yp = &y[Ncof-1];
  for (m = 0; m < Nout; ++m) {
    sum = x[m];
    for (j = 1; j < Ncof; ++j)
      sum = sum - h[j] * yp[m-j];
    yp[m] = (float) sum / h[0];
  }

  return;
}
