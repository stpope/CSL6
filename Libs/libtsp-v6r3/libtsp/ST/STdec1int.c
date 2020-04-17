/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int STdec1int (const char String[], int *Ival)

Purpose:
  Decode an integer value

Description:
  This routine decodes a string containing a numeric value.  The decoded datum
  is stored as an integer value.  If a decoding error is detected, a warning
  message is printed and an error status value is returned.  A warning message
  is printed if extra data follows the requested value (this data is ignored).

Parameters:
  <-  int STdec1int
      Error status,
        0 - no error
        1 - error, too few values or data format error
	2 - warning, data values too large or too small
   -> const char String[]
      Input string
  <-  int *Ival
      Returned value.  This value is not changed if an error status is
      returned.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.11 $  $Date: 2003/05/09 03:02:44 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/nucleus.h>


int
STdec1int (const char String[], int *Ival)

{
  return (STdec1val (String, 'I', (void *) Ival));
}
