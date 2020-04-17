/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void VRlPrint (FILE *fp, const char Header[], const long int x[], int N)

Purpose:
  Print a long integer array along with a header text

Description:
  This routine prints the values of a long int array along with an identifying
  header.

Parameters:
   -> FILE *fp
      File pointer for the output.  If fp is NULL, no printing occurs.
   -> const char Header[]
      Character string for the header.  The header line is omitted if the
      header string is empty.
   -> const long int x[]
      Array of N long ints
   -> int N
      Number of elements in x

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.3 $  $Date: 2003/05/09 03:33:11 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


void
VRlPrint (FILE *fp, const char Header[], const long int x[], int N)

{
  MAlPrint (fp, Header, &x, 1, N);

  return;
}
