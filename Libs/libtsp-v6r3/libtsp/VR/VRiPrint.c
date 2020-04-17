/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void VRiPrint (FILE *fp, const char Header[], const int x[], int N)

Purpose:
  Print an integer array along with a header text

Description:
  This routine prints the values of an int array along with an identifying
  header.

Parameters:
   -> FILE *fp
      File pointer for the output.  If fp is NULL, no printing occurs.
   -> const char Header[]
      Character string for the header.  The header line is omitted if the
      header string is empty.
   -> const int x[]
      Array of N ints
   -> int N
      Number of elements in x

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.5 $  $Date: 2003/05/09 03:33:11 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


void
VRiPrint (FILE *fp, const char Header[], const int x[], int N)

{
  MAiPrint (fp, Header, &x, 1, N);

  return;
}
