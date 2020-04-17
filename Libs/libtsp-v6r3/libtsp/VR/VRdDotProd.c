/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double VRdDotProd (const double x1[], const double x2[], int N)

Purpose:
  Dot product of two double arrays

Description:
  This routine calculates the dot product of two double arrays x1 and x2.
                 N-1
    VRfDotProd = SUM x1[i] * x2[i] .
                 i=0

Parameters:
  <-  double VRfDotProd
      Dot product
   -> const double x1[]
      Input array (N elements)
   -> const double x2[]
      Input array (N elements)
   -> int N
      Number of elements in the arrays (may be zero)

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.2 $  $Date: 2003/05/09 03:33:10 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


double
VRdDotProd (const double x1[], const double x2[], int N)

{
  int i;
  double sum;

  sum = 0.0;
  for (i = 0; i < N; ++i)
    sum += x1[i] * x2[i];

  return sum;
}
