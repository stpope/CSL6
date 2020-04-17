/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFsetFileType (const char String[])

Purpose:
  Set the input audio file type

Description:
  This routine sets the audio file type.  This file type is checked when
  reading audio files.  Explicitly setting the audio file type bypasses the
  need for a check for file type from the file header.
    "auto"      - determine the input file type from the file header
    "AU" or "au" - AU audio file
    "WAVE" or "wave" - WAVE file
    "AIFF" or "aiff" - AIFF or AIFF-C sound file
    "noheader"  - headerless (non-standard or no header) audio file
    "SPHERE"    - NIST SPHERE audio file
    "ESPS"      - ESPS sampled data feature file
    "IRCAM"     - IRCAM soundfile
    "SPPACK"    - SPPACK file
    "INRS"      - INRS-Telecom audio file
    "SPW"       - Comdisco SPW Signal file
    "CSL" or "NSP" - CSL NSP file
    "text"      - Text audio file

  If the input string contains has a leading '$', the string is assumed to
  specify the name of an environment variable after the '$'.  This routine
  uses the value of this environment variable to determine the parameters.
  For instance, if this routine is called as AFsetFileType("$AF_FILETYPE"),
  this routine would look for the parameter string in environment variable
  AF_FILETYPE.

Parameters:
  <-  int AFsetFileType
      Error code, zero for no error
   -> const char String[]
      String with the input file type

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.16 $  $Date: 2003/05/09 01:03:46 $

-------------------------------------------------------------------------*/

#include <stdlib.h>	/* getenv prototype */

#include <libtsp.h>
#include <libtsp/AFpar.h>
#include <libtsp/AFmsg.h>

static const char *Ftype_keys[] = {
  "auto",
  "AU", "au", "AFsp", "Sun", "sun",
  "WAVE", "wave",
  "AIFF*-C", "aiff*-c",
  "nohead*er",
  "SPHERE",
  "ESPS",
  "IRCAM",
  "SPPACK",
  "INRS",
  "SPW",
  "CSL", "NSP",
  "text",
  NULL
};

/* Values corresponding to keywords */
static const int Ftype_vals[] = {
  FT_AUTO,
  FT_AU, FT_AU, FT_AU, FT_AU, FT_AU,
  FT_WAVE, FT_WAVE,
  FT_AIFF, FT_AIFF,
  FT_NH,
  FT_SPHERE,
  FT_ESPS,
  FT_SF,
  FT_SPPACK,
  FT_INRS,
  FT_SPW,
  FT_NSP, FT_NSP,
  FT_TXAUD
};


int
AFsetFileType (const char String[])

{
  int n, ErrCode;
  const char *p;
  struct AF_opt *AFopt;

/* Check for an environment variable */
  if (String[0] == '$') {
    p = getenv (&String[1]);
    if (p == NULL)
      p = "";
  }
  else
    p = String;

  ErrCode = 0;
  if (p[0] != '\0') {
    n = STkeyMatch (p, Ftype_keys);
    if (n < 0) {
      UTwarn ("AFsetFileType - %s, \"%.20s\"", AFM_BadFType, p);
      ErrCode = 1;
    }
    else {
      AFopt = AFoptions ();
      AFopt->FtypeI = Ftype_vals[n];
    }
  }

  return ErrCode;
}
