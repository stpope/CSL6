/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int MSdLocate (double x, const double T[], int N)

Purpose:
  Binary search in a table

Description:
  This function searches a table for the interval which contains a given input
  value.  Index i indicates that the value lies between T[i-1] and T[i].  A
  binary search is used to determine the interval.

  The region index takes on values from 0 to N.  If N is equal to zero, the
  region index is set to zero.  Otherwise, the index is determined as follows.
  Note that the number of regions is one larger than the number of table
  values.
   index         input value
     0                 x < T[0]
     1         T[0] <= x < T[1]
     2         T[1] <= x < T[2]
    ...              ...
     i       T[i-1] <= x < T[i]
    ...              ...
    N-1      T[N-2] <= x < T[N-1]
     N       T[N-1] <= x

Parameters:
  <-  int MSdLocate
      Output region index of the quantizer region containing the input value x.
      MSdLocate takes on values from 0 to N.
   -> double x
      Value to be quantized
   -> const double T[]
      Table of N values which delimit the N regions.  These levels must
      be in ascending order.
   -> int N
      Number of table values

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.2 $  $Date: 2003/05/09 02:29:38 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


int
MSdLocate (double x, const double T[], int N)

{
  int iL, iU, i;

/* Binary search for the interval (T[iL], T[iU]] which brackets x */
  iL = 0;
  iU = N+1;

  /* Isolate the interval */
  while (iU > iL + 1) {
    i = (iL + iU) / 2;
    if (x < T[i-1])
      iU = i;
    else
      iL = i;
  }

/* Return the index */
  return iL;
}
