/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int FNiLog2 (int N)

Purpose:
  Find the integer greater than or equal to log (base 2) of a given value

Description:
  This function returns the number of bits needed to represent N-1, where N is
  a given integer value. This routine can be used to find a suitable FFT length
  (power of two) for a sequence of length N. The FFT length is 2^m, where m is
  the value returned by this routine. The returned value is smallest m
  satisfying
    2^m >= N.

Parameters:
  <-  int FNiLog2
      Returned integer value
   -> int N
      Input value (greater than zero)

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.7 $  $Date: 2003/05/09 01:41:29 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/FNmsg.h>


int
FNiLog2 (int N)

{
  int log2;
  int m;

  if (N <= 0)
    UThalt ("FNiLog2: %s", FNM_NonPosValue);

/* This routine successively divides the N-1 to determine the number of
   bits needed to represent it.  This requires log_2(N) iterations.  A more
   efficient routine would result if we used a table of powers of two and
   used a binary search in the table.  However, the size of the table is
   machine dependent.  That approach would require log_2 (log_2 (Max_Value))
   probes, say 5 for 32 bit integers, but each  probe is more complicated
   than the test in this loop.
*/
  log2 = 0;
  m = N - 1;
  while (m > 0) {
    ++log2;
    m = m / 2;
  }

  return log2;
}

