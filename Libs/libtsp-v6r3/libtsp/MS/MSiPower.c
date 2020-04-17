/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int MSiPower (int a, int n)

Purpose:
  Integer raised to an integer power

Description:
  This routine calculates a raised to the n'th power.  Zero raised to the
  zero'th power is returned as one.

Parameters:
  <-  int MSiPower
      Returned integer value
   -> int a
      Input value
   -> int n
      Integer exponent

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.4 $  $Date: 2003/05/09 02:29:38 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


int
MSiPower (int a, int n)

{
  int value;

  if (n <= 0) {

/* Non-positive exponent */
    if (n == 0)
      /* Zero exponent */
      return 1;

    /* Negative exponent */
    if (a == 1)
      return 1;
    if (a == 0)
      return (1 / a);	/* Force a divide by zero */
    if (a != -1)
      return 0;

    /* a = -1, negate n */
    n = -n;
    if (n % 2 == 0)
      return 1;
    else
      return -1;
  }

/* Binary decomposition of the exponent */
  else {
    value = 1;
    while (n > 0) {
      if (n % 2)
	value = value * a;
      a = a * a;
      n = n / 2;
    }
    return value;
  }
}
