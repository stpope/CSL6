/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double FNbessI0 (double x)

Purpose:
  Evaluate the zeroth-order modified Bessel function of first kind

Description:
  This function returns the value of the zero'th order modified Bessel function
  of the first kind.  This function is defined as
              1   2 pi
    I0(z) = ----  Int exp (z cos(a)) da.
            2 pi   0
  This routine calculates an approximation using an infinite series.  This
  series is rapidly converging for moderate values of z.

Parameters:
  <-  double FNbessI0
      Returned value
   -> double x
      Input value

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.3 $  $Date: 2003/05/09 01:41:29 $

-------------------------------------------------------------------------*/

#include <libtsp.h>

#define EPS	1E-16

/*
  This routine uses a series expansion
                inf   (x/2)^k
    Io(x) = 1 + SUM [ ------- ]^2
                k=1     k!

  The stopping criterion is that the last term has a relative error smaller
  than eps.  The number of terms required is shown below.
     eps = 1e-8    eps = 1e-12    eps = 1e-16
      x     N       x     N        x     N
     ----------    -----------    -----------
       0    1        0    1         0    1
     0.1    3      0.1    3       0.1    5
     0.2    4      0.2    4       0.2    6
     0.5    5      0.5    5       0.5    8
       1    6        1    8         1    9
       2    8        2   10         2   12
       5   12        5   14         5   17
      10   17       10   20        10   23
      20   25       20   30        20   34
      50   48       50   54        50   60
     100   81      100   89       100   97
*/


double
FNbessI0 (double x)

{
  double xh, sum, pow, ds;
  int k;

  xh = 0.5 * x;
  sum = 1.0;
  pow = 1.0;
  k = 0;
  ds = 1.0;
  while (ds > sum * EPS) {
    ++k;
    pow = pow * (xh / k);
    ds = pow * pow;
    sum = sum + ds;
  }

  return sum;
}
