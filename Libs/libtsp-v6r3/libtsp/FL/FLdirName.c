/*------------- Telecommunications & Signal Processing Lab -------------
                          McGill University

Routine:
  int FLdirName (const char Fname[], char Bname[])

Purpose:
  Return all but the last component of a file path

Description:
  This routine takes a file path specification and returns all but the
  right-most component of the file name.  Components are separated by "/"
  characters.  If no "/" character appears in the name, an empty string is
  returned.  For Windows, the directory separator character is "\".

  Examples: 
     "abc/def"      => "abc"
     "abc/def/"     => "abc/def"
     "abc/bcd/def"  => "abc/bcd"
     "/abc/def"     => "/abc"
     "/abc/bcd/def" => "/abc/bcd"
     "./def"        => "."
     "def"          => ""
  Special cases: 
     "/def"         => "/"
     "/"            => "/"

Parameters:
  <-  int FLdirName
      Number of characters in the output string
   -> const char Fname[]
      Input character string with the path name
  <-  char Bname[]
      Output string with the all but the last path name component.  This string
      at most FILENAME_MAX characters long including the terminating null
      character.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.22 $  $Date: 2003/05/09 01:36:43 $

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
FLdirName (const char Fname[], char Dname[])

{
  char *p;
  int n, nc;

/* Break the string at the last directory separator */
  p = strrchr (Fname, DIR_SEP_CHAR);
  if (p != NULL) {
    nc = p - Fname;
    if (nc == 0)
      n = STcopyMax (DIR_SEP_STR, Dname, FILENAME_MAX-1);
    else
      n = STcopyNMax (Fname, Dname, nc, FILENAME_MAX-1);
  }
  else {
    Dname[0] = '\0';
    n = 0;
  }

  return n;
}
