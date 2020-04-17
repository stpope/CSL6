/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int STkeyXpar (const char Line[], const char Delims[], const char Quotes[],
                 const char *KeyTable[], char Par[])

Purpose:
  Decode keywords and return parameter values

Description:
  This routine decodes a keyword and returns the associated parameter value
  from a line of text.  Each parameter specification line contains information
  of the following form.
    <keyword> <sep> <value>
  The value string is separated from the keyword by a character from a set of
  delimiter characters.  The allowable keywords are specified in a keyword
  table.  This routine finds the index of the keyword match and returns the
  value associated with the keyword as a string.

  Whitespace before and after the keyword and the value is ignored. The entire
  value may be optionally enclosed in quotes.  The quotes are removed before
  the value is returned.

  The format of the keyword table entries is described in the documentation for
  routine STkeyMatch.  This routine prints a warning message if the keyword
  does not match an entry in the keyword table.

Parameters:
  <-  int STkeyXpar
      Index of the matched keyword.  This value is set to -1 if no match is
      found.
        -1 - No match
         0 - Match to the first keyword
         1 - Match to the second keyword
             ...
   -> const char Line[]
      Input text line
   -> const char Delims[]
      Character string specifying delimiter characters
   -> const char Quotes[]
      Character string specifying pairs of quote characters (the left and
      right quote characters).  In the part of the input string between a
      matched pair of quote characters, any other characters, including quote
      characters other than from the active pair, are treated as ordinary
      characters.  Up to 5 pairs of quote characters can be specified.  A zero
      length string indicates that quote characters should not to be
      recognized.
   -> const char *KeyTable[]
      Pointer array with pointers to the keyword strings.  The end of the
      keyword table is signalled with a NULL pointer.  Note that with ANSI C,
      if the actual parameter is not declared to have the const attribute, an
      explicit cast to (const char **) is required.
  <-  char Par[]
      Parameter value associated with the decoded keyword.  This array should
      allow for room for as many characters as are in the input line.  This
      string can be the same string as Line.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.16 $  $Date: 2003/05/09 03:02:44 $

-------------------------------------------------------------------------*/

#include <string.h>

#include <libtsp.h>
#include <libtsp/STmsg.h>

#define NCBUF		512
#define WS_STRIP	1


int
STkeyXpar (const char Line[], const char Delims[], const char Quotes[],
	   const char *Keytable[], char Par[])

{
  char cbuf[NCBUF+1];
  char *p;
  char *token;
  int nc;
  int ind;

/* Set up the token buffer */
  nc = strlen (Line);
  if (nc <= NCBUF)
    token = cbuf;
  else
    token = (char *) UTmalloc (nc + 1);

/* Find the keyword string */
  p = STfindToken (Line, Delims, "", token, WS_STRIP, nc);
  ind = STkeyMatch (token, Keytable);
  if (ind < 0)
    UTwarn ("STkeyXpar - %s: \"%s\"", STM_BadIdent, token);

/* Find the value string */
  p = STfindToken (p, "", Quotes, token, WS_STRIP, nc);
  if (p != NULL)
    UTwarn ("STkeyXpar - %s: \"%s\"", STM_ExtraChars, token);

/* Remove outer quote characters */
  STunQuote (token, Quotes, Par);

/* Deallocate the buffer */
  if (nc > NCBUF)
    UTfree ((void *) token);

/* Return the index */
  return ind;
}
