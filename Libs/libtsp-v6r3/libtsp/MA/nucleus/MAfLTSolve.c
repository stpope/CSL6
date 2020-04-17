/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void MAfLTSolve (const float *L[], const float b[], float x[], int N)

Purpose:
  Solve a lower triangular set of equations

Description:
  This routine uses backsubstitution to solve an upper triangular set of
  equations.  The equations solved are L x = b, where L is a lower triangular
  matrix.

  This routine requires:
        N       divides, and
    (N^2-N)/2   multiplies and adds.

Parameters:
   -> const float *L[]
      L is an array of pointers to the rows of an N by N matrix.  The matrix is
      assumed to be a lower triangular matrix.  Only the lower triangular
      portion of the matrix is accessed.
   -> const float b[]
      Right hand side vector (N elements)
  <-  float x[]
      Solution vector (N elements).  The vectors b and x can occupy the same
      storage locations.
   -> int N
      Size of the matrix and the vectors

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.12 $  $Date: 2003/05/09 01:47:19 $

-------------------------------------------------------------------------*/

#include <libtsp/nucleus.h>


void
MAfLTSolve (const float *L[], const float b[], float x[], int N)

{
  int i, k;
  double sum;

/*
   Solution by elimination ( L x = b)
                  i-1
   x(i) = [b(i) - SUM l(i,k) x(k)] / l(i,i)
                  k=0
*/

  for (i = 0; i < N; ++i) {
    sum = b[i];
    for (k = 0; k < i; ++k)
      sum = sum - L[i][k] * x[k];
    x[i] = (float) sum / L[i][i];
  }
  return;
}
