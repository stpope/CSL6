/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFsetNHpar (const char String[])

Purpose:
  Set defaults for input headerless audio files from a string specification

Description:
  This routine sets the default audio file data parameters.  These parameters
  are used for reading audio files with unrecognized (non-standard) headers
  or files with no headers (raw audio files).  This routine must be called
  before opening the file with AFopnRead.  The parameters for AFsetNHpar are
  determined from an input string which consists of a list of parameters
  separated by commas and/or white space.  The form of the list is
    "Format, Start, Sfreq, Swapb, Nchan, ScaleF"
  Format: File data format
       "undefined" - Headerless files will be rejected
       "mu-law8"   - 8-bit mu-law data
       "A-law8"    - 8-bit A-law data
       "unsigned8" - offset-binary 8-bit integer data
       "integer8"  - two's-complement 8-bit integer data
       "integer16" - two's-complement 16-bit integer data
       "integer24" - two's-complement 24-bit integer data
       "integer32" - two's-complement 32-bit integer data
       "float32"   - 32-bit floating-point data
       "float64"   - 64-bit floating-point data
       "text"      - text data
  Start: byte offset to the start of data (integer value)
  Sfreq: sampling frequency in Hz (floating-point number)
  Swapb: Data byte swap parameter
       "native" - no byte swapping
       "little-endian" - file data is in little-endian byte order and
          will be swapped if the current host uses big-endian byte order
       "big-endian" - data is in big-endian byte order and will be
          swapped swapped if the current host uses little-endian byte
          order
       "swap" - swap the data bytes
  Nchan: number of channels
    The data consists of interleaved samples from Nchan channels
  ScaleF: Scale factor
       "default" - Scale factor chosen appropriate to the type of data. The
          scaling factors shown below are applied to the data in the file.
          8-bit mu-law:    1
          8-bit A-law:     1
          8-bit integer:   128
          16-bit integer:  1
          24-bit integer:  1/256
          32-bit integer:  1/65536
          float data:      32768
       "<number or ratio>" - Specify the scale factor to be applied to
          the data from the file.
  The default values for the audio file parameters correspond to the following
  string.
    "undefined, 0, 8000., native, 1, default"

  Leading and trailing white-space (as defined by isspace) is removed from
  each item.  Any of the parameters may be omitted, in which case whatever
  value has been previously set remains in effect for that parameter.  The
  string ",512, 10000." would set the Start and Sfreq parameters and leave
  the other parameters undisturbed.

  If the input string contains has a leading '$', the string is assumed to
  specify the name of an environment variable after the '$'.  This routine uses
  the value of this environment variable to determine the parameters.  If this
  routine is called as AFsetNHpar("$AF_NOHEADER"), this routine would look for
  the parameter string in environment variable AF_NOHEADER.

Parameters:
  <-  int AFsetNHpar
      Error flag, zero for no error
   -> const char String[]
      String containing the list of parameters for headerless files or the name
      of an environment variable (with a leading $)

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.53 $  $Date: 2003/05/09 01:03:46 $

-------------------------------------------------------------------------*/

#include <stdlib.h>	/* getenv prototype */
#include <string.h>

#include <libtsp.h>
#include <libtsp/AFmsg.h>
#include <libtsp/AFpar.h>

#define N_NHPAR		6
#define NCBUF           512

/* Keyword tables */
static const char *Format_keys[NFD+1] = {
  "u*ndefined",
  "m*u-law8",
  "A-l*aw8",
  "u*nsigned8",
  "integer8",
  "i*nteger16",
  "integer24",
  "integer32",
  "f*loat32",
  "float64",
  "t*ext",
  NULL
};
static const char *Swapb_keys[] = {
  "n*ative", "l*ittle-endian", "b*ig-endian", "s*wap", NULL };
static const char *Def_key[] = {
  "def*ault", NULL};

/* Values corresponding to keywords */
static const int Format_values[NFD] = {
  FD_UNDEF,
  FD_MULAW8, FD_ALAW8, FD_UINT8, FD_INT8, FD_INT16, FD_INT24, FD_INT32,
  FD_FLOAT32, FD_FLOAT64, FD_TEXT
};
static const int Swapb_values[] = {
  DS_NATIVE, DS_EL, DS_EB, DS_SWAP };


int
AFsetNHpar (const char String[])

{
  char cbuf[NCBUF+1];
  int n, k, nc, ErrCode;
  const char *p;
  char *token;
  double DN, DD;
  struct AF_opt *AFopt;
  struct AF_NHpar AFNH;

/* Copy the current values */
  AFopt = AFoptions ();
  AFNH = AFopt->NHpar;

/* Check for an environment variable */
  if (String[0] == '$') {
    p = getenv (&String[1]);
    if (p == NULL)
      p = "";
  }
  else
    p = String;

/* Set up the token buffer */
  nc = strlen (p);
  if (nc <= NCBUF)
    token = cbuf;
  else
    token = (char *) UTmalloc (nc + 1);

/* Separate the parameters */
  ErrCode = 0;
  for (k = 0; k < N_NHPAR && p != NULL; ++k) {
    p = STfindToken (p, ",", "", token, 2, nc);
    if (token[0] != '\0') {

      /* Decode the parameter values */
      switch (k) {
      case 0:
	n = STkeyMatch (token, Format_keys);
	if (n < 0) {
	  UTwarn ("AFsetNHpar - %s: \"%.10s\"", AFM_BadFormat, token);
	  ErrCode = 1;
	}
	else
	  AFNH.Format = Format_values[n];
	break;
      case 1:
	if (STdec1long (token, &AFNH.Start) || AFNH.Start < 0L) {
	  UTwarn ("AFsetNHpar - %s: \"%.10s\"", AFM_BadStart, token);
	  ErrCode = 1;
	}
	break;
      case 2:
	if (STdec1double (token, &AFNH.Sfreq) || AFNH.Sfreq < 0.0) {
	  UTwarn ("AFsetNHpar - %s: \"%.10s\"", AFM_BadSFreq, token);
	  ErrCode = 1;
	}
	break;
      case 3:
	n = STkeyMatch (token, Swapb_keys);
	if (n < 0) {
	  UTwarn ("AFsetNHpar - %s: \"%.10s\"", AFM_BadSwap, token);
	  ErrCode = 1;
	}
	else
	  AFNH.Swapb = Swapb_values[n];
	break;
      case 4:
	if (STdec1long (token, &AFNH.Nchan) || AFNH.Nchan <= 0L) {
	  UTwarn ("AFsetNHpar - %s: \"%.10s\"", AFM_BadNChan, token);
	  ErrCode = 1;
	}
	break;
      case 5:
	if (STkeyMatch (token, Def_key) == 0)
	  AFNH.ScaleF = AF_SF_DEFAULT;
	else {
	  if (STdecDfrac (token, &DN, &DD)) {
	    UTwarn ("AFsetNHpar - %s: \"%.10s\"", AFM_BadSFactor, token);
	    ErrCode = 1;
	  }
	  else
	    AFNH.ScaleF = DN / DD;
	}
	break;
      }
    }
  }
  if (p != NULL) {
    UTwarn ("AFsetNHpar - %s", AFM_TooManyPar);
    ErrCode = 1;
  }

/* Deallocate the buffer */
  if (nc > NCBUF)
    UTfree ((void *) token);

/* Set the parameters */
  if (! ErrCode)
    AFopt->NHpar = AFNH;

  return ErrCode;
}
