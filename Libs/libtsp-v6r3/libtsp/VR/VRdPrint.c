/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void VRdPrint (FILE *fp, const char Header[], const double x[], int N)

Purpose:
  Print a double array along with a header text

Description:
  This routine prints the values of a double array along with an identifying
  header.

Parameters:
   -> FILE fp*
      File pointer for the output.  If fp is NULL, no printing occurs.
   -> const char Header[]
      Character string for the header.  The header line is omitted if the
      header string is empty.
   -> const double x[]
      Array of N doubles
   -> int N
      Number of elements in x

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.8 $  $Date: 2003/05/09 03:33:10 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


void
VRdPrint (FILE *fp, const char Header[], const double x[], int N)

{
  MAdPrint (fp, Header, &x, 1, N);

  return;
}
