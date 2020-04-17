/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  char *STstrDots (const char Si[], int Maxchar)

Purpose:
  Generate a text with at most Maxchar characters (terminated with ...)

Description:
  This routine copies an input string to an output string.  The output string
  is at most Maxchar characters long.  If the input string is longer than
  Maxchar characters long, the algorithm below tries to truncate the line at
  a blank and add "..." to the output string.  Any non-printable characters
  are translated to "?".  The intended purpose of this routine is to format
  error messages.

  Let the length of the input string be N.  The maximum length of the output
  string is 80; let M = min(80,Maxchar).
  1: N <= M
    The output string is equal to the input string.
  2: N > M, M >= 20
    Look for a blank at positions [M-10, M-4] in the input string.  Call the
    position of the blank i.  Copy the first i+1 characters (includes the
    blank) of the input string to the output string.  If no blank is found,
    copy M-3 characters to the output string.  Append "..." to the output
    string.
  3: N > M, 6 <= M < 20
    Copy the first M-3 characters of the input string to the output string.
    Append "..." to the output string.
  4: N > M, M < 6
    Copy the first M characters of the input string to the output string.

  Examples:
      input : "It was the best of times, it was the worst"
               012345678901234567890123456789012345678901
               0         10       20        30        40 
        M = 60
      output: "It was the best of times, it was the worst"
        M = 40
      output: "It was the best of times, it was the ..."
        M = 20
      output: "It was the best ..."

      input : "Address http://www.TSP.EE.McGill.CA/software.html"
        M = 30
      output: "Address http://www.TSP.EE.M..."

Parameters:
 <-   char *STstrDots
      Output string.  This string is in an internally allocated area and is
      overwritten each time this routine is called.
   -> const char Si[]
      Input character string
   -> int Maxchar
      Maximum number of characters (not including the trailing null character)
      for the output string.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.5 $  $Date: 2003/05/09 03:06:42 $

-------------------------------------------------------------------------*/

#include <ctype.h>
#include <string.h>

#include <libtsp/nucleus.h>

#define MINV(a, b)	(((a) < (b)) ? (a) : (b))
#define MAXM		80
#define MINM		20

static int
ST_copyNtr (const char Si[], char So[], int N);


char *
STstrDots (const char Si[], int Maxchar)

{
  int nc, i, N, M;
  static char Line[MAXM+1];

  N = strlen (Si);
  M = MINV (MAXM, Maxchar);

  if (N <= M) {
    /* No truncation */
    M = N;
    nc = N;
  }
  else if (M >= MINM) {
    nc = M - 3;
    for (i = M-4; i >= M-10; --i) {
      if (Si[i] == ' ') {
	nc = i + 1;
	break;
      }
    }
  }
  else if (M >= 6)
    nc = M-3;
  else
    nc = M;

/* Copy characters */
  ST_copyNtr (Si, Line, nc);
  if (nc <= M-3)
    strcpy (&Line[nc], "...");

  return Line;
}

/* Copy N characters, translating non-printing characters to '?' */


static int
ST_copyNtr (const char Si[], char So[], int N)

{
  int i;

  for (i = 0; i < N; ++i) {
    if (isprint ((int) Si[i]))
      So[i] = Si[i];
    else
      So[i] = '?';
  }
  So[i] = '\0';

  return i;
}
  
