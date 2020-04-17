/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int STtrimNMax (const char Si[], char So[], int N, int Maxchar)

Purpose:
  Copy at most N characters, trimming trailing white-space

Description:
  This routine copies characters from the input string to the output string.
  The length of the input string is considered to be equal to N less any
  trailing white-space (as defined by isspace).  Characters are copied from
  input string up to the minimum of the length of the input string and Maxchar.
  A null character in the input string also terminates the transfer at that
  point.  If the input string is longer than Maxchar, a string truncated
  warning message is printed.

Parameters:
  <-  int STtrimNMax
      Number of characters in the output string
   -> const char Si[]
      Input character string
  <-  char So[]
      Output character string.  This string is always null terminated, with
      at most Maxchar characters not including the terminating null character.
      If the input string is longer than Maxchar, only the first Maxchar
      characters are copied and a warning message is printed.
   -> int N
      Number of characters to be transferred
   -> int Maxchar
      Maximum number of characters (not including the trailing null character)
      to be placed in So.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.13 $  $Date: 2003/05/09 03:06:42 $

-------------------------------------------------------------------------*/

#include <ctype.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>

int
STtrimNMax (const char Si[], char So[], int N, int Maxchar)

{
  const char *p;
  int n, nc;

  /* Determine the length of the input string */
  p = Si;
  for (n = 0; n < N; ++n, ++p)
    if (*p == '\0')
      break;

  /* Trim trailing white-space */
  for (p = Si+(n-1); n > 0 ; --n, --p)	/* n is the number of characters */
    if (! isspace ((int) *p))
      break;

  /* Copy the trimmed string to the output string */
  nc = STcopyNMax (Si, So, n, Maxchar);
  return nc;
}
