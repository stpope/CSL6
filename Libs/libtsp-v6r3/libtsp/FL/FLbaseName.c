/*------------- Telecommunications & Signal Processing Lab -------------
                          McGill University

Routine:
  int FLbaseName (const char Fname[], char Bname[])

Purpose:
  Return the last component of a file path

Description:
  This routine takes a file path specification and returns the rightmost
  component.  Components are separated by "/" characters.  If no "/" character
  appears in the path, the input path name is returned.  For Windows, the
  directory separator character is "\".

Parameters:
  <-  int FLbaseName
      Number of characters in the output string
   -> const char Fname[]
      Input character string with the path name
  <-  char Bname[]
      Output string with the last path name component.  This string is at most
      FILENAME_MAX characters long including the terminating null character.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.20 $  $Date: 2003/05/09 01:36:43 $

----------------------------------------------------------------------*/

#include <string.h>

#include <libtsp.h>
#include <libtsp/sysOS.h>

#if (SY_FILENAME_SPEC == SY_FNS_UNIX)
#  define DIR_SEP_STR	"/"
#elif (SY_FILENAME_SPEC == SY_FNS_WINDOWS)
#  define DIR_SEP_STR	"\\"
#else
#  error "Bad SY_FILENAME_SPEC value"
#endif
#define DIR_SEP_CHAR	((DIR_SEP_STR)[0])


int
FLbaseName (const char Fname[], char Bname[])

{
  char *p;
  int n;

  p = strrchr (Fname, DIR_SEP_CHAR);
  if (p != NULL)
    n = STcopyMax (p+1, Bname, FILENAME_MAX-1);
  else
    n = STcopyMax (Fname, Bname, FILENAME_MAX-1);

  return n;
}
