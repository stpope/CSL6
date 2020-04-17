/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double MSdNint (double x)

Purpose:
  Nearest integer function

Description:
  This routine returns the integer-valued double nearest x.  Specifically, the
  returned value is floor(x+0.5) for positive x and ceil(x-0.5) for negative
  x.

Parameters:
  <-  double MSdNint
      Returned integer value
   -> double x
      Input value

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.7 $  $Date: 2003/05/09 02:29:38 $

-------------------------------------------------------------------------*/

#include <math.h>	/* floor and ceil */

#include <libtsp.h>


double
MSdNint (double x)

{
  double pint;

  if (x >= 0.0)
    pint = floor (x + 0.5);
  else
    pint = ceil (x - 0.5);

  return pint;
}
