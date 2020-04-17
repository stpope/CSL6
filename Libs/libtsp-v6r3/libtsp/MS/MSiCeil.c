/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int MSiCeil (int n, int m)

Purpose:
  Ceiling function for the ratio of two integers

Description:
  This routine calculates the ceiling function of the ratio n/m.  It returns
  the smallest integer that is greater than or equal to the quotient.

Parameters:
  <-  int MSiCeil
      Returned integer value
   -> int n
      Input value (numerator)
   -> int m
      Input value (denominator)

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.8 $  $Date: 2003/05/09 02:29:38 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


int
MSiCeil (int n, int m)

{
  int q;

/*
   For positive operands, the ceiling function is just (n-1)/m + 1.  However,
   if n and/or m is negative, the result of integer division may not truncate
   towards zero.  Consider
     q = (n-r)/m, where q = [n/m] (integer division) and r = n % m.
   The ceiling function is then either q or q+1, depending on the sign of r
   and m.
*/
  q = n / m;
  if (m > 0) {
    if (n <= m * q)
      return q;
    else
      return q + 1;
  }
  else {
    if (n >= m * q)
      return q;
    else
      return q + 1;
  }
}
