/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int STdec1float (const char String[], float *Fval)

Purpose:
  Decode a float value

Description:
  This routine decodes a string containing a numeric value.  The decoded datum
  is stored as a float value.  If a decoding error is detected, a warning
  message is printed and error status is set.  A warning messages is printed
  if extra data follows the requested value (this data is ignored).

Parameters:
  <-  int STdec1float
      Error status,
        0 - no error
        1 - error, too few values or data format error
	2 - warning, data values too large or too small
   -> const char String[]
      Input string
  <-  float *Fval
      Returned value.  This value is not changed if an error status is
      returned.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.12 $  $Date: 2003/05/09 03:02:44 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/nucleus.h>


int
STdec1float (const char String[], float *Fval)

{
  return (STdec1val (String, 'F', (void *) Fval));
}
