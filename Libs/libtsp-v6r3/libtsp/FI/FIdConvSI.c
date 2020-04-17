/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void FIdConvSI (const double x[], double y[], int Nout, const double h[],
		  int Ncof, int mr, int Nsub, int Ir)

Purpose:
  Filter a signal with an FIR filter (sample rate change)

Description:
  This procedure convolves a set of filter coefficients with an array of data.
  Optionally, the input signal can be interpolated before filtering, and the
  output signal can be subsampled.

  The process of interpolation is equivalent to the conceptual notion of
  inserting Ir-1 zeros between each element of the input array.  This increased
  rate signal array is convolved with the filter coefficients.  Since the
  increased rate signal array has imbedded zeros, only every Ir'th filter
  coefficient is involved in producing a given output point.  These
  coefficients can be considered to constitute a sub-filter.  Sub-filters
  are used in a round-robin fashion to produce successive output points.

  The process of subsampling is equivalent to keeping only every Nsub'th output
  point.  The filter slides along Nsub elements at a time in the increased rate
  (interpolated) signal array for every point that is stored in the output
  array.

  The input array is x[.].  The first lmem = (Ncof-1)/Ir samples of x[.] are
  past inputs.  Let the increased rate array be xi[.].  Then, xi[l*Ir]=x[l],
  with other elements in xi[.] being zero.  The relationship between indices
  of  x[l] and xi[m] is m=l*Ir+mr, or l=floor(m/Ir), mr=m-l*Ir.  The first
  output point is calculated with filter coefficient h[0] aligned with sample
  xi[Ir*lmem+mr], or equivalently, h[mr] aligned with sample x[lmem].  The
  last output point is calculated with h[0] aligned with the sample
  xi[Ir*lmem+mr+(Nout-1)*Nsub].

  If Nout output values are to be calculated, the input array x[.] must have Nx
  elements, where Nx is determined from
    lmem*Ir+mr+(Nout-1)*Nsub <= Ir*Nx-1
  From this relationship,
    Nx = lmem + ceil((mr+1+(Nout-1)*Nsub)/Ir)
       = lmem + floor((mr+(Nout-1)*Nsub)/Ir) + 1
  Conversely if the input array has Nx elements, the number of output samples
  that can be calculated as
    Nout = floor((Ir*(Nx-lmem)-1-mr)/Nsub) + 1
         = ceil((Ir*(Nx-lmem)-mr)/Nsub).

Parameters:
   -> const double x[]
      Input array of data.  Let lmem=(Ncof-1)/Ir.  The first output point is
      calculated as follows
        y[0] = h[mr]*x[lmem] + h[mr+Ir]*x[lmem-1]
                             + h[mr+2*Ir]*x[lmem-2] + ...
      The array x must have at least lmem+((Nout-1)*Nsub+mr)/Ir+1 elements.
  <-  const double y[]
      Array of output samples
   -> int Nout
      Number of output samples to be calculated
   -> const double h[]
      Array of Ncof filter coefficients
   -> int Ncof
      Number of filter coefficients
   -> int mr
      Filter coefficient offset.  The first output point has filter coefficient
      h[mr] aligned with data element x[lmem].  Normally mr is in the range 0
      to Ir-1.
   -> int Nsub
      Subsampling ratio.  Only every Nsub'th filtered output is calculated for
      the interpolated sequence and stored in the output array.
   -> int Ir
      Interpolating ratio.  Conceptually, Ir-1 zeros are inserted between each
      element of the input array to create an interpolated sequence before
      convolving with the filter coefficients.

Author / revision:
  P. Kabal  Copyright (C) 2005
  $Revision: 1.1 $  $Date: 2005/02/01 13:19:58 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


void
FIdConvSI (const double x[], double y[], int Nout, const double h[], int Ncof,
	   int mr, int Nsub, int Ir)

{
  int l, m, lp, j, ia;
  double sum;

  /* The pair (l,mr) is an index ==>  xi[l*Ir+mr} */
  l = (Ncof - 1) / Ir;		/* Initial l = lmem */

  /* Loop over output points */
  for (m = 0; m < Nout; ++m) {

    /* Convolution with subfilter mr */
    sum = 0.0;
    lp = l;
    for (j = mr; j < Ncof; j += Ir) {
      sum += h[j] * x[lp];
      --lp;
    }
    y[m] = sum;

    /* Advance the filter Nsub (interpolated) samples, recalculate (l,mr) */
    mr += Nsub;
    ia = mr / Ir;
    mr = mr - ia * Ir;
    l += ia;
  }

  return;
}
