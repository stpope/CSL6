/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void SPfTrMat2 (float A[], int N, int Ifn)

Purpose:
  In-place transposition of a 2 x N or N x 2 matrix

Description:
  This subroutine transposes a 2 by N/2 or a N/2 by 2 matrix in place.  This
  operation changes columnwise storage to rowwise storage or vice versa.  N
  must be an integer power of 2.

Parameters:
  <-> float A[]
      Array, either 2 by N/2 or N/2 by 2 depending on the value of Ifn
   -> int N
      The total number of elements in A.  N must be a integer power of 2.
   -> int Ifn
      Ifn > 0, the matrix A is considered to be A[N/2][2],
      Ifn < 0, the matrix A is considered to be A[2][N/2]

Author / revision:
  J. Costa / P. Kabal
  $Revision: 1.10 $  $Date: 2003/05/09 02:47:19 $

-------------------------------------------------------------------------*/

#include <libtsp/nucleus.h>

static void
SP_mt2x2 (float A[], int N1, int N2);


void
SPfTrMat2 (float A[], int N, int Ifn)

{
  int N2, N1;

  N2 = 2;
  while (N2 < N) {
    N1 = N / N2;
    if (Ifn < 0)
      SP_mt2x2 (A, N2, N1);
    else
      SP_mt2x2 (A, N1, N2);
    N2 = N2 + N2;
  }
}

/*
   SP_mt2x2:
   In-place transposition of 2 x 2 submatrices of a N1 by N2 matrix
*/

#define a(i,j)	A[(i) * N1 + (j)]


static void
SP_mt2x2 (float A[], int N1, int N2)

{
  int i, j;
  float t;

  for (i = 0; i < N2; i += 2) {
    for (j = 0; j < N1; j += 2) {
      t = a(i+1,j);
      a(i+1,j) = a(i,j+1);
      a(i,j+1) = t;
    }
  }
  return;
}
