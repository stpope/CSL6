/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double MAfTpQuad (const float R[], const float x[], int N)

Purpose:
  Calculate a quadratic form for a symmetric Toeplitz matrix

Description:
  This routine calculates a quadratic form S = x'R x, where x is a vector and
  R is a symmetric Toeplitz matrix,
        N-1 N-1
    S = SUM SUM x(i) R(i,j) x(j) .
        i=0 j=0
  The elements of the matrix R are constant down diagonals, R(i,j) = R(|i-h|).
  In this routine R is specified by its first column or row.  The result is
  accumulated as a double value and returned as a double value.

Parameters:
  <-  double MAfTpQuad
      Resultant value
   -> const float R[]
      First column (or row) of the symmetric Toeplitz matrix (N values)
   -> const float x[]
      Vector of N values
   -> int N
      Number of elements in x and R

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.3 $  $Date: 2003/05/09 01:45:58 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


double
MAfTpQuad (const float R[], const float x[], int N)

{
  int k;
  double sum0, sumN;

/*
  Let g(i,j) = x(i) R(i,j) x(j).  The double sum defining the quadratic form
  can be calculated down the main diagonal, then down minor diagonals.

        N-1 N-1          N-1          N-1 N-1
    S = SUM SUM g(i,j) = SUM g(l,l) + SUM SUM [g(l,l-k) + g(l-k,l)]
        i=0 j=0          l=0          k=1 l=k

  From symmetry,

    g(l,l-k) = g(l-k,l) = x(l) R(l,l-k) x(l-k) = x(l) R[k] x(l-k) .

  Then the sum can be written as

             N-1               N-1      N-1
    S = R[0] SUM x(l) x(l) + 2 SUM R[k] SUM x(l) x(l-k) .
             l=0               k=1      l=k
*/

/* Main diagonal */
  sum0 = R[0] * VRfDotProd (x, x, N);

/* Minor diagonals */
  sumN = 0.0;
  for (k = 1; k < N; ++k)
    sumN += R[k] * VRfDotProd (&x[k], &x[0], N-k);

  return (sum0 + 2.0 * sumN);
}
