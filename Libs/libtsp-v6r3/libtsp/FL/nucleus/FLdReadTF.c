/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int FLdReadTF (FILE *fp, int MaxNval, int cc, double x[])

Purpose:
  Read numeric data in text form from a stream

Description:
  This procedure reads floating point numeric data from a stream.  The stream
  contains data in text form.  Data fields are free format, with data values
  separated by white-space (as defined by isspace).  Zero or more data values
  can appear in each line of input.  Commas can also be used to separate data
  values, but only within a line, i.e. a comma should not appear at the end of
  a line.  The cc argument specifies a character which marks the beginning of
  comments.

  This routine prints an error message and halts execution on detection of an
  error.

Parameters:
  <-  int FLdReadTF
      Number of values returned
   -> FILE *fp
      File pointer to the stream
   -> int MaxNval
      Maximum number of values to be returned
   -> int cc
      Comment delimiter
  <-  double x[]
      Array of output values

Author / revision:
  P. Kabal  Copyright (C) 2005
  $Revision: 1.1 $  $Date: 2005/02/01 02:32:07 $

-------------------------------------------------------------------------*/

#include <string.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/FLmsg.h>


int
FLdReadTF (FILE *fp, int MaxNval, int cc, double x[])

{
  char *line;
  char *c;
  int N, Nvr, Nv, lineno;

/* Read the values until end-of-file */
  N = 0;
  lineno = 0;
  while (1) {

    /* Read a line of input */
    line = FLgetLine (fp);
    if (line == NULL)
      break;
    ++lineno;

    /* Process comments */
    c = strchr (line, cc);
    if (c != NULL)
      *c = '\0';		/* Force an end-of-line at the comment */

    /* Decode data records */
    Nvr = MaxNval - N;
    if (Nvr > 0) {
      if (STdecNdouble (line, 0, Nvr, &x[N], &Nv))
	UThalt ("FLdReadTF: %s %d", FLM_DataErr, lineno);
      N = N + Nv;
    }
    else if (*(STtrimIws (line)) != '\0')
      UThalt ("FLdReadTF: %s", FLM_ExtraData);
  }

/* Return the number of values */
  return N;
}
