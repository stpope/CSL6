/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int STdecLrange (const char String[], long int *Lval1, long int *Lval2)

Purpose:
  Decode a range specification for long integer values

Description:
  This routine decodes a string specifying a range of long integer values.  The
  range is specified in the form "Lv" or "Lv1:Lv2", for example "-23 : 45".
  Optional white-space (as defined by isspace) can surround the values.  For
  the case of a single value Lv, this is equivalent to the range "Lv:Lv".  If
  an error is encountered, neither output value is set.

Parameters:
  <-  int STdecIrange
      Error status, 0 for no error, 1 for error
   -> const char String[]
      Input string
  <-  long int *Lval1
      First value
  <-  long int *Lval2
      Second value

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.7 $  $Date: 2003/05/09 03:02:44 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/STmsg.h>

#define MAXC	23
#define DP_EMPTY		0
#define DP_LVAL			1
#define DP_DELIM		2
#define DP_RVAL			4	


int
STdecLrange (const char String[], long int *Lval1, long int *Lval2)

{
  int status;
  long int lval1, lval2;

  /* Decode the range values */
  status = STdecPair (String, ":", 'L', (void *) (&lval1), (void *) (&lval2));

  if (status == DP_LVAL) {
    *Lval1 = lval1;
    *Lval2 = lval1;
  }
  else if (status == (DP_LVAL + DP_DELIM + DP_RVAL)) {
    *Lval1 = lval1;
    *Lval2 = lval2;
  }
  else if (status >= 0) {
    UTwarn ("STdecLrange - %s: \"%s\"", STM_DataErr, STstrDots (String, MAXC));
    status = -1;
  }

  return (status < 0);
}
