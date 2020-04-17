/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  char *STfindToken (const char String[], const char Delims[],
                     const char Quotes[], char Token[], int WSFlag,
		     int Maxchar);

Purpose:
  Find the first token string in a string

Description:
  This routine finds the first token string in a given string.  A token string
  is delimited by a character from a set of delimiter characters.  If no
  delimiter is found, the token string is the entire input string.  A flag
  determines how white-space characters are treated.  Optionally, quote
  characters can be specified to allow separators to be treated as ordinary
  characters between paired quote characters.

  The processing of white-space (as defined by isspace) is controlled by the
  WSFlag flag.
  WSFlag = 0:
      This mode treats white-space characters as ordinary characters.
  WSFlag = 1:
      This mode causes leading and trailing white-space in the token string to
      be stripped off before the token string is returned.
  WSFlag = 2:
      This mode causes white-space to serve as an additional delimiter.
      There are two subcases here.  If the Delims string is empty, then
      only white-space serves as a delimiter.  However leading and trailing
      white-space in the input string does not serve as a delimiter, i.e.
      only the characters after the initial white-space are returned as token
      characters.  The second subcase occurs if the Delims string is not empty.
      Now the delimiter can be either white-space alone, or a character from
      the Delims string with optional white-space surrounding the delimiter
      character.  The white-space serving as a delimiter or surrounding the
      delimiter character is stripped off before returning the token string.

  Example:
        String = "XXX : yyy";
        Delims = ":";
        WSFlag = 1;
        STfindToken (String, Delims, Token, WSFlag, 100);
  On return, Token contains the string "XXX".

  Quote characters are specified in pairs, a left quote character and a right
  quote character for each pair.  When multiple pairs of quote characters are
  specified, the appearance of the first left quote character disables the
  quoting interpretation of characters from other pairs of quote characters.
  While within the scope of this quote character, only the left quote and the
  right quote characters from the active pair will be interpreted as special
  characters.
  Quoting strings:
      The left and right quote characters are the same.  After an initial left
      quote character, a second quote character is interpreted as a right quote
      character.  This means that nested quotes cannot occur.
  Examples:
   1: |"def ghi"|.  Quote characters |""| and white-space as a delimiter.
      The initial quote character '"' disables the interpretation of the blank
      in the string as a token delimiter.  The string returned in the token
      includes the quote characters.
   2: |"ab cd""ef gh"|.  Quote characters |""| and white-space as a delimiter.
      This string would be returned in its entirety, including the quote
      characters.

  Grouping expressions:
      In this case, the left and right quote characters are different.  Nesting
      of groups can occur.
  Example:
      |f1 (x, f2 (y,z)), f3(x)|.  Quote characters |()|, |,| as a delimiter.
      The commas occurring inside the parentheses do not serve as delimiters.
      Also, nesting is observed, so that only the second |)| character matches
      the first |(|.  The string returned is |f1 (x, f2 (y,z))|.

  Notes:
  Unmatched quote characters are not reported.  The reporting of such errors is
  left to whatever routine interprets the token strings.

  This routine always returns a Token string, even if it is of zero length.  If
  the input is zero length, or all white-space (for WSFlag!= 0), such an input
  string may be interpreted as having no token string.  To enforce such an
  interpretation, the calling routine should check for this case.

  There is no escape mechanism to allow quote characters to appear in a string
  without acting as quote characters.  Note however the behaviour described
  above, viz. once in quotes, quotes from other than the active quote pair are
  treated as ordinary characters.

  A typical program snippet for using this routine is
    p = string;
    while (p != NULL) {
      p = STfindToken (p, Delims, Quotes, Token, WSFlag, Maxchar);
      ... process Token ...
    }

Parameters:
  <-  char *STFindToken
      Pointer to the character after the delimiter.  This pointer is set to
      NULL if all tokens have been parsed from the input string.  If not NULL,
      this value is suitable for feeding back into this routine to extract the
      next token string.
   -> const char String[]
      Input character string.  If String is the NULL pointer, this routine
      returns NULL and sets Token to be the empty string.
   -> const char Delims[]
      Character string specifying delimiter characters.  White-space may also
      act as a delimiter depending on the setting of the WSFlag flag.  The
      Delims string can be zero length, indicating either that no delimiters
      are to be recognized, or that only white-space is to be recognized as a
      delimiter (if WSFlag is set appropriately).
   -> const char Quotes[]
      Character string specifying pairs of quote characters (the left and
      right quote characters).  In the part of the input string between a
      matched pair of quote characters, any other characters, including quote
      characters other than from the active pair, are treated as ordinary
      characters.  Up to 5 pairs of quote characters can be specified.  A zero
      length string indicates that quote characters should not to be
      recognized.
  <-  char Token[]
      Output token string.  This string has at most Maxchar characters, not
      including the terminating null character.  This string is always null
      terminated.  The token string may have leading and trailing white-space
      stripped off if WSFlag is set appropriately.  If the actual token string
      is more than Maxchar characters long, only Maxchar characters are written
      to Token, and a warning message is printed.  Token can be occupy the same
      string space as String.  In that case Token overwrites the beginning
      part of String.
   -> int WSFlag
      Flag controlling the interpretation of white-space characters.
        0 - White-space characters are treated as ordinary characters
        1 - Leading and trailing white-space in the token string are
            stripped off.
        2 - White-space serves as an additional delimiter
   -> int Maxchar
      Maximum number of characters (not including the trailing null character)
      to be placed in Token.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.36 $  $Date: 2003/05/09 03:16:39 $

-------------------------------------------------------------------------*/

#include <ctype.h>
#include <string.h>
#include <libtsp.h>
#include <libtsp/nucleus.h>

enum { OTHER = 0, WHITESPACE = 1, DELIM = 2 };
enum { START = 0, TOKEN = 1, WSAFTER = 2, FINIS = 3 };
enum { LEFT = 0, RIGHT = 1 };
static const int NextState[3][3] = {
/* OTHER  WHITESPACE DELIM  <== CharType */
  {TOKEN,  WSAFTER, FINIS},	/* PrevState = START */
  {TOKEN,  WSAFTER, FINIS},	/* PrevState = TOKEN */
  {FINIS,  WSAFTER, FINIS},	/* PrevState = WSAFTER */
};

static char *
ST_strochr (const char s[], int c);


char *
STfindToken (const char String[], const char Delims[], const char Quotes[],
	     char Token[], int WSFlag, int Maxchar)

{
  int inquotes;
  int n;
  const char *pst;
  const char *p;
  int CharType;
  char *q;
  char QC[3];
  int State;

  CharType = OTHER;

/* Check for NULL string */
  if (String == NULL) {
    Token[0] = '\0';
    return NULL;
  }

/* Find the first non-white-space character */
  if (WSFlag > 0)
    pst = STtrimIws (String);
  else
    pst = String;

/*
   Main loop to check each character in the string
   The search is terminated when the string is exhausted or a separator is
   found.
*/
  State = START;
  inquotes = 0;
  p = pst;
  while (*p != '\0' && State != FINIS) {

/* Find the character type */
    if (inquotes) {

/* In quotes */
      CharType = OTHER;
      /*
	 Check only the active quote pair in QC.
	 If the left and right quote characters are the same, the match
	 should be interpreted as a right quote.
      */
      q = strchr (QC, *p);
      if (q != NULL) {
	if ((q-QC) % 2 == RIGHT || QC[0] == QC[1])
	  inquotes--;
	else
	  inquotes++;
      }
    }

    else {

/* Not in quotes, look for new left quotes */
      /*
	 Check all left quote characters, ignoring possible right quote
	 matches.
      */
      q = ST_strochr (Quotes, *p);
      if (q != NULL) {
	CharType = OTHER;
	n = q-Quotes;
	inquotes++;
	STcopyNMax (&Quotes[n], QC, 2, 2);
      }

/* Not in quotes - process other characters */
      /* Check for a separator */
      else if (strchr (Delims, *p) != NULL)
	CharType = DELIM;

      /* Look for white-space */
      else if (WSFlag > 1 && isspace ((int) *p))
	CharType = WHITESPACE;

      /* Must be other */
      else
	CharType = OTHER;
    }

/* Make a state transition */
    State = NextState [State][CharType];
    n = p - String;
    ++p;
  }

/* End of loop
   Note: The flag inquotes may be set on loop exit in the case that a left
     quote after a white-space delimiter terminated the scan.

   Several ways to exit the loop:
     - null character acting as a delimiter, p points at the null character.
       For this case, State != FINIS.
     - delimiter found, p points at one beyond the delimiter, State == FINIS.
     - white-space terminated by an OTHER character, p points at one beyond the
       OTHER character and should be pushed back, State == FINIS.
*/
  if (State == FINIS) {
    if (CharType == OTHER)
      --p;		/* unget the last character read */
    n = (p - pst) - 1;
  }
  else
    n = p - pst;	/* Null character terminated the scan */

/* Omit trailing white-space, if appropriate */
  if (WSFlag > 0)
    STtrimNMax (pst, Token, n, Maxchar);
  else
    STcopyNMax (pst, Token, n, Maxchar);

/* Return the pointer to the next character */
  if (State == FINIS)
    return ((char *) p);
  else
    return NULL;	/* Null character terminated the scan */
}

/* Modified version of strchr
   - checks every second character
   - number of characters can be odd
*/


static char *
ST_strochr (const char *s, int c)

{
  char ch;

  ch = (char) c;
  for (; *s != '\0' && *(s+1) != '\0'; ++s, ++s) {
    if (*s == ch)
      return (char *) s;
  }
  return NULL;

}
