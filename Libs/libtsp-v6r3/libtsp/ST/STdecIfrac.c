/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int STdecIfrac (const char String[], int *Ival1, int *Ival2)

Purpose:
  Decode a ratio specification of two integer values

Description:
  This routine decodes a string specifying a ratio of integer values.  The
  ratio is specified in the form "V" or "V1/V2", for example "-23 / 45".
  Optional white-space (as defined by isspace) can surround the values.  For
  the case of a single value V, this is equivalent to the ratio "V/1".  If an
  error is encountered, neither output value is set.

Parameters:
  <-  int STdecIfrac
      Error status, 0 for no error, 1 for error
   -> const char String[]
      Input string
  <-  int *Ival1
      First value
  <-  int *Ival2
      Second value

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.8 $  $Date: 2003/05/09 03:02:44 $

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
STdecIfrac (const char String[], int *Ival1, int *Ival2)

{
  int status;
  int ival1, ival2;

  /* Decode the ratio values */
  status = STdecPair (String, "/", 'I', (void *) (&ival1), (void *) (&ival2));

  if (status == DP_LVAL) {
    *Ival1 = ival1;
    *Ival2 = 1;
  }
  else if (status == (DP_LVAL + DP_DELIM + DP_RVAL)) {
    *Ival1 = ival1;
    *Ival2 = ival2;
  }
  else if (status >= 0) {
    UTwarn ("STdecIfrac - %s: \"%s\"", STM_DataErr, STstrDots (String, MAXC));
    status = -1;
  }

  return (status < 0);
}
