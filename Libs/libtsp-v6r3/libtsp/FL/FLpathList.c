/*------------- Telecommunications & Signal Processing Lab -------------
                         McGill University

Routine:
  int FLpathList (const char Fi[], const char Dlist[], char Fo[])

Purpose:
  Fill in a directory for a file name from a list of paths

Description:
  This routine fills in a directory path name for a file name from a list of
  directories.  The intended purpose of this routine is to fill in the
  directory path from a list of defaults prior to opening the file.  If the
  input file name specifies any directory components, the output name is set
  to the input name.  If the input file name is a simple file name (no
  directory components), each path from the list is used in turn as a directory
  path.  A check is made to see if the corresponding file can be found.  The
  file name of the first file found is returned.  If the search for such a file
  is unsuccessful, the input name is returned.  Note that this behaviour
  effectively puts the current directory at the end of the list of paths.  The
  user can specify a search of the current directory in some other order, by
  including the current directory in the list of paths.  The current directory
  can be specified as "." or as an empty specification.

  For Unix sytems this routine performs the additional service of expanding
  home directory specifications of the form "~" or "~USER" which appear as the
  first component of the path name in the input name or in one of the paths in
  the list of paths.  In the case of the input name containing a home directory
  specification, the resulting expanded name will then have a directory
  component and no further search using the list of paths will occur.  If
  the home directory cannot be found (unknown user, for instance), the "~"
  specification is treated as part of an ordinary filename string.

  The path name list is a white-space or colon (semicolon for Windows)
  separated list of paths.  For example, ":/usr/users/usera/:~userb".  The
  path names can have an optional trailing / character.  This specification
  includes the current directory as the first element of the list.

  The list of path names can be specified directly in the Dlist variable, or
  Dlist can specify the name of an environment variable (signalled by a leading
  $ character) which contains the list.

Parameters:
  <-  int FLpathList
      Number of characters in the output string
   -> const char Fi[]
      Character string containing the file name
   -> const char Dlist[]
      Input character string containing a list of directories, or the name of
      an environment variable (including a leading $).  If Dlist specifies an
      environment variable, the value of this variable supplies the list of
      directories.
  <-  char Fo[]
      Character string containing the file name.  At most FILENAME_MAX
      characters including the terminating null are returned.  This string can
      overlay the input string if desired.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.28 $  $Date: 2003/05/09 01:36:44 $

----------------------------------------------------------------------*/

#include <stdlib.h>	/* getenv */
#include <sys/types.h>
#include <sys/stat.h>

#include <libtsp.h>
#include <libtsp/sysOS.h>

#if (SY_FILENAME_SPEC == SY_FNS_UNIX)
#  define DIR_SEP_STR	"/"
#  define PATH_SEP_STR	":"
#elif (SY_FILENAME_SPEC == SY_FNS_WINDOWS)
#  define DIR_SEP_STR	"\\"
#  define PATH_SEP_STR	";"
#else
#  error "Bad SY_FILENAME_SPEC value"
#endif
#define DIR_SEP_CHAR	((DIR_SEP_STR)[0])


int
FLpathList (const char Fi[], const char Dlist[], char Fo[])

{
  struct stat Fstat;
  char Fexp[FILENAME_MAX];
  char Fdir[FILENAME_MAX];
  char Ftrial[FILENAME_MAX];
  char *p;
  int n, nd;

/* Check if the input name already has any directory specified. */
  FLexpHome (Fi, Fexp);
  FLdirName (Fexp, Fdir);
  if (Fdir[0] != '\0') {
    n = STcopyMax (Fexp, Fo, FILENAME_MAX-1);
    return n;
  }

/* Check for an environment variable */
  if (Dlist[0] == '$') {
    p = getenv (&Dlist[1]);
    if (p == NULL)
      p = "";
  }
  else
    p = (char *) Dlist;

/* Zero length strings */
  if (*p == '\0') {
    n = STcopyMax (Fexp, Fo, FILENAME_MAX-1);
    return n;
  }

/* Loop over the list of directories */
  while (p != NULL) {
    p = STfindToken (p, PATH_SEP_STR, "", Ftrial, 1, FILENAME_MAX-1);

    /* Expand the home directory "~User" */
    nd = FLexpHome (Ftrial, Fdir);

    /* Construct a trial name; supply a / for the directory if needed */
    if (nd > 0 && Fdir[nd-1] != DIR_SEP_CHAR)
      STcatMax (DIR_SEP_STR, Fdir, FILENAME_MAX-1);
    FLdefName (Fexp, Fdir, Ftrial);

/* Use stat to determine if the file exists (no check to see if it is a regular
   file or not, or whether it is readable). */
    if (stat (Ftrial, &Fstat) == 0) {
      n = STcopyMax (Ftrial, Fo, FILENAME_MAX-1);
      return n;
    }
  }

/* No luck in finding a file, return the input name */
  n = STcopyMax (Fexp, Fo, FILENAME_MAX-1);
  return n;
}
