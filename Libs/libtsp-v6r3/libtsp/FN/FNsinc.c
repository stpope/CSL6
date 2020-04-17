/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double FNsinc (double x)

Purpose:
  Evaluate the sin(pi*x)/(pi*x) function

Description:
  This function returns the value of the sinc function, where
               sin (pi x)
    sinc (x) = ---------- .
                  pi x

Parameters:
  <-  double FNsinc
      Returned value
   -> double x
      Input value

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.6 $  $Date: 2003/05/09 01:41:29 $

-------------------------------------------------------------------------*/

#include <math.h>

#include <libtsp.h>

#define PI	3.14159265358979323846


double
FNsinc (double x)

{
  double xm, val;

  /* Calculate sin(x)/x, taking care at multiples of pi */
  if (x == 0.0)
    val = 1.0;
  else {
    xm = fmod (x, 2.0);
    if (x == floor (x))
      val = 0.0;
    else
      val = sin (PI * xm) / (PI * x);
  }

  return val;
}
