/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void VRfPrint (FILE *fp, const char Header[], const float x[], int N)

Purpose:
  Print a float array along with a header text

Description:
  This routine prints the values of a float array along with an identifying
  header.

Parameters:
   -> FILE *fp
      File pointer for the output.  If fp is NULL, no printing occurs.
   -> const char Header[]
      Character string for the header.  The header line is omitted if the
      header string is empty.
   -> const float x[]
      Array of N floats
   -> int N
      Number of elements in x

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.10 $  $Date: 2003/05/09 03:33:10 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


void
VRfPrint (FILE *fp, const char Header[], const float x[], int N)

{
  MAfPrint (fp, Header, &x, 1, N);

  return;
}
