/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int STcopyNMax (const char Si[], char So[], int N, int Maxchar)

Purpose:
  Copy N characters characters to a string

Description:
  This routine is used to copy min(N, strlen(Si)) characters from the input
  input string to the output string.  If min(N, strlen(Si) is greater than
  Maxchar, a string is truncated at that point and a warning message is
  printed.

Parameters:
 <-   int STcopyNMax
      Number of characters in the output string
   -> const char Si[]
      Input character string
  <-  char So[]
      Output character string.  This string is always null terminated, with
      at most Maxchar characters not including the terminating null character.
   -> int N
      Number of characters to be transferred
   -> int Maxchar
      Maximum number of characters (not including the terminating null
      character) to be placed in So.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.20 $  $Date: 2003/05/09 03:02:44 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/STmsg.h>

#define MINV(a, b)	(((a) < (b)) ? (a) : (b))


int
STcopyNMax (const char Si[], char So[], int N, int Maxchar)

{
  const char *si;
  int n;
  int NM;

  /* Save the initial input pointer */
  si = Si;

  /* Copy at most max(N, Maxchar) characters */
  NM = MINV (Maxchar, N);
  for (n = 0; n < NM && *si != '\0'; n++)
    *So++ = *si++;

  /* Add a trailing null */
  *So = '\0';

  /* Check for truncation */
  if (N > Maxchar && n == NM && Si[NM-1] != '\0')
    UTwarn ("STcopyNMax - %s: \"%.*s...\"", STM_StrTrunc, MINV (30, n), Si);

  /* Return the number of characters in So */
  return n;
}
