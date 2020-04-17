/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  float **MAfAllocMat (int Nrow, int Ncol)

Purpose:
  Allocate a matrix of float values

Description:
  This routine allocates a matrix of float values.  The returned value is
  a pointer to an array of pointers to the rows of the matrix.  The calling
  routine can treat the returned value as a two-dimensional matrix of float
  values.  Consider the invocation
    float **A;
    int Nrow, Ncol;
    ...
    A = MAfAllocMat (Nrow, Ncol);
  Then A[i][j] is a matrix of float values for 0 <= i < Nrow and
  0 <= j < Ncol.  The routine MAfAllocMat allocates an array of Nrow pointers
  and a contiguous block of memory sufficient to hold the Nrow*Ncol float
  values.  Each pointer is initialized to point to a row of the matrix.  A[0]
  is a pointer to the first row (Ncol float values) (and to the full matrix of
  Nrow*Ncol float values).  A[1] points to the next row (Ncol elements beyond
  A[0]), and so on.

  The space allocated by this routine should be deallocated using the routine
  MAfFreeMat.

Parameters:
  <-  float **MAfAllocMat
      Returned pointer to array of row pointers
   -> int Nrow
      Number of rows for the matrix
   -> int Ncol
      Number of elements in each row (number of columns)

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.8 $  $Date: 2003/05/09 01:45:58 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


float **
MAfAllocMat (int Nrow, int Ncol)

{
  float **A;
  int i;

/* Allocate the row pointers */
  A = (float **) UTmalloc (Nrow * sizeof (float *));

/* Allocate the matrix of float values */
  A[0] = (float *) UTmalloc (Nrow * Ncol * sizeof (float));

/* Set up the pointers to the rows */
  for (i = 1; i < Nrow; ++i)
    A[i] = A[i-1] + Ncol;

  return A;
}
