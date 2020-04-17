/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double VRfCorSym (const float x[], int N)

Purpose:
  Find the correlation coefficient for a vector and its reversal

Description:
  This procedure returns a value which indicates whether a given array of
  values is symmetric or anti-symmetric.  The correlation coefficient for
  the vectors x[i] and x[N-1-i] is calculated as
          SUM x[i] x[N-1-i]
    rho = ----------------- .
              SUM x[i]^2

  An alternate interpretation is in terms of the even (symmetric) and odd
  (anti-symmetric) parts of a vector,
    xe[i] = 0.5 (x[n] + x[N-1-i]),
    xo[i] = 0.5 (x[n] - x[N-1-i]).
  Then
          SUM xe[i]^2 - SUM xo[i]^2
    rho = ------------------------- .
          SUM xe[i]^2 + SUM xo[i]^2
  If the vector is symmetric (x[i] = x[N-1-i]), rho is +1.  If the vector
  is anti-symmetric (x[i] = -x[N-1-i]), the result is -1.  All other cases
  fall in between.

  A test for near symmetry or near anti-symmetry is suggested by
                SUM xo[i]^2     SUM xe[i]^2
    rho = 1 - 2 ----------- = 2 ----------- - 1.
                SUM x[i]^2      SUM x[i]^2
  Declare the vector to be near-symmetric if rho >= 1 - 2 eps, where eps is
  the relative energy in the odd part of the vector. Similarly, declare the
  vector to be near-anti-symmetric if rho <= -(1 - 2 eps), where eps is the
  relative energy in the even part of the vector.  Choosing a small value for
  eps, a suitable test for near symmetry or near anti-symmetry is
     (int) (1./(1.-2.*eps) * rho).
  This returns +1 for a near symmetric vector, -1 for a near anti-symmetric
  vector and 0 otherwise.

Parameters:
  <-  double VRfCorSym
      Correlation coefficient in the range -1 to +1.  If the vector has all
      zero coefficients, the value 0 is returned.
   -> const float x[]
      vector of doubles (N elements)
   -> int N
      Number of elements in the input array

Author / revision:
  P. Kabal  Copyright (C) 2004
  $Revision: 1.4 $  $Date: 2004/06/10 02:54:54 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


double
VRfCorSym (const float x[], int N)

{
  int i, j;
  double Sn, Sd, rho;

/*
  For two vectors x[i] and y[i], the correlation coefficient is
                SUM x[i] y[i]
    rho = --------------------------- .
          sqrt(SUM x[i]^2 SUM y[i]^2)
  
  The correlation takes on values from -1 to +1.  In our case, the vector
  y[i] is the reversal of x[i].  The two sums in the denominator are equal
  and so the square root can be eliminated.
*/
  Sn = 0.0;
  Sd = 0.0;
  for (i = 0, j = N-1; i < j; ++i, --j) {
    Sn += ((double) x[i] * x[j] + (double) x[i] * x[j]);
    Sd += ((double) x[i] * x[i] + (double) x[j] * x[j]);
  }
  if (i == j) {
    Sn += (double) x[i] * x[j];
    Sd += (double) x[i] * x[j];
  }

  if (Sd != 0.0)
    rho = Sn/Sd;
  else
    rho = 0.0;

  return rho;
}
