/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int MSiFloor (int n, int m)

Purpose:
  Floor function for the ratio of two integers

Description:
  This routine calculates the floor function of the ratio n/m.  It returns the
  smallest integer that is less than or equal to the quotient.

Parameters:
  <-  int MSiFloor
      Returned integer value
   -> int n
      Input value (numerator)
   -> int m
      Input value (denominator)

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.5 $  $Date: 2003/05/09 02:29:38 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


int
MSiFloor (int n, int m)

{
  int q;

/*
   For positive operands, the floor function is just n/m.  However, if n and/or
   m is negative, the result of integer division may not truncate towards zero.
   Consider
     q = (n-r)/m, where q = [n/m] (integer division) and r = n % m.
   The floor function is then either q or q-1, depending on the sign of r and
   m.
*/
  q = n / m;
  if (m > 0) {
    if (n >= m * q)
      return q;
    else
      return q - 1;
  }
  else {
    if (n <= m * q)
      return q;
    else
      return q - 1;
  }
}
