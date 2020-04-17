/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double FIrCosF (double x, double alpha)

Purpose:
  Calculate the raised-cosine function

Description:
  This function calculates a raised-cosine function with its half amplitude
  point at x=1.  The funciton is defined in three parts,
    f(x,a) = 1,                             0  < |x| <= 1-a
    f(x,a) = 0.5 * (sin(pi*(1-x)/2a)+1),   1-a < |x| <= 1+a
    f(x,a) = 0,                            1+a < |x| .
  The function is even, f(-x,a) = f(x,a).  The parameter a (alpha) takes on
  values from zero to one.  For a=0, f(x,0) is a rectangular function for x
  from -1 to +1.  For a=1, f(x,1) is a full raised cosine which extends from
  -2 to +2.

Parameters:
  <-  double FIrCosF
      Returned raised cosine value
   -> double x
      Normalized input argument value (x=1 is the "cutoff" value)
   -> double alpha
      Parameter (0 to 1)

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.5 $  $Date: 2003/05/09 01:30:46 $

-------------------------------------------------------------------------*/

#include <math.h>

#include <libtsp/nucleus.h>

#define PI_2		1.57079632679489661923


double
FIrCosF (double x, double alpha)

{
  double rval;

  if (x < 0.0)
    x = -x;

  if (x <= 1.0 - alpha)
    rval = 1.0;
  else if (x > 1.0 + alpha)
    rval = 0.0;
  else
    rval = 0.5 * (sin (PI_2 * (1.0 - x) / alpha) + 1.0);

  return rval;
}
