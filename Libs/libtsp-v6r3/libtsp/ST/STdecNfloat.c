/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int STdecNfloat (const char String[], int Nmin, int Nmax, float Fval[],
		   int *N)

Purpose:
  Decode float values (variable number)

Description:
  This routine decodes a string containing numeric data.  Multiple data items
  data items in the string are separated by commas or white-space (as defined
  by the isspace routine).  The decoded data is stored as float values.  If
  the number of data values in the string is less than a given minimum number
  or a decoding error is detected, an error message is printed and an error
  indication is returned.  A warning messages is printed if extra data follows
  the requested values (this data is ignored).

Parameters:
  <-  int STdecNfloat
      Error status,
        0 - no error
        1 - error, too few values or data format error
	2 - warning, data values too large or too small
   -> const char String[]
      Input string
   -> int Nmin
      Minimum number of values to be read (may be zero)
   -> int Nmax
      Maximum number of values to be read
  <-  float Fval[]
      Array of Nmax elements used to store the decoded values.  Only the first
      N values are modified.
  <-  int *N
      Actual number of values decoded.  In the case of an error, N indicates
      the number of values successfully decoded.  In that case, N may be less
      than Nmin.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.22 $  $Date: 2003/05/09 03:02:44 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/nucleus.h>


int
STdecNfloat (const char String[], int Nmin, int Nmax, float Fval[], int *N)

{
  return (STdecNval (String, Nmin, Nmax, 'F', (void *) Fval, N));
}
