/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double FIgdelIIR (double w, const float h[][5], int Nsec)

Purpose:
  Calculate the group delay for a cascade IIR filter

Description:
  This function calculates the group delay of an IIR filter specified by its
  coefficients.  The filter is a cascade of second order sections, with the
  z-transform of the i'th filter section given as

             h(i,0)*z^2 + h(i,1)*z + h(i,2)
    H(i,z) = ------------------------------ .
                z^2 + h(i,3)*z + h(i,4)

  The group delay in seconds at frequency f for a filter with sampling
  frequency Fs can be calculated as

    gdelay = FIgdelIIR (2.0 * Pi * f / Fs, h, Nsec) / Fs  .

Parameters:
   -> double w
      Normalized radian frequency (0 to Pi).
   -> const float h[][5]
      Array of filter coefficients for the filter sections.  Each filter
      section is defined by 5 filter coefficients.
   -> int Nsec
      Number of filter sections

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.6 $  $Date: 2003/05/09 01:29:45 $

-------------------------------------------------------------------------*/

#include <math.h>

#include <libtsp.h>

static double
FI_gdelSec (const float b[3], double cosW, double sinW);


double
FIgdelIIR (double w, const float h[][5], int Nsec)

{
  int k;
  double gdel, cosW, sinW;
  float a[3];

  cosW = cos (w);
  sinW = sin (w);

  /* Sum the group delay of each second order section */
  gdel = 0.0;
  a[0] = 1.0;
  for (k = 0; k < Nsec; ++k) {
    a[1] = h[k][3];
    a[2] = h[k][4];

    /* Delay due to the numerator less the delay due to the denominator */
    gdel += FI_gdelSec (&h[k][0], cosW, sinW) - FI_gdelSec (a, cosW, sinW);
  }

  return gdel;
}

/* Calculate the group delay for a quadratic section,

     B(z) = b[0] z^2 + b[1] z + b[2] .

   If the function is evaluated on the unit circle with z=exp(jw),

     B(w) = exp(jw) [ (b[0]+b[2]) cos(w) + b[1] + j(b[0]-b[2]) sin(w) ] .

  The group delay is the negative derivative with respect to w of the phase
  angle of this function.  If the polynomial has zero coefficients, the group
  delay is set to zero.  Note that since the polynomial is a function of
  positive powers of z, where z can be interpreted to be a time advance of -1
  samples, the group delay is generally a negative value.
*/


static double
FI_gdelSec (const float b[3], double cosW, double sinW)

{
  double gdel, Rw, Rpw, Xw, Xpw;

/*
  The polynomial is
    B(z) = b[0]*z^2 + b[1]*z + b[2]
  On the unit circle, z=exp(jw), express the function as
    B(w) = R(w) + jX(w),
 where

  B(w) = exp(jw) [ (b[0]+b[2]) cos(w) + b[1] + j (b[0]-b[2]) sin(w) ]

  The exponential term adds a delay of -1 sample.  Neglecting this exponential
  term,
    R(w) = b[1] + (b[0]+b[2]) cos(w)
    X(w) = (b[0]-b[2]) sin(w)
  The group delay is

             d arctan(X(w)/R(w))      X'(w) R(w) - R'(w) X(w)
   Td(w) = - -------------------  = - ----------------------- ,
              d w                         R(w)^2 + X(w)^2

  where X'(w) is the derivative of X(w) and R'(w) is the derivative of R(w),
    R'(w) = -(b[0]+b[2]) sin(w)
    X'(w) = (b[0]-b[2]) cos(w)

  Roots on the unit circle present computational problems.  It can be verified
  that the group delay is continuous through the roots.  For that reason, the
  following code detects roots on the unit circle and handles them as special
  cases.  For instance conjugate roots give rise to a symmetrical polynomial,
  which has a constant group delay.  For values near but not on the unit
  circle, numerical problems can still occur.

  The group delay for roots near the unit circle, but not exactly on the unit
  circle, can be very large.  Consider B(z) = (1+e) z^2 + 1.  The group delay
  for w=pi is gdel = -1 - (2+e)/e .
*/

  /* Symmetric polynomial
     - includes the case with conjugate roots on the unit circle
  */
  if (b[0] == b[2]) {
    if (b[0] == 0.0F && b[1] == 0.0F)
      gdel = 0.0;
    else
      gdel = -1.0;
  }

  /* Root at z=-1 (delay -0.5)
   - also handles the case of roots at both z=1 and z=-1 (b[1]=0)
   - the remaining polynomial is b[0]*z + b[2]
  */
  else if (b[0]+b[2] == b[1]) {
    if (b[1] == 0.0)
      gdel = -1.0;
    else {
      /* delay for b[0]*z + b[2] (zero not on the unit circle) */
      Rw = b[0] + b[2] * cosW;
      Rpw = -b[2] * sinW;
      Xw = -b[2] * sinW;
      Xpw = -b[2] * cosW;
      gdel = -1.5 -(Xpw * Rw - Rpw * Xw) / (Rw*Rw + Xw*Xw);
    }
  }

  /* Root at z=1 (delay -0.5)
     - the remaining polynomial is b[0]*z - b[2] (zero not on the unit circle)
  */
  else if (b[0] + b[2] == -b[1]) {
    Rw = b[0] - b[2] * cosW;
    Rpw = b[2] * sinW;
    Xw = b[2] * sinW;
    Xpw = b[2] * cosW;
    gdel = -1.5 -(Xpw * Rw - Rpw * Xw) / (Rw*Rw + Xw*Xw);
  }

  /* General case (no unit circle zeros) */
  else {
    Rw = b[1] +(b[0] + b[2]) * cosW;
    Rpw = -(b[0] + b[2]) * sinW;
    Xw = (b[0] - b[2]) * sinW;
    Xpw = (b[0] - b[2]) * cosW;
    gdel = -1. -(Xpw * Rw - Rpw * Xw) / (Rw*Rw + Xw*Xw);
  }

  return gdel;
}
