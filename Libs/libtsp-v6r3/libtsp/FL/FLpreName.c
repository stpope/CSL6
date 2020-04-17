/*------------- Telecommunications & Signal Processing Lab -------------
                          McGill University

Routine:
  int FLpreName (const char Fname[], char Pname[])

Purpose:
  Return the last component of a file name stripped of its extension

Description:
  This routine takes a filename and returns the last component of the filename
  stripped of the filename extension.  The extension is the part of the last
  component of the path name beginning with (and including) a period.  The
  string returned for a path name "/abc/def.gh.ij" is "def.gh".

Parameters:
  <-  int FLpreName
      Number of characters in the output string
   -> const char Fname[]
      Input character string with the file name
  <-  char Pname[]
      Output string filename string without the extension.  This string is
      at most FILENAME_MAX characters long including the terminating null
      character.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.8 $  $Date: 2003/05/09 01:36:44 $

----------------------------------------------------------------------*/

#include <string.h>

#include <libtsp.h>


int
FLpreName (const char Fname[], char Pname[])

{
  char *p;
  int n;
  char Bname[FILENAME_MAX];

  FLbaseName (Fname, Bname);
  p = strrchr (Bname, '.');
  if (p != NULL)
    *p = '\0';
  n = STcopyMax (Bname, Pname, FILENAME_MAX-1);

  return n;
}
