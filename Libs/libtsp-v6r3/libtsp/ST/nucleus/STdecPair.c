/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int STdecPair (const char String[], const char Delim[], int Type,
                 void *Val1, void *Val2)

Purpose:
  Decode a data specification for a pair of numeric values

Description:
  This routine decodes a string specifying a pair of integer values.  The pair
  is specified as "V", "V1:", ":V2", or "V1:V2", for example "-23 : 45".  The
  delimiter is shown as ":" in the example, but a general delimiter string can
  be specified in the argument list.  Optional white-space (as defined by
  isspace) can surround the values.

Parameters:
  <-  int STdecPair
      Return code, negative for errors.
      0 - Empty string
      1 - Left value (no delimiter)
      2 - Delimiter alone
      3 - Left value + delimiter
      6 - Delimiter + right value
      7 - Left value + delimiter + right value
   -> const char String[]
      Input string
   -> const char Delim[];
      Delimiter string.  This value is used in STfindToken to separate the
      two values.
   -> int Type
      Code for the data type, 'D' for double, 'F' for float, 'I' for int, 'L'
      for long int.
  <-  void *Val1
      First value (may be unchanged)
  <-  void *Val2
      Second value (may be unchanged)

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.9 $  $Date: 2003/05/09 03:06:42 $

-------------------------------------------------------------------------*/

#include <string.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>

#define MAXC	80
#define DP_EMPTY		0
#define DP_LVAL			1
#define DP_DELIM		2
#define DP_RVAL			4	


int
STdecPair (const char String[], const char Delim[], int Type, void *Val1,
	   void *Val2)

{
  char *token;
  int nc;
  const char *p;
  int status;
  char cbuf[MAXC+1];

/* Allocate temporary string space */
  nc = strlen (String);
  if (nc > MAXC)
    token = (char *) UTmalloc (nc + 1);
  else
    token = cbuf;

/* Check for an empty string */
  if (*STtrimIws (String) == '\0')
    return (DP_EMPTY);

/* Decode the first value of the pair */
  p = STfindToken (String, Delim, "", token, 1, nc);

/* Notes:
  p == NULL, indicates that there was no delimiter.
    - decode token (if it is of zero length, STdec1val will print a message)
  p != NULL, delimiter present
    - token zero length, skip the first value
    - second value is p, if this is zero length, skip this value
*/
  if (p == NULL) {
    status = DP_LVAL;
    if (STdec1val (token, Type, Val1))		/* No delimiter */
      status = -1;
  }
  else {					/* Delimiter */
    status = DP_DELIM;
    if (token[0] != '\0') {
      status += DP_LVAL;			/* Left value present */
      if (STdec1val (token, Type, Val1))
	status = -1;
    }

    /* Decode the second value of the pair */
    if (status >= 0 && *p != '\0') {
      status += DP_RVAL;			/* Right value present */
      if (STdec1val (p, Type, Val2))
	status = -2;
    }
  }
    
  if (token != cbuf)
    UTfree ((void *) token);

  return status;
}
