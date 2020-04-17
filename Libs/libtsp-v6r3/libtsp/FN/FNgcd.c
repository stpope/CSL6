/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  long int FNgcd (long int u, long int v)

Purpose:
  Find the greatest common divisor of two integer values

Description:
  This function finds the (positive) greatest common divisor of two integer
  values.  If both elements are zero, the greatest common divisor is set to
  zero.

Parameters:
  <-  long int FNgcd
      Greatest common divisor of u and v
   -> long int u
      First integer value
   -> long int v
      Second integer value

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.3 $  $Date: 2003/05/09 01:41:29 $

-------------------------------------------------------------------------*/

#include <libtsp.h>

#define ABSV(x)		(((x) < 0) ? -(x) : (x))


long int
FNgcd (long int u, long int v)

{
  long int r;

  u = ABSV (u);
  v = ABSV (v);

  while (v != 0) {
    r = u % v;
    u = v;
    v = r;
  }

  return u;
}
