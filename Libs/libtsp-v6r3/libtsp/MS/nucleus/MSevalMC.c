/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double MSevalMC (double x, double x1, double x2, double y1, double y2,
		   double d1, double d2)

Purpose:
  Evaluate a cubic given values and derivatives at two reference points

Description:
  This function calculates the value of an interpolating cubic for a given
  abscissa value.  The cubic is specified in terms of its values and
  derivatives at two points.

Parameters:
  <-  double MSevalMC
      Interpolated value
   -> double x
      Abscissa value at which the cubic is to be evalutated
   -> double x1
      Abscissa value at the first reference point
   -> double x2
      Abscissa value at the second reference point
   -> double y1
      Value at the first reference point
   -> double y2
      Value at the second reference point
   -> double d1
      Derivative at the first reference point
   -> double d2
      Derivative at the second reference point

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.9 $  $Date: 2003/05/09 02:31:19 $

-------------------------------------------------------------------------*/

#include <assert.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>


double
MSevalMC (double x, double x1, double x2, double y1, double y2, double d1,
	  double d2)

{
  double dx, dx1, dx2, val;

/* Check for distinct X values */
  dx = x2 - x1;

  if (dx == 0.0) {

    /* Single point */
    assert (y1 == y2 && d1 == d2);
    val = y1 + d1 * (x - x1);
  }
  else {

    /* Evaluate the cubic, let the compiler find the common expressions.
       This form of the result is symmetrical with respect to y1 and y2,
       giving the correct result at both x1 and x2. */
    dx1 = (x-x1) / dx;
    dx2 = (x2-x) / dx;
    val = dx1*dx2 * (d1 * (x2-x) - d2 * (x-x1)) +
          dx2*(2.0*dx1*dx2 + dx2) * y1 + dx1*(2.0*dx1*dx2 + dx1) * y2;
  }

  return val;
}
