/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double VRfMin (const float x[], int N)

Purpose:
  Return the minimum value in a float array

Description:
  This routine finds the minimum of the values in a float array.

Parameters:
  <-  double VRfMin
      Minimum of the values in x
   -> const float x[]
      Input array (N elements)
   -> int N
      Number of elements in the array (must be greater than zero)

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.7 $  $Date: 2003/05/09 03:33:10 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/VRmsg.h>


double
VRfMin (const float x[], int N)

{
  int i;
  float xmin;

  if (N <= 0)
    UThalt ("VRfMin: %s", VRM_BadN);

  xmin = x[0];
  for (i = 1; i < N; ++i) {
    if (x[i] < xmin)
      xmin = x[i];
  }

  return (double) xmin;
}
