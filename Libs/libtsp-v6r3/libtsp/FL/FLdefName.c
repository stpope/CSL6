/*------------- Telecommunications & Signal Processing Lab -------------
                         McGill University

Routine:
  int FLdefName (const char Fi[], const char Fd[], char Fo[])

Purpose:
  Fill in file name components from a default string

Description:
  This routine takes file name components from two input strings.  The
  components are the path name (directory string) and the file name.  The
  output file name is formed by taking components from the first string.  If
  If components are missing, components from the second string, if present,
  are used as defaults.

  The examples below show the results for Unix style filenames.  For Windows,
  the directory separator character is "\".

   first string   default string  output string
     a             /dir/c         /dir/a
     a             dir/           dir/a
     /path/a       /dir/c         /path/a
     -             /dir/c         /dir/c
     path/         /dir/c         /path/c

Parameters:
  <-  int FLdefName
      Number of characters in the output string
   -> const char Fi[]
      Input character string containing the first file name.  The components of
      this name, if present, will be used in the output name.
   -> const char Fd[]
      Input character string containing the default file name.  The components
      of this name are used if the corresponding components of the first name
      are missing.
  <-  char Fo[]
      Character string with the output file name.  This string has at most
      FILENAME_MAX characters including the terminating null character.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.20 $  $Date: 2003/05/09 01:36:43 $

----------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/nucleus.h>


int
FLdefName (const char Fi[], const char Fd[], char Fo[])

{
  char Fdir[FILENAME_MAX];
  char Fbase[FILENAME_MAX];
  int n;

/* Parse the input names into the constituent parts */
  FLdirName (Fi, Fdir);
  FLbaseName (Fi, Fbase);

/* Replace missing parts from the default name */
  if (Fdir[0] == '\0')
    FLdirName (Fd, Fdir);
  if (Fbase[0] == '\0')
    FLbaseName (Fd, Fbase);

/* Form the output string */
  n = FLjoinNames (Fdir, Fbase, Fo);

  return n;
}
