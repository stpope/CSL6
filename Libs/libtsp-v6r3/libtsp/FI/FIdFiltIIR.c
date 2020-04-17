/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void FIdFiltIIR (const double x[], double y[], int Nout, const double h[][5],
		   int Nsec)

Purpose:
  Filter a signal using a cascade of biquadratic IIR filters

Description:
  This procedure forms the output of a cascade of IIR filters.  Each IIR filter
  is a biquadratic section with the z-transform,

             h(i,0)*z^2 + h(i,1)*z + h(i,2)
    H(i,z) = ------------------------------ .
                z^2 + h(i,3)*z + h(i,4)

  Each filter section needs an input sample, two past input samples and two
  past output samples to calculate each output sample.  In the cascade
  connection, the output of one section is the input to next section.  This
  means that past output samples for one section are also the past input
  samples for the next section.  For Nsec sections, the total filter "memory"
  required is mem=2*(Nsec+1) values.  In this routine, the input array has 2
  extra elements at the beginning to store the two past inputs.  The output
  array must be primed with 2 past outputs for each of the Nsec sections.

  On input, the first two elements of the input array (x[0] and x[1]) are the
  two previous inputs to the filter; the next Nout elements are new input
  values.  The first 2*Nsec elements of the output array y[] prime the
  calculation.  Elements y[0] and y[1] are the previous outputs of the filter.
  The next two elements are the outputs of the previous to last filter section,
  and so on up to y[2*Nsec-2] and y[2*Nsec-1] which are previous outputs of the
  first section.

  The filter is processed one section at a time.  For the first section, the
  input array is the input.  For the second section, the output from the first
  section is the input and so on.  The output of these sections overlay each
  other.  The computation is arranged such that the overall output of the
  filter appears as Nout samples starting with y[2].  On return, the last
  2*Nsec elements of this array are suitable for priming the array for the
  next invocation.  This priming can be accomplished by moving the top 2*Nsec
  elements of the output array to the bottom of the array.  The input array can
  be handled similarly; the top two elements of x[] serve as the needed memory
  values for the next invocation of this routine.

  The input and output arrays can also be shared.  As output values are
  computed, they can overlay the input values that are no longer needed.
  Consider an array A[] with mem+Nout elements, where mem=2*(Nsec+1).  The
  first 2 elements are previous outputs of the overall filter, the next two
  elements are previous outputs of the penultimate section, and so on up to
  elements A[mem-2] and A[mem-1] which are previous inputs.  The remaining
  Nout values are new input values.  Invoke this routine as
    FIfiltIIR (&A[mem-2], A, Nout, h, Nsec)
  On return, the Nout new output values start at A[2].  The top mem elements
  of A[] are suitable for priming the array for the next invocation.  These mem
  elements should be moved to the bottom of array A[].
  On input:
       A[0] A[1] ...  A[mem-1] A[mem] A[mem+1] ... A[N+mem-1]
       |<-- filter memory -->| |<---   N input values   --->|
  On Ouput:
       A[0] A[1] A[2] A[3] ... A[N] A[N+1] A[N+2] ... A[N+mem-1]
                 |<-- N output values -->|
                               |<---     filter memory     --->|

Parameters:
   -> const double x[]
      Input array with Nout+2 elements.  The first two elements are previous
      input values.  The next Nout values are new input values.
  <-> double y[]
      Input/output array with Nout+2*Nsec elements.  On input, the first 2*Nsec
      elements represent filter memory values.  On output, Nout outputs are
      placed starting at y[2].  The top 2*Nsec elements of the array contain
      new values which can be used as the filter memory for the next invocation
      of this routine.
   -> int Nout
      Number of output samples
   -> const double h[][5]
      Array of filter coefficients for the filter sections.  Each filter
      section is defined by 5 filter coefficients.
   -> int Nsec
      Number of filter sections

Author / revision:
  P. Kabal  Copyright (C) 2005
  $Revision: 1.4 $  $Date: 2005/02/01 13:20:37 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


void
FIdFiltIIR (const double x[], double y[], int Nout, const double h[][5],
	    int Nsec)

{
  int k;
  int i;
  const double *xp;

/* Process the input, one section at a time */
  k = 2 * Nsec;
  xp = &x[0];
  for (i = 0; i < Nsec; ++i) {
    FIdBiquad (xp, &y[k-2], Nout, h[i]);
    k = k - 2;
    xp = &y[k];
  }

  return;
}
