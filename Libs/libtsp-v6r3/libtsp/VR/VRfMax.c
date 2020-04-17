/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double VRfMax (const float x[], int N)

Purpose:
  Return the maximum value in a float array

Description:
  This routine finds the maximum of the values in a float array.

Parameters:
  <-  double VRfMax
      Maximum of the values in x
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
VRfMax (const float x[], int N)

{
  int i;
  float xmax;

  if (N <= 0)
    UThalt ("VRfMax: %s", VRM_BadN);

  xmax = x[0];
  for (i = 1; i < N; ++i) {
    if (x[i] > xmax)
      xmax = x[i];
  }

  return (double) xmax;
}
