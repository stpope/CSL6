/*-------------------------------------------------------------------------

Routine:
  int UTdecOption (const char String[], const char Option[],
                   const char **Arg)

Purpose:
  Match a string to an option keyword

Description:
  This subroutine checks for a match to an option keyword string.  The keyword
  string can specify the minimum length for a match and a length beyond which
  characters need no longer match.

  The option keyword is a string with an optional asterisk ('*') which is used
  to indicate the minimum number of characters needed for a match.  A second 
  asterisk can be used to indicate that characters after this point need not
  match.  A trailing '#' indicates that an argument follows the keyword.  A
  trailing '=' indicates that the argument is separated from the keyword by a
  '=' character.  On finding an argument, this routine returns a pointer to the
  beginning of the argument part of the input string.

  Example keywords:
    "abc" - Simple option, all characters have to match
    "a*bc" - Variable length option, matches a substring of "abc"
    "ab#" - Simple keyword followed by an argument.  The string "ab33" matches
        with an argument value of "33".
    "a*bc=" - Variable length keyword followed by an argument separated by a
        '=' character.  The string "ab=33" matches with an argument value of
	"33".

Parameters:
  <-  int *UTdecOption
      Status status.
      -2, Matches an option, but argument missing
       0, Not a recognized option
       1, Matches an option, argument present or not needed
   -> const char String[]
      Input character string
   -> const char Option[]
      Input option keyword specification
  <-  const char **Arg
      Argument string.  For an option with the argument following as part of
      the input string, this is a pointer to a null terminated substring of
      String.  If the decoded option does not take a value or the value is
      missing, this pointer will be NULL.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.23 $  $Date: 2003/05/09 03:23:41 $

-------------------------------------------------------------------------*/

#include <string.h>

#include <libtsp/nucleus.h>

#define NOSTAR	0
#define STAR	1

#define NOARG	0
#define HASH	1
#define EQUAL	2

#define NOMATCH	0	/* No match */
#define MATCH	1	/* Match, argument present or not needed */
#define MVAL	-2	/* Match, argument missing */


int
UTdecOption (const char String[], const char Option[], const char **Arg)

{
  const char *kt;
  const char *kend;
  const char *s;
  int lenk, state, argt;

/* See if an argument is needed
     NOARG - No argument allowed
     HASH - Argument follows directly
     EQUAL - Argument follows an equal sign
   kend is the character past the last one of interest for matching
*/
  lenk = strlen (Option);
  kend = &Option[lenk];
  argt = NOARG;
  if (lenk > 0) {
    if (Option[lenk-1] == '#') {
      --kend;
      argt = HASH;
    }
    else if (Option[lenk-1] == '=') {
      --kend;
      argt = EQUAL;
    }
  }

  /* Main loop to check matching characters */
  state = NOSTAR;
  for (kt = Option, s = String; kt < kend && *s != '\0'; ++kt) {
    if (*kt == '*') {
      if (state == NOSTAR)
	state = STAR;
      else {
	*Arg = NULL;		/* Second asterisk: ignore remaining	*/
	if (argt != NOARG)	/* characters in the string; these are	*/
	  return MVAL;		/* not considered to be part of an	*/
	else			/* argument string.			*/
	  return MATCH;
      }
    }
    else if (*s != *kt) {
      if (*s == '=' && state == STAR && argt == EQUAL) {
	*Arg = s+1;		/* Characters differ: this is normally	*/
	return MATCH;		/* a no-match condition, except if the	*/
      }				/* string contains an '=', the minimum	*/
      else {			/* number of characters has been	*/
	*Arg = NULL;		/* matched, and an argument of type	*/
	return NOMATCH;		/* EQUAL is expected.			*/
      }
    }
    else
      ++s;
  }
  /* On exit from the loop:  Ran out of characters in the input string or the
     keyword string (*s == '\0' or kt == kend).  The condition kt == kend
     triggers a state = STAR below.

     Each keyword has an implicit asterisk at the end, to indicate that the
     keyword constains the minimum number of characters to be matched.

     The STAR state means that the minimum number of matching characters has
     been matched.
  */
  if (state == NOSTAR && (kt == kend || *kt == '*'))
    state = STAR;

  /* A match will occur only if the minimum number of character has been
     matched (state == STAR).
   1. The string has been exhausted (*s == '\0')
   2. String not finished (*s != '\0')
      (a) Argument expected
      (b) No argument expected (NOMATCH)
  */
  if (state == STAR) {
    if (*s == '\0') {		/* case 1 */
      *Arg = NULL;
      if (argt != NOARG)
	return MVAL;
      else
	return MATCH;
    }
    else {			/* case 2 */
      if (argt == HASH) {
	*Arg = s;
	return MATCH;
      }
      else if (argt == EQUAL && *s == '=') {
	*Arg = s+1;
	return MATCH;
      }
    }
  }
  
  *Arg = NULL;
  return NOMATCH;
}
