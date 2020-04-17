/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  char *AFgetLine (FILE *fp, int *ErrCode);

Purpose:
  Read a line of text from a file

Description:
  This routine retrieves a line of input from a file.  This routine reads
  characters until a newline is encountered.  The newline is not returned.
  The return value is a pointer to the text string or NULL if end-of-file is
  encountered.

Parameters:
  <-  char *AFgetLine
      Pointer to the text string.  This is NULL if end-of-file is encountered
      and the line is empty.  Otherwise, when end-of-file is encountered, the
      line is treated as if it were terminated with a newline.  The text string
      is in an internally storage area; each call to this routine overlays
      this storage.
   -> FILE *fp
      File pointer to the file
  <-> int ErrCode
      Error code.  This value is set if an error is detected, but otherwise
      remains unchanged.  If ErrCode is set on input, this routine is a no-op.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.8 $  $Date: 2003/05/09 01:11:34 $

-------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include <libtsp.h>
#include <libtsp/AFdataio.h>
#include <libtsp/AFmsg.h>
#include <libtsp/AFpar.h>

#define NBUF	256			/* buffer size */


char *
AFgetLine (FILE *fp, int *ErrCode)

{
  char *p;
  int nc;
  static char line[NBUF];

  if (! *ErrCode) {

    /* Read a line or partial line */
    line[0] = '\0';
    p = fgets (line, NBUF, fp);

    /* Check for error or empty line at end-of-file */
    if (p == NULL) {
      if (ferror (fp)) {
	UTsysMsg ("AFgetLine: %s", AFM_ReadErr);
	*ErrCode = AF_IOERR;
      }
      return NULL;
    }

    /* Check that end-of-line has been reached (delete the newline) */
    /* - terminated by newline (successful read)
       - not terminated by newline
           - line too long (error)
           - OK, end-of-file and NULL line will be returned the next time
    */
    nc = strlen (line);
    if (line[nc-1] == '\n')
      line[nc-1] = '\0';
    else if (nc >= NBUF-1) {
      line[nc-1] = '\0';
      UTwarn ("AFgetLine - %s", AFM_LongLine);
      *ErrCode = AF_DECERR;
    }

  }

  return line;
}
