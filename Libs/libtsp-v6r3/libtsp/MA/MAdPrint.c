/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void MAdPrint (FILE *fp, const char Header[], const double *A[], int Nrow,
		 int Ncol)

Purpose:
  Print a double matrix along with a header text

Description:
  This routine prints the values of a double matrix along with an identifying
  header.  No printing occurs if the file pointer is NULL.  The values for each
  row of the matrix are printed 5 to a line, with each row starting on a new
  line.

Parameters:
   -> FILE *fp
      File pointer for the output.  If fp is NULL, no printing occurs.
   -> const char Header[]
      Character string for the header.  This routine supplies a terminating
      newline.  The header line is omitted if the header string is empty.
   -> const double *A[]
      Pointer to an array of row pointers.  A is interpreted as an N by M
      matrix.  Note that with ANSI C, if the actual parameter is not declared
      to have the const attribute, an explicit cast to (const double **) is
      required.
   -> int Nrow
      Number of rows in the matrix
   -> int Ncol
      Number of elements in each row (number of columns)

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.18 $  $Date: 2003/05/09 01:45:58 $

-------------------------------------------------------------------------*/

#include <string.h>

#include <libtsp.h>

#define NVLINE		5	/* Number of values to a line */
#define NCSTAGGER	2	/* No. chars to stagger continuation lines */
#define MAXSTAGGER	10	/* Max. chars to stagger continuation lines */


void
MAdPrint (FILE *fp, const char Header[], const double *A[], int Nrow, int Ncol)

{
  int i, j, ns, jl;

  if (fp != NULL) {

    if (strlen (Header) > 0)
      fprintf (fp, "%s\n", Header);

    for (i = 0; i < Nrow; ++i) {
      jl = 0;
      for (j = 0; j < Ncol; ++j) {
	fprintf (fp, "%13.5g", A[i][j]);
	if (j > 0 && j < Ncol - 1 && (j + 1) % NVLINE == 0) {
	  /* Stagger continuation lines for a row */
	  jl = jl + 1;
	  ns = (jl * NCSTAGGER) % MAXSTAGGER;
	  fprintf (fp, "%-*s", ns + 1, "\n");
	}
      }
      fprintf (fp, "\n");
    }
  }
  return ;
}
