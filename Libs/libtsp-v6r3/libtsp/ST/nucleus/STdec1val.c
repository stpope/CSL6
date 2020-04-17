/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int STdec1val (const char String[], int Type, void *Val)

Purpose:
  Decode a numeric value

Description:
  This routine decodes a string containing a numeric value.  The data type can
  be specified.  If a decoding error is detected, a warning message is printed
  and an error status value is returned.

Parameters:
  <-  int STdec1val
      Error status,
        0 - no error
        1 - format error
        2 - range warning
   -> const char String[]
      Input string
   -> int Type
      Code for the data type, 'D' for double, 'F' for float, 'I' for int, 'L'
      for long int.
  <-  void *Val
      Returned value of the type indicated by Type.  This value is not changed
      for a format error.  For a range warning, the returned value is set to
      the appropriate representable value.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.22 $  $Date: 2003/05/09 03:17:00 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <stdlib.h>	/* strtod */


#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/STmsg.h>

#define ABSV(x)		(((x) < 0) ? -(x) : (x))

#define MAXC	23


int
STdec1val (const char String[], int Type, void *Val)

{
  int BS;
  double dv;
  long int lv;
  char *endstr;

  /* Trim initial white space */
  for (; isspace ((int) *String); ++String)
    ;
  if (String[0] == '\0') {
    UTwarn ("STdec1val - %s", STM_EmptyData);
    return 1;
  }

  /* Note: strtol should clamp a large input to INT_MAX, but the SunOS 4
     version wraps the value around instead, with no ERANGE errcode. (This
     is documented in the man page: "Overflow conditions are ignored.")
  */

  dv = 0.0;	/* stop compiler warnings */
  lv = 0;
  errno = 0;
  BS = 0;
  switch (Type) {
  case 'D':
  case 'F':
    dv = strtod (String, &endstr);
    if (errno == ERANGE) {
      if (dv == 0.0)
	BS = -1;
      else
	BS = 1;
    }
    break;
  case 'L':
  case 'I':
    lv = strtol (String, &endstr, 10);
    if (errno == ERANGE)
      BS = 1;
    break;
  default:
    assert (0);
    break;
  }

  /* Trim white space on endstr */
  for (; isspace ((int) *endstr); ++endstr)
    ;
  if (endstr[0] != '\0') {
    /* Data format error */
    UTwarn ("STdec1val - %s: \"%s\"", STM_DataErr, STstrDots (String, MAXC));
    return 1;
  }

  /* Return a value of the correct type */
  switch (Type) {
  case 'D':
    *((double *) Val) = dv;
    break;
  case 'F':
    if (dv > FLT_MAX) {
      dv = FLT_MAX;
      BS = 1;
    }
    else if (dv < -FLT_MAX) {
      dv = -FLT_MAX;
      BS = 1;
    }
    else if (dv != 0.0 && ABSV (dv) < FLT_MIN) {
      dv = 0.0;
      BS = -1;
    }
    *((float *) Val) = (float) dv;
    break;
  case 'L':
    *((long int *) Val) = lv;
    break;
  case 'I':
#if (INT_MAX != LONG_MAX)
    if (lv < INT_MIN) {
      lv = INT_MIN;
      BS = +1;
    }
    else if (lv > INT_MAX) {
      lv = INT_MAX;
      BS = +1;
    }
#else
    *((int *) Val) = lv;
#endif
    break;
  }

  if (BS < 0)
    UTwarn ("STdec1val - %s: \"%s\"", STM_SmallVal, STstrDots (String, MAXC));
  else if (BS > 0)
    UTwarn ("STdec1val - %s: \"%s\"", STM_BigVal, STstrDots (String, MAXC));

/* Return the status value */
  if (BS != 0)
    return 2;
  else
    return 0;
}
