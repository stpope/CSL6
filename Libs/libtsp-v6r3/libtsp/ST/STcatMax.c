/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int STcatMax (const char Si[], char So[], int Maxchar)

Purpose:
  Append characters to a string (maximum length specified)

Description:
  This routine appends characters from the input string to the end of the
  second string.  Characters are appended until a null is seen in the input
  string or the number of characters in the output string is Maxchar.  A
  trailing null character is appended to the output string.  If not all
  characters from the input string can be copied into the output string, a
  string truncated warning message is printed.

Parameters:
  <-  int STcatMax
      Number of characters in the output string
   -> const char Si[]
      Input character string to be appended to So
  <-> char So[]
      Output character string.  This string is always null terminated, with at
      most Maxchar characters not including the terminating null character.  If
      the sum of the input string lengths is more than Maxchar, only the first
      Maxchar characters appear in So and a warning message is printed.
   -> int Maxchar
      Maximum number of characters (not including the trailing null character)
      to appear in So.  Note that if Si is initially longer than Maxchar, it is
      truncated to this length.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.23 $  $Date: 2003/05/09 03:02:43 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/STmsg.h>

#define MINV(a, b)	(((a) < (b)) ? (a) : (b))


int
STcatMax (const char Si[], char So[], int Maxchar)

{
  char *so;
  int n, trunc;

  /* Save the initial output pointer */
  so = So;

  /* Find the end of So */
  for (n = 0; n < Maxchar && *so != '\0'; ++n)
    so++;
  trunc = (*so != '\0');

  /* Copy to So */
  for (; n < Maxchar && *Si != '\0'; ++n)
    *so++ = *Si++;

/* Add a trailing null */
  *so = '\0';

  /* Check for truncation */
  if (*Si != '\0' || trunc )
    UTwarn ("STcatMax - %s: \"%.*s...\"", STM_StrTrunc, MINV (30, n), So);

  /* Return the number of characters in So */
  return n;
}
