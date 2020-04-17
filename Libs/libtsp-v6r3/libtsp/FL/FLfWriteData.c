/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void FLfWriteData (const char Fname[], const float x[], int N)

Purpose:
  Write a float array in text form to a file

Description:
  This routine writes float data to a named file.  The float data is written to
  the file as text, with one data value to a line.

  The intended use of this routine is to write data to a file in a form that
  can be edited or imported into other programs.  An example would be to write
  data that can be easily imported into MATLAB, using the MATLAB load command.

Parameters:
   -> const char Fname[]
      File name for the data
   -> const float x[]
      Array of N floats
   -> int N
      Number of elements in x

Author / revision:
  P. Kabal  Copyright (C) 2005
  $Revision: 1.12 $  $Date: 2005/02/01 13:18:39 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/FLmsg.h>


void
FLfWriteData (const char Fname[], const float x[], int N)

{
  FILE *fp;
  int i;

  fp = fopen (Fname, "w");
  if (fp == NULL)
    UTerror ("FLfWriteData: %s: \"%s\"", FLM_OpenWErr, Fname);

  for (i = 0; i < N; ++i)
    fprintf (fp, "%g\n", x[i]);

  fclose (fp);

  return;
}
