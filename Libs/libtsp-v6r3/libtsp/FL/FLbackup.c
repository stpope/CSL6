/*------------ Telecommunications & Signal Processing Lab -------------
                         McGill University

Routine:
  void FLbackup (const char Fname[])

Purpose:
  Rename an existing file

Description:
  This subroutine backs up a file.  If a file with the given name exists, say
  xxx, that file is renamed to xxx~.  Any previous file with the name xxx~ is
  removed.

  Only regular files are backed up.  If a backup file is created, a warning
  message indicating the new name is issued.  One use of this routine is to
  a copy of an existing file before opening a new file.

  This routine will terminate execution if the file cannot be backed up.

Parameters:
   -> const char Fname[]
      File name

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.33 $  $Date: 2003/05/09 01:36:43 $

----------------------------------------------------------------------*/

#include <string.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/FLmsg.h>

#define MINV(a, b)	(((a) < (b)) ? (a) : (b))


void
FLbackup (const char Fname[])

{
  char Fback[FILENAME_MAX];
  int n;

  /* Rename only if the file is a regular file */
  if (FLexist (Fname)) {

/* Add a ~ to the overall file name */
    n = STcopyMax (Fname, Fback, FILENAME_MAX-2);
    Fback[n] = '~';
    Fback[n+1] = '\0';

/* Rename the file; rename is not guaranteed to delete a previous backup */
    if (FLexist (Fback)) {
      if (remove (Fback) != 0)
	UTerror ("FLbackup: %s: \"%s\"", FLM_DelBackErr, Fback);
    }
    if (rename (Fname, Fback) == 0)
      UTwarn ("FLbackup - %s \"%s\"", FLM_Rename, Fback);
    else
      UTerror ("FLbackup: %s: \"%s\"", FLM_RenameErr, Fname);
  }
  return;
}
