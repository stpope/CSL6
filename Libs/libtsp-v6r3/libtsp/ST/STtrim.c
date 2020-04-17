/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int STtrim (const char Si[], char So[])

Purpose:
  Copy a string, trimming leading and trailing white-space

Description:
  This routine copies characters from the input string to the output string.
  Leading white space characters and trailing white space characters are
  omitted.

Parameters:
  <-  int STtrim
      Number of characters in the output string
   -> const char Si[]
      Input character string
  <-  char So[]
      Output character string.  The output string pointer can be the same as
      the input string pointer.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.7 $  $Date: 2003/05/09 03:02:44 $

-------------------------------------------------------------------------*/

#include <ctype.h>
#include <string.h>

#include <libtsp.h>


int
STtrim (const char Si[], char So[])

{
  const char *q;
  int n, nc;

  /* Find the first non-white-space character */
  for (; isspace ((int) *Si); ++Si)
    ;

  /* Find the end of the string */
  for (q = Si; *q != '\0'; ++q)
    ;
  nc = q - Si;			/* q points to the null */

  /* Trim trailing white-space */
  for (; nc > 0; --nc) {	/* nc is the number of characters */
    if (! isspace ((int) *(--q)))
      break;
  }

  /* Copy the trimmed string to the output string */
  for (n = 0; n < nc; ++n)
    *So++ = *Si++;
  *So = '\0';			/* Add a trailing null */

  return nc;
}
