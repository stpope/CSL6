/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double FIgdelFIR (double w, const float h[], int Ncof)

Purpose:
  Calculate the group delay for an FIR filter

Description:
  The group delay is calculated for the equivalent non-causal filter with
  reference point at the mid-point of the filter.  The delay to the mid-point
  is added to this figure.  The group delay in seconds at frequency f for a
  response with sampling frequency Fs can be calculated as

    gdel = FIgdelFIR (2 * Pi * f / Fs, h, Ncof) / Fs .

Parameters:
   -> double w
      Normalized radian frequency (0 to Pi).
   -> const float h[]
      Array of Ncof filter coefficients
   -> int Ncof
      Number of filter coefficients

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.4 $  $Date: 2003/05/09 01:29:45 $

-------------------------------------------------------------------------*/

#include <math.h>

#include <libtsp.h>


double
FIgdelFIR (double w, const float h[], int Ncof)

{
  int i, j;
  double ai, gdel, he, ho, cosWi, sinWi, Rw, Xw, Rpw, Xpw;

  if (Ncof % 2 == 0) {
    ai = 0.5;
    i = Ncof / 2;
    j = i - 1;
    Rw = 0.0;
  }
  else {
    ai = 1.;
    i = (Ncof + 1) / 2;
    j = i - 2;
    Rw = h[i-1];	/* Re[H(w)] */
  }
  Xw = 0.0;		/* Im[H(w)] */
  Rpw = 0.0;		/* Re[H'(w)] */
  Xpw = 0.0;		/* Im[H'(w)] */

/*
  Calculate the terms for the group delay computation in terms of the
  non-causal filter with reference point at the mid-point.

             d arctan(X(w)/R(w))      X'(w) R(w) - R'(w) X(w)
   Td(w) = - -------------------  = - ----------------------- ,
              d w                         R(w)^2 + X(w)^2

  Symmetric filters:      ho is zero => Xw = 0, Xpw = 0 => numerator zero
  Anti-symmetric filters: he is zero => Rw = 0, Rpw = 0 => numerator zero
*/
  for (; i < Ncof; ++i, --j) {
    he = h[i] + h[j];
    ho = h[i] - h[j];

    cosWi = cos (ai * w);
    sinWi = sin (ai * w);

    Rw += he * cosWi;
    Xw -= ho * sinWi;
    Rpw -= ai * he * sinWi;
    Xpw -= ai * ho * cosWi;

    ai += 1.;
  }

  /* Calculate the group delay */
  gdel = 0.5 * (Ncof - 1) -(Xpw * Rw - Rpw * Xw) / (Rw*Rw + Xw*Xw);

  return gdel;
}
