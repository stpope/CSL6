/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int STunQuote (const char Si[], const char Quotes[], char So[])

Purpose:
  Copy a string, removing outer quote characters

Description:
  This routine processes a quoted string.  Any outer paired quotes are removed.

Parameters:
  <-  int STunQuote
      Number of characters in the output string
   -> const char Si[]
      Input character string
   -> const char Quotes[]
      Character string specifying pairs of quote characters (the left and right
      quote characters).  A zero length string indicates that quote characters
      should not to be recognized.
  <-  char So[]
      Output character string.  This string should provide for at least as many
      characters as the input string.  This routine accepts Si = So, in which
      case the output string overlays the input string.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.4 $  $Date: 2003/05/09 03:02:44 $

-------------------------------------------------------------------------*/

#include <string.h>

#include <libtsp.h>


int
STunQuote (const char Si[], const char Quotes[], char So[])

{
  int n, nc;
  const char *p;

  /* Count the number of characters */
  nc = strlen (Si);

  /* Check for a leading and trailing quote */
  for (p = Quotes; *p != '\0' && *(p+1) != '\0'; ++p, ++p) {
    if (*p == Si[0] && *(p+1) == Si[nc-1]) {
      nc -= 2;
      ++Si;
      break;
    }
  }

  /* Copy characters */
  for (n = 0; n < nc; ++n)
    *So++ = *Si++;

  /* Add a trailing null */
  *So = '\0';

  /* Return the number of characters in So */
  return nc;
}
