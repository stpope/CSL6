/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int STcopyMax (const char Si[], char So[], int Maxchar)

Purpose:
  Copy at most Maxchar characters to a string

Description:
  This routine copies characters from the input string to the output string.
  Characters are copied until a null is seen in the input string or the number
  of characters copied is Maxchar.  Then a trailing null character is appended
  to the output string.  If the input string is longer than Maxchar (not
  including the null character), a string truncated warning message is printed.

Parameters:
 <-   int STcopyMax
      Number of characters in the output string
   -> const char Si[]
      Input character string
  <-  char So[]
      Output character string.  This string is always null terminated, with
      at most Maxchar characters not including the terminating null character.
      If the input string is longer than Maxchar, only the first Maxchar
      characters are copied and a warning message is printed.
   -> int Maxchar
      Maximum number of characters (not including the trailing null character)
      to be placed in So.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.17 $  $Date: 2003/05/09 03:02:44 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/STmsg.h>

#define MINV(a, b)	(((a) < (b)) ? (a) : (b))


int
STcopyMax (const char Si[], char So[], int Maxchar)

{
  const char *si;
  int n;

  /* Save the initial input pointer */
  si = Si;

  /* Copy at most Maxchar characters */
  for (n = 0; n < Maxchar && *si != '\0'; n++)
    *So++ = *si++;

  /* Add a trailing null */
  *So = '\0';

  /* Check for truncation */
  if (*si != '\0')
    UTwarn ("STcopyMax - %s: \"%.*s...\"", STM_StrTrunc, MINV (30, n), Si);

  /* Return the number of characters in So */
  return n;
}
