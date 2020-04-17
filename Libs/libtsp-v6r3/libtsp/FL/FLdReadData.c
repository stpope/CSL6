/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int FLdReadData (const char Fname[], int MaxNval, float x[])

Purpose:
  Read numeric data in text form from a file

Description:
  This procedure reads floating point numeric data from a file.  The file
  contains data in text form.  Data fields are free format, with data values
  separated by white-space (as defined by isspace).  Zero or more data values
  can appear in each line of input.  Commas can also be used to separate data
  values, but only within a line, i.e. a comma should not appear at the end of
  a line.  A "!" character marks the beginning of a comment that extends to the
  end of the line.

  This routine prints an error message and halts execution on detection of an
  error.

Parameters:
  <-  int FLfReadData
      Number of values returned
   -> const char Fname[]
      File name
   -> int MaxNval
      Maximum number of values to be returned
  <-  double x[]
      Array of output values

Author / revision:
  P. Kabal  Copyright (C) 2005
  $Revision: 1.1 $  $Date: 2005/02/01 02:29:01 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/FLmsg.h>

#define COMMENT_CHAR	'!'


int
FLdReadData (const char Fname[], int MaxNval, double x[])

{
  FILE *fp;
  int Nval;

/* Open the data file */
  fp = fopen (Fname, "r");
  if (fp == NULL)
    UTerror ("FLdReadData: %s: \"%s\"", FLM_OpenRErr, Fname);

/* Read the values */
  Nval = FLdReadTF (fp, MaxNval, COMMENT_CHAR, x);

/* Close the file */
  fclose (fp);

  return Nval;
}
