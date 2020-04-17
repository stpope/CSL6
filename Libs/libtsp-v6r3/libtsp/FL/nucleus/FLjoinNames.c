/*------------- Telecommunications & Signal Processing Lab -------------
                          McGill University

Routine:
  int FLjoinNames (const char Dname[], const char Bname[], char Fname[])

Purpose:
  Form a file name from a directory path and a base name

Description:
  This routine takes an input file path and appends file name components to
  form a file name string.  This routine can be used to form a name from a
  directory string and a file name string.  It joins these components together
  separated by a "/" character ("\" character for Windows).  However, the "/"
  character is omitted if either input part is empty or if the directory part
  already ends in a "/".  The operations are arranged so that the output file
  name can overlay either of the input strings.

  If the base name starts with the root directory, this name alone appears in
  the output string.

Parameters:
  <-  int FLjoinNames
      Number of characters in the output string
   -> const char Dname[]
      Input character string with the path name
   -> const char Bname[]
      Input character string with the base name of the file
  <-  char Fname[]
      Output string formed by joining the two input strings.  This string is
      at most FILENAME_MAX characters long including the terminating null
      character.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.19 $  $Date: 2003/05/09 01:39:26 $

----------------------------------------------------------------------*/

#include <ctype.h>
#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/sysOS.h>
#include <libtsp/FLmsg.h>

#if (SY_FILENAME_SPEC == SY_FNS_UNIX)
#  define DIR_SEP_STR	"/"
#elif (SY_FILENAME_SPEC == SY_FNS_WINDOWS)
#  define DIR_SEP_STR	"\\"
#else
#  error "Bad SY_FILENAME_SPEC value"
#endif
#define DIR_SEP_CHAR	((DIR_SEP_STR)[0])

static int
FL_rootdir (const char Fname[]);


int
FLjoinNames (const char Dname[], const char Bname[], char Fname[])

{
  char tname[FILENAME_MAX+1];	/* One extra: catch names that are too long */
  int n;

  /* Test for a root directory specification in Bname */
  if (FL_rootdir (Bname))
    /* Basename alone appears in the output string */
    n = STcopyMax (Bname, Fname, FILENAME_MAX-1);

  else {

    /* Form the combined name in temporary storage */
    n = STcopyMax (Dname, tname, FILENAME_MAX);
    if (n > 0 && Bname[0] != '\0' && tname[n-1] != DIR_SEP_CHAR)
      STcatMax (DIR_SEP_STR, tname, FILENAME_MAX);
    n = STcatMax (Bname, tname, FILENAME_MAX);
    if (n > FILENAME_MAX-1)
      UThalt ("FLjoinNames: %s", FLM_LongName);

    n = STcopyMax (tname, Fname, FILENAME_MAX-1);
  }

  return n;
}

static int
FL_rootdir (const char Fname[])

{
  int rootdir;

  rootdir = 0;
  if (Fname[0] == DIR_SEP_CHAR)
    rootdir = 1;

#if (SY_FILENAME_SPEC == SY_FNS_WINDOWS)
  else if (isalpha(Fname[0]) && Fname[1] == ':')
    rootdir = 1;
#endif

  return rootdir;
}
