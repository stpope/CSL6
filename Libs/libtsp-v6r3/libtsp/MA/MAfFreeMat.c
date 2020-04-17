/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void MAfFreeMat (float *A[])

Purpose:
  Free an allocated float matrix

Description:
  This routine frees the space occupied by a float matrix.  This routine frees
  up space allocated by routine MAfAllocMat.

Parameters:
   -> float *A[]
      Pointer to an array of row pointers.  If A is NULL, no action is taken.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.10 $  $Date: 2003/05/09 01:45:58 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


void
MAfFreeMat (float *A[])

{
  if (A != NULL) {
    UTfree ((void *) A[0]);
    UTfree ((void *) A);
  }
  return;
}
