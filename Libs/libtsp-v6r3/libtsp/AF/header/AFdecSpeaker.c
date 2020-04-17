/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFdecSpeaker (const char String[], unsigned char *SpkrConfig, int MaxN)
  int AFcheckSpeakers (const unsigned char *SpkrConfig);
  int AFspeakerNames (const unsigned char *SpkrConfig, char *SpkrNames,
                      int MaxNc)

Purpose:
  Decode, check, list loudspeaker spacial positions

Description:
  This routine sets the default mapping of output channels to speaker
  positions.  The spacial positions of the loudspeakers are specified as a
  list of comma and/or white-space separated locations from the list below.
    "FL"  - Front Left
    "FR"  - Front Right
    "FC"  - Front Center
    "LF"  - Low Frequency
    "BL"  - Back Left
    "BR"  - Back Right
    "FLC" - Front Left of Center
    "FRC" - Front Right of Center
    "BC"  - Back Center
    "SL"  - Side Left
    "SR"  - Side Right
    "TC"  - Top Center
    "TFL" - Top Front Left
    "TFC" - Top Front Center
    "TFR" - Top Front Right
    "TBL" - Top Back Lefty
    "TBC" - Top Back Center
    "TBR" - Top Back Right
    "-"   - none
  A speaker position can be associated with only one channel.  Only WAVE files
  store the speaker locations in the file header.  In the case of WAVE files,
  the subset of spacial positions must appear in the order given above.

Parameters:
  <-  int AFdecSpeaker
      Error flag, zero for no error
   -> const char String[]
      String containing the list of speaker positions
  <-  unsigned char *SpkrConfig
      Null-terminated string containing the speaker location codes
   -> int MaxN
      Maximum number of speaker locations

  <-  int AFspeakerNames
      Error flag, zero for no error
   -> int Nchan
      Number of channels
   -> const unsigned char *SpkrConfig
      Null-terminated string containing the speaker location codes
   -> int Nextra
      Maximum number of extra (not-specified) speaker positions
  <-  char *SpkrNames
      String containing the list of speaker positions

  <-  int AFcheckSpeakers
      Error flag, zero for no error
   -> const unsigned char *SpkrConfig
      Null-terminated string containing the speaker location codes

Author / revision:
  P. Kabal  Copyright (C) 2004
  $Revision: 1.8 $  $Date: 2004/03/31 13:25:41 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include <libtsp.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFmsg.h>
#define AF_SPKR_NAMES
#include <libtsp/AFpar.h>

#define NCBUF           512


int
AFdecSpeaker(const char String[], unsigned char *SpkrConfig, int MaxN)

{
  char cbuf[NCBUF+1];
  const char *p;
  char *token;
  int n, k, nc, ErrCode;

  p = String;

/* Set up the token buffer */
  nc = strlen (p);
  if (nc <= NCBUF)
    token = cbuf;
  else
    token = (char *) UTmalloc (nc + 1);

  ErrCode = 0;
  k = 0;
  while (p != NULL && k < MaxN) {
    p = STfindToken (p, ",", "", token, 2, nc);
    n = STkeyMatch (token, AF_Spkr_Names);
    if (n < 0) {
      ErrCode = 1;
      UTwarn ("AFdecSpeaker - %s: \"%.10s\"", AFM_BadSpkr, token);
      break;
    }
    SpkrConfig[k] = n+1;
    ++k;
  }

  if (k == MaxN) {
    ErrCode = 1;
    UTwarn ("AFdecSpeaker - %s", AFM_XSpkr);
    k = 0;
  }
  SpkrConfig[k] = '\0';

  /* Check for duplicate speakers */
  if (! ErrCode)
    ErrCode = AFcheckSpeakers (SpkrConfig);

/* Deallocate the buffer */
  if (nc > NCBUF)
    UTfree ((void *) token);

  if (ErrCode)
    SpkrConfig[0] = '\0';

  return ErrCode;
}

/* Check for duplicate speaker names */


int
AFcheckSpeakers (const unsigned char *SpkrConfig)

{
  int ErrCode, Nspkr, i, n;
  unsigned char SpkrCode[AF_N_SPKR_NAMES];

  ErrCode = 0;
  if (SpkrConfig == NULL || SpkrConfig[0] == AF_X_SPKR)
    Nspkr = 0;
  else
    Nspkr = strlen ((const char *) SpkrConfig);

  for (i = 0; i < AF_N_SPKR_NAMES; ++i)
    SpkrCode[i] = 0;

  for (i = 0; i < Nspkr; ++i) {
    n = SpkrConfig[i];
    assert (n >= 1 && n <= AF_N_SPKR_NAMES);
    if (n != AF_SPKR_X) {
      if (SpkrCode[n-1] > 0) {
	ErrCode = 1;
	UTwarn ("AFcheckSpeakers - %s: \"%.10s\"", AFM_DupSpkr,
		AF_Spkr_Names[n-1]);
	break;
      }
    }
    ++SpkrCode[n-1];
  }

  return ErrCode;
}

/* Speaker configuration names */
/* Let Nspkr be the number of locations specified in SpkrConfig. The number of
   of speaker locations returned in SpkrNames is
     if (Nchan <= Nspkr+Nextra)
       Ns = Nchan
     else
       Ns = Nspkr
   The "extra" speaker locations are designated "-".
   SpkrNames must provides space for Ns * AF_NC_SPKR - 1 characters.
*/


int
AFspeakerNames (int Nchan, const unsigned char *SpkrConfig, int Nextra,
		char *SpkrNames)

{
  int ErrCode;
  int i, Nspkr, Ns, nc, n;

  ErrCode = 0;

  SpkrNames[0] = '\0';
  if (SpkrConfig == NULL || SpkrConfig[0] == AF_X_SPKR ||
      SpkrConfig[0] == '\0')
    return ErrCode;		/* Quick bypass */

  /* Check for duplicate speakers */
  ErrCode = AFcheckSpeakers (SpkrConfig);
  if (ErrCode)
    return ErrCode;

  Nspkr = strlen ((const char *) SpkrConfig);
  if (Nchan <= Nspkr + Nextra)
    Ns = Nchan;
  else
    Ns = Nspkr;

  SpkrNames[0] = '\0';
  for (i = 0; i < Ns; ++i) {

    /* Append the speaker name */
    nc = strlen (SpkrNames);
    if (nc > 0) {
      SpkrNames[nc] = ' ';
      nc = nc + 1;
    }
    if (i >= Nspkr)
      n = AF_SPKR_X;
    else
      n = SpkrConfig[i];
    strcpy (&SpkrNames[nc], AF_Spkr_Names[n-1]);
  }

  if (ErrCode)
    SpkrNames[0] = '\0';

  return ErrCode;
}
