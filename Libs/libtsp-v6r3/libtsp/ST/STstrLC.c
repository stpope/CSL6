/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int STstrLC (const char Si[], char So[])

Purpose:
  Copy a string, changing uppercase characters to lowercase

Description:
  This routine changes uppercase characters to lowercase.

Parameters:
 <-   int STstrLC
      Number of characters in the output string
   -> const char Si[]
      Input character string
  <-  char So[]
      Output character string.  This string should provide for at least as many
      characters as the input string.  This routine accepts Si = So, in which
      case the output string overlays the input string.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.2 $  $Date: 2003/05/09 03:02:44 $

-------------------------------------------------------------------------*/

#include <ctype.h>

#include <libtsp.h>


int
STstrLC (const char Si[], char So[])

{
  int nc;

  for (nc = 0; Si[nc] != '\0'; ++nc)
    So[nc] = tolower ((unsigned const char) Si[nc]);

  So[nc] = '\0';

  return nc;
}
