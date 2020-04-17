/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int FIdReadFilt (const char Fname[], int MaxNcof, double h[], int *Ncof,
		   FILE *fpinfo)

Purpose:
  Read a filter coefficient file

Description:
  This procedure reads filter coefficients from a filter coefficient file.  The
  first line in the file indicates the type of filter.
    !FIR	- FIR filter, direct form
    !IIR	- IIR filter, cascade of biquad sections
    !ALL	- All-pole filter, direct form
    !WIN	- Window coefficients, direct form
    !CAS	- Cascade analog biquad sections
  Subsequent lines contain filter coefficients in text form.  Data fields
  are free format, with data values separated by white-space (as defined by
  isspace).  Zero or more data values can appear in each line of input.  Commas
  can also be used to separate data values, but only within a line, i.e. a
  comma should not appear at the end of a line.  A "!" character marks the
  beginning of a comment that extends to the end of the line.

  This routine prints an error message and halts execution on detection of an
  error.

Parameters:
  <-  int FIdReadFilt
      Filter type coded as follows,
        FI_UNDEF = 0, undefined filter file identifier
        FI_FIR   = 1, FIR filter, direct form
        FI_IIR   = 2, IIR filter, cascade of biquad sections
        FI_ALL   = 3, All-pole filter, direct form
        FI_WIN   = 4, Window coefficients, direct form
        FI_CAS   = 5  Cascade analog biquad sections
   -> const char Fname[]
      Filter file name
   -> int MaxNcof
      Maximum number of coefficients to be returned
  <-  double h[]
      Array of Ncof output filter coefficients
  <-  int *Ncof
      Number of filter coefficients returned
   -> FILE *fpinfo
      File pointer for printing filter file information.  If fpinfo is not
      NULL, information about the filter file is printed on the stream
      selected by fpinfo.

Author / revision:
  P. Kabal  Copyright (C) 2005
  $Revision: 1.1 $  $Date: 2005/02/01 01:21:13 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/FIpar.h>
#include <libtsp/FImsg.h>

#define COMMENT_CHAR	'!'
#define CHECKSYM(x,N)	((int) (1.00001 * VRdCorSym(x,N)))

/* Keyword templates for the filter types */
static const char *FItab[] = {
  "!FIR**",	/* FIR filter, direct form */
  "!IIR**",	/* IIR filter, cascade of biquad sections */
  "!ALL**",	/* All-pole filter, direct form */
  "!WIN**",	/* Window coefficients, direct form */
  "!CAS**",	/* Cascade analog biquad sections */
  NULL
};

/* String descriptions of the filter types */
static const char *FItype[] = {
  FI_UNDEF_desc,
  FI_FIR_desc,
  FI_IIR_desc,
  FI_ALL_desc,
  FI_WIN_desc,
  FI_CAS_desc
};


int
FIdReadFilt (const char Fname[], int MaxNcof, double h[], int *Ncof,
             FILE *fpinfo)

{
  FILE *fp;
  char *line;
  int N, FiltType, Sym;
  char FullName[FILENAME_MAX];

/* Open the filter coefficient file */
  fp = fopen (Fname, "r");
  if (fp == NULL)
    UTerror ("FIdReadFilt: %s: \"%s\"", FIM_OpenRErr, Fname);

/* Determine the filter type */
  FiltType = FI_UNDEF;
  line = FLgetLine (fp);
  if (line != NULL)
    FiltType = STkeyMatch (line, FItab) + 1;
  rewind (fp);

/* Decode data records */
  N = FLdReadTF (fp, MaxNcof, COMMENT_CHAR, h);

/* Error checks */
  if (FiltType == FI_IIR) {
    if (N % 5 != 0)
      UThalt ("FIdReadFilt: %s", FIM_IIRNCoefErr);
  }

/* Print filter file information */
  if (fpinfo != NULL) {
    FLfullName (Fname, FullName);
    fprintf (fpinfo, FIMF_FiltFile, FullName);

    if (FiltType == FI_FIR || FiltType == FI_WIN || FiltType == FI_ALL) {
      Sym = CHECKSYM (h, N);
      if (Sym == 1)
	fprintf (fpinfo, FIMF_FiltSubType, FItype[FiltType],
		 FIM_FiltSym, FLfileDate(fp, 3));
      else if (Sym == -1)
	fprintf (fpinfo, FIMF_FiltSubType, FItype[FiltType],
		 FIM_FiltAsym, FLfileDate(fp, 3));
      else
	fprintf (fpinfo, FIMF_FiltSubType, FItype[FiltType],
		 FIM_FiltDirect, FLfileDate(fp, 3));
    }
    else if (FiltType == FI_IIR || FiltType == FI_CAS)
	fprintf (fpinfo, FIMF_FiltSubType, FItype[FiltType],
		 FIM_FiltBiquad, FLfileDate(fp, 3));
    else
      	fprintf (fpinfo, FIMF_FiltType, FItype[FiltType], FLfileDate(fp, 3));

    if (FiltType == FI_IIR) {
      if (N == 5)
	fprintf (fpinfo, FIMF_NumIIR1, N);
      else
	fprintf (fpinfo, FIMF_NumIIRN, N, N / 5);
    }
    else
      fprintf (fpinfo, FIMF_NumCoef, N);
  }

/* Close the file */
  fclose (fp);

/* Return the values */
  *Ncof = N;
  return FiltType;
}
