/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  long int FLfileSize (FILE *fp)

Purpose:
  Find the size of a file

Description:
  This routine finds the size of a binary file.  The size of the file is
  determined by positioning to end-of-file and returning the end-of-file
  position.  On exit, the file position is restored to the original position.

Parameters:
  <-  long int FLfileSize
      File size in bytes
   -> FILE *fp
      File pointer associated with the file

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.22 $  $Date: 2003/05/09 01:39:26 $

-------------------------------------------------------------------------*/

#include <errno.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/FLmsg.h>


long int
FLfileSize (FILE *fp)

{
  long int pos;
  long int endpos;

/*
   The ANSI C standard does not guarantee that this method of determining
   the file size will work on all systems.  It works on Unix systems and
   probably a lot of other systems.  The alternative is to use stat, but that
   is probably even less portable.
*/
  errno = 0;
  pos = ftell (fp);
  if (pos == -1L && errno)
    UTerror ("FLfileSize: %s", FLM_NoFilePos);

  if (fseek (fp, 0L, SEEK_END) != 0)
    UThalt ("FLfileSize: %s", FLM_FilePosErr);

  endpos = ftell (fp);
  fseek (fp, pos, SEEK_SET);

  return endpos;
}
