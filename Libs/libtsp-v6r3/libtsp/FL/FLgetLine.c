/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  char *FLgetLine (FILE *fp);

Purpose:
  Read a line of text from a file

Description:
  This routine retrieves a line of input from a file.  This routine reads
  characters until a newline is encountered.  The newline is not returned.
  The return value is a pointer to the text string or NULL if end-of-file is
  encountered.

Parameters:
  <-  char *FLgetLine
      Pointer to the text string.  This is NULL if end-of-file is encountered
      and the line is empty.  Otherwise, when end-of-file is encountered, the
      line is treated as if it were terminated with a newline.  The text string
      is in an internally allocated storage area; each call to this routine
      overlays this storage.
   -> FILE *fp
      File pointer to the file

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.15 $  $Date: 2003/05/09 01:36:44 $

-------------------------------------------------------------------------*/

#include <string.h>

#include <libtsp.h>
#include <libtsp/FLmsg.h>

#define DEF_BUF_SIZE	256			/* default buffer size */
#define MAX_BUF_SIZE	(40 * DEF_BUF_SIZE)	/* max buffer size */


char *
FLgetLine (FILE *fp)

{
  char *p;
  int nc;
  static char *linebuf = NULL;
  static int lenbuf = 0;

/* On each call, restore the buffer to the default size */
  if (lenbuf != DEF_BUF_SIZE) {
    UTfree ((void *) linebuf);
    linebuf = (char *) UTmalloc (DEF_BUF_SIZE);
    lenbuf = DEF_BUF_SIZE;
  }

/* Read a line of input */
  nc = 0;
  linebuf[0] = '\0';
  while (1) {

    /* Read a line or partial line */
    p = fgets (&linebuf[nc], lenbuf - nc, fp);
    nc += strlen (&linebuf[nc]);

    /* Check for end-of-file */
    if (p == NULL) {
      if (! feof (fp))
	UTerror("FLgetLine: %s", FLM_ReadErr);
      if (nc != 0)
	break;
      else {
	UTfree ((void *) linebuf);
	lenbuf = 0;
	linebuf = NULL;
	break;
      }
    }

    /* Return if the end-of-line has been reached (delete the newline) */
    if (linebuf[nc-1] == '\n') {	/* nc is always > 0 */
      linebuf[nc-1] = '\0';
      break;
    }

    /* Allocate more buffer space */
    lenbuf += DEF_BUF_SIZE;
    if (lenbuf > MAX_BUF_SIZE)
      UThalt ("FLgetLine: %s", FLM_LongLine);
    linebuf = (char *) UTrealloc (linebuf, lenbuf);
  }

  return linebuf;
}
