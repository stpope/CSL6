/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  char *STstrstrNM (const char Si[], const char Ti[], int N, int M)

Purpose:
  Search for the first occurrence of string Ti in string Si

Description:
  This routine searches for the first occurrence of string Ti in the input
  string Si.  The string Si is of length N and the string Ti is of length M.
  Null characters in either string are treated as ordinary characters.

Parameters:
   -> char Si[]
      Input character string of length N
   -> char Ti[]
      Input character string of length M
   -> int N
      Number of characters in Si
   -> int M
      Number of characters in Ti

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.13 $  $Date: 2003/05/09 03:06:42 $

-------------------------------------------------------------------------*/

#include <libtsp/nucleus.h>


char *
STstrstrNM (const char Si[], const char Ti[], int N, int M)

{
  const char *ss;
  const char *tt;
  int i;
  int k;
  int NM;

/* Null search string matches the first character */
  if (M == 0)
    return (char *) Si;

/* Outer loop searches for a match to the first character of Ti */
  NM = N - M;
  for (i = 0; i <= NM; ++i, ++Si) {

    if (*Si == *Ti) {
      /* Inner loop checks the remaining characters in Ti */
      ss = Si;
      tt = Ti;
      for (k = 1; k < M; ++k) {
	if (*++ss != *++tt)
	  break;
      }
      if (k == M)
	return (char *) Si;
    }

  }
  return NULL;
}
