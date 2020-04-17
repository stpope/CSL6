/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double *MScoefMC (double x1, double x2, double y1, double y2,
		    double d1, double d2)

Purpose:
  Find the coefficients of a cubic given values and slopes

Description:
  This routine calculates the coefficients of a polynomial, given the values
  and slopes at two reference points.  The output is the vector of coefficients
  c[i], where 0 <= i <= 3,

    p(x) = c[3] x^3 + c[2] x^2 + c[1] x + c[0].

  The polynomial is specified on input in terms of values and derivatives at
  two reference points,
                                   d p(x1)        d p(x2)
    y1 = p(x1),  y2 = p(x2),  d1 = -------,  d2 = ------- .
                                     dx             dx

Parameters:
  <-  double *MScoefMC
      Pointer to the array of polynomial coefficients
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
  $Revision: 1.11 $  $Date: 2003/05/09 02:31:19 $

-------------------------------------------------------------------------*/

#include <assert.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>


double *
MScoefMC (double x1, double x2, double y1, double y2, double d1, double d2)

{
  double ap, bp, cp, dp, dX, S, dd1, dd2;
  static double c[4];

/*
   Express the cubic as
     R(u) = A u^3 + B u^2 + C u + D ,
   where u=(x-x1)/(x2-x1) varies between 0 and 1 as x varies between x1 and x2.
   The coefficients of R(u) can be determined by solving the simultaneous
   equations:
      R(0) = y1       | 1  0  0  0 |  | D |   | y1 |
      R(1) = y2   or  | 1  1  1  1 |  | C | = | y2 |
     R'(0) = du1      | 0  1  0  0 |  | B |   | du1|   du1 = d1 (x2-x1)
     R'(1) = du2      | 0  1  2  3 |  | A |   | du2|   du2 = d2 (x2-x1)
   The polynomial coefficients for R(u) are:
     D = y1                     = y1
     C = du1                    = d1 dX
     B = 3 (y2-y1) - 2 du1 - du2 = (3S - 2d1 - d2) dX
     A = -2 (y2-y1) + du1 + du2  = (-2S + d1 + d2) dX
   where S=(y2-y1)/(x2-x1) and dX=(x2-x1).

   Define a new polynomial in v, where v = x-x1 = u dX,
     Q(v)=a' v^3 + b' v^2 + c' v + d' ,
   The coefficients of Q(v) can be related to those of R(u),
     d' = D       = y1
     c' = C/dX    = d1
     b' = B/dX^2  = (3S - 2 d1 - d2)/dX
     a' = A/dX^3  = (-2S + d1 + d2)/dX^2

   Finally, consider the transformation x = v+x1, giving
     P(x) = a x^3 + b x^2 + c x + d .
   Substituting v=x-x1 in Q(v) and expanding out the terms, we get
     d = -a' x1^3 + b' x1^2 - c' x1 + d'
     c = 3 a' x1^2 - 2 b' x1 + c'
     b = -3 a' x1 + b'
     a = a'
*/
  if (x1 != x2) {

    dX = x2 - x1;
    S = (y2 - y1) / dX;
    dd1 = (d1 - S) / dX;
    dd2 = (d2 - S) / dX;
    dp = y1;
    cp = d1;
    bp = -(dd1 + dd2 + dd1);
    ap = (dd1 + dd2) / dX;

    c[0] = dp - x1 * (cp - x1 * (bp - x1 * ap));
    c[1] = cp - x1 * (2.0 * bp - x1 * (3.0 * ap));
    c[2] = bp - x1 * (3.0 * ap);
    c[3] = ap;
  }

  else {

    /* Zero size interval */
    assert (y1 == y2 && d1 == d2);
    c[0] = y1 - d1 * x1;
    c[1] = d1;
    c[2] = 0.0;
    c[3] = 0.0;
  }

  return c;
}
