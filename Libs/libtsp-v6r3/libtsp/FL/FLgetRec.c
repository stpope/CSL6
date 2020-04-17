/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  char *FLgetRec (FILE *fp, const char CommChar[], const char ContChar[],
                  int echo)

Purpose:
  Read and assemble a text record from an input stream

Description:
  This routine reads lines of text from an input stream.  Comments are removed
  and multiple lines with continuation marks are assembled into a single text
  record.  Empty lines (all white space) are considered to be comment lines.

  The operations in assembling a text record are as follows.
   1: Read a line of text
   2: Remove any characters from the comment marker to the end of the line
   3: Look for a continuation marker as the last character on the line.  For
      lines with continuation markers: remove the continuation marker and
      concatenate the next line onto the end of the current line.

  This routine has provision for echoing the input lines, with or without
  comments.  No echoing is done is the input is from a terminal.

Parameters:
  <-  char *FLgetRec
      Pointer to the text string.  This is NULL if end-of-file is encountered
      and the line is empty.  Otherwise, when end-of-file is encountered, the
      line is treated as if it were terminated with a newline.  The text string
      is in an internally allocated storage area; each call to this routine
      overlays this storage.
   -> FILE *fp
      File pointer for the input stream
   -> const char CommChar[]
      String containing characters that mark the beginning of a comment.  This
      string may be empty.
   -> const char ContChar[]
      String containing characters that mark a continuation line.  This string
      may be empty.
   -> int echo
      Echo control flag. 0 - No echo, 1 - Echo with comments stripped off,
      2, echo with comments intact
      
Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.18 $  $Date: 2003/05/09 01:36:44 $

-------------------------------------------------------------------------*/

#include <string.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>

#define ECHO_NOCOMMENTS		1
#define ECHO_COMMENTS		2


char *
FLgetRec (FILE *fp, const char CommChar[], const char ContChar[], int echo)

{
  char *line, *rec;
  char *c;
  int nl, nc;
  static char *lbuf = NULL;

/* Reclaim the buffer space */
  UTfree ((void *) lbuf);

/* Loop over blank lines and empty (or comment) lines */
/* lbuf != NULL - continuation lines are being processed */
  lbuf = NULL;
  while (1) {

  /* Read a line of input */
    line = FLgetLine (fp);
    if (line == NULL) {
      rec = lbuf;	/* return lbuf, which may be NULL */
      break;
    }

  /* Echo option - with comments */
    if (echo == ECHO_COMMENTS && ! FLterm (fp)) {
      fprintf (stdout, "%s\n", line);
      fflush (stdout);
    }

  /* Process comments */
    c = strpbrk (line, CommChar);
    if (c != NULL)
      *c = '\0';		/* Force an end-of-line at the comment */
    c = STtrimIws (line);
    if (*c == '\0')
      line[0] = '\0';

  /* Echo option - without comments */
    if (echo == ECHO_NOCOMMENTS && ! FLterm (fp) && line[0] != '\0') {
      fprintf (stdout, "%s\n", line);
      fflush (stdout);
    }

 /* Check for a continuation mark on the current line */
    nl = strlen (line);
    if (nl > 0)
      c = strpbrk (&line[nl-1], ContChar);
    else
      c = NULL;
    if (c != NULL) {
      --nl;
      *c = '\0';
      if (lbuf == NULL) {
	lbuf = (char *) UTmalloc (nl+1);
	strcpy (lbuf, line);
      }
      else {
	nc = strlen (lbuf);
	lbuf = (char *) UTrealloc (lbuf, nc+nl+1);
	strcpy (&lbuf[nc], line);
      }
      continue;			/* Loop again to pick up the next line */
    }

    if (lbuf == NULL) {

    /* Non-continuation line */
      if (nl == 0)
	continue;		/* Loop again to pick up the next line */
      rec = line;
      break;
    }
    else {

    /* Concatenate the last of a group of continuation lines */
      if (nl > 0) {
	nc = strlen (lbuf);
	lbuf = (char *) UTrealloc (lbuf, nc+nl+1);
	strcpy (&lbuf[nc], line);
      }
      rec = lbuf;		/* return the concatenated line */
      break;
    }

  }	/* end while loop */

  return rec;
}
