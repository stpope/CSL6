/*------------- Telecommunications & Signal Processing Lab -------------
                          McGill University

Routine:
  int FLexpHome (const char Fname[], char Ename[])

Purpose:
  Expand the home directory specification for a file

Description:
  This routine takes an input file name and replaces instances of "~" or
  "~USER" which appear as the first component of a directory specification.
  The first form is replaced by the home directory of the current user and the
  second form is replaced by the home directory of the named user.  If the home
  directory cannot be determined, for instance if the user is unknown, or the
  home directory specification is empty, the output file name is copied from
  the input file name.

Parameters:
  <-  int FLexpHome
      Number of characters in the output string
   -> const char Fname[]
      Input character string specifying a file name path
  <-  char Ename[]
      Output string with the home directory expanded.  This string is at most
      FILENAME_MAX characters long including the terminating null character.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.20 $  $Date: 2003/05/09 01:36:43 $

----------------------------------------------------------------------*/

#include <string.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>
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
FLexpHome (const char Fname[], char Ename[])

{
  char User[FILENAME_MAX];
  char Home[FILENAME_MAX];
  char *p;
  int n;

  if (Fname[0] == '~') {

    /* Get the user name */
    p = strchr (Fname, DIR_SEP_CHAR);
    if (p == NULL)
      STcopyMax (&Fname[1], User, FILENAME_MAX-1);
    else {
      n = (p - Fname) - 1;
      STcopyNMax (&Fname[1], User, n, FILENAME_MAX-1);
    }

    /* Find the home directory */
    FLhomeDir (User, Home);
    if (Home[0] == '~' || Home[0] == '\0')

      /* No home directory */
      n = STcopyMax (Fname, Ename, FILENAME_MAX-1);

    else {

      /* Form the output name */
      if (p == NULL)
	n = STcopyMax (Home, Ename, FILENAME_MAX-1);
      else
	n = FLjoinNames (Home, p+1, Ename);
    }
  }

  else

    /* No "~user" specification */
    n = STcopyMax (Fname, Ename, FILENAME_MAX-1);

  return n;
}
