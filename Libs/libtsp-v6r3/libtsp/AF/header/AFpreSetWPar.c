/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AF_write AFpreSetWPar (int Ftype, int Dformat, long int Nchan,
                             double Sfreq, struct AF_write *AFw)

Purpose:
  Create a parameter structure for writing an audio file

Description:
  This routine creates a parameter structure for writing to an audio file.  The
  The structure is filled in with default values, values supplied as input, or
  user supplied options.

  Fields set from the input values:
    AFw->DFormat.Format
    AFw->Sfreq
    AFw->Nchan
    AFw->Ftype
  Fields set from options:
    AFw->DFormat.NbS
    AFw->SpkrConfig
    AFw->InfoX
    AFw->Nframe
  Fields preset to tentative values
    AFw->DFormat.ScaleF
    AFw->DFormat.Swapb

Parameters:
  <-  int AFpreSetWPar
      Error flag, 0 for no error
   -> int Ftype
      File type code
   -> int Dformat
      Data format code
   -> long int Nchan
      Number of channels
   -> double Sfreq
      Sampling frequency
  <-  struct AF_write *AFw
      Pointer to an audio write structure
 
Author / revision:
  P. Kabal  Copyright (C) 2006
  $Revision: 1.20 $  $Date: 2006/06/06 13:55:37 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <math.h>	/* floor */
#include <string.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFmsg.h>
#define AF_DATA_TYPE_NAMES
#define AF_DATA_LENGTHS
#include <libtsp/AFpar.h>

#define ICEILV(n,m)	(((n) + ((m) - 1)) / (m))	/* int n,m >= 0 */

static int
AF_checkDT (int Ftype, int Format);
static int
AF_checkIEEE (int Ftype, int Format);
static int
AF_setNbS (int Format, int NbS);
static void
AF_setSpeaker (const unsigned char *SpkrX, int Nchan,
	       unsigned char *SpkrConfig);
static struct AF_infoX
AF_genInfo (const char *Uinfo, const struct AF_write *AFw);
static int
AF_stdInfo (const struct AF_write *AFw, char *Sinfo);
static int
AF_nlNull (const char *InfoX, char *Info);


int
AFpreSetWPar (int Ftype, int Dformat, long int Nchan, double Sfreq,
	      struct AF_write *AFw)

{
  struct AF_opt *AFopt;

  AFopt = AFoptions ();

/* File type and data format */
  AFw->Ftype = Ftype;
  AFw->DFormat.Format = Dformat;

/* Set file data parameters */
  AFw->Sfreq = Sfreq;
  AFw->DFormat.ScaleF = AF_SF_DEFAULT;
  AFw->DFormat.Swapb = DS_NATIVE;		    /* Tentative */
  AFw->DFormat.NbS = AF_setNbS (AFw->DFormat.Format, AFopt->NbS);
  AFw->Nchan = Nchan;
  AFw->Nframe = AFopt->Nframe;	/* From the options structure */
  AF_setSpeaker (AFopt->SpkrConfig, Nchan, AFw->SpkrConfig);

/* Info string:
     The space for the info string is allocated within AF_genInfo.  It is
     reclaimed each time AF_genInfo is called.
  */
  AFw->InfoX = AF_genInfo (AFopt->Uinfo, AFw);

/* Error Checks */
  if (AF_checkDT (AFw->Ftype, AFw->DFormat.Format))
    return 1;
  if (AF_checkIEEE (AFw->Ftype, AFw->DFormat.Format))
    return 1;

  if (AFw->Nchan <= 0L) {
    UTwarn ("AFopnWrite - %s: %ld", AFM_BadNChan, AFw->Nchan);
    return 1;
  }
  if (AFw->Nframe != AF_NFRAME_UNDEF && AFw->Nframe < 0L) {
    UTwarn ("AFopnWrite - %s: %ld", AFM_BadNFrame, AFw->Nframe);
    return 1;
  }

  return 0;
}

/* Check for file type / data type compatibility */


static int
AF_checkDT (int Ftype, int Format)

{
  int ErrCode;

/* undef mulaw  Alaw uint8  int8 int16 int24 int32 flt32 flt64 text */
  static const int DT_AU[NFD] = {
     0,    1,    1,    0,    1,    1,    1,    1,    1,    1,    0};
  static const int DT_WAVE[NFD] = {
     0,    1,    1,    1,    0,    1,    1,    1,    1,    1,    0};
  static const int DT_AIFF_C[NFD] = {
     0,    1,    1,    0,    1,    1,    1,    1,    1,    1,    0};
  static const int DT_AIFF[NFD] = {
     0,    0,    0,    0,    1,    1,    1,    1,    0,    0,    0};
  static const int DT_NH[NFD] = {
     0,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1};

  ErrCode = 0;
  if (Format < 1 || Format >= NFD) {
    UTwarn ("AFopnWrite - %s: \"%d\"", AFM_BadDataC, Format);
    ErrCode = 1;
    return ErrCode;
  }

  switch (Ftype) {
  case FTW_AU:
    if (DT_AU[Format] == 0) {
      UTwarn ("AFopnWrite - %s: \"%s\"", AFM_AU_UnsData, AF_DTN[Format]);
      ErrCode = 1;
    }
    break;
  case FTW_WAVE:
  case FTW_WAVE_NOEX:
    if (DT_WAVE[Format] == 0) {
      UTwarn ("AFopnWrite - %s: \"%s\"", AFM_WV_UnsData, AF_DTN[Format]);
      ErrCode = 1;
    }
    break;
  case FTW_AIFF_C:
    if (DT_AIFF_C[Format] == 0) {
      UTwarn ("AFopnWrite - %s: \"%s\"", AFM_AIFF_UnsData, AF_DTN[Format]);
      ErrCode = 1;
    }
    break;
  case FTW_AIFF:
    if (DT_AIFF[Format] == 0) {
      UTwarn ("AFopnWrite - %s: \"%s\"", AFM_AIFF_UnsData, AF_DTN[Format]);
      ErrCode = 1;
    }
    break;
  case FTW_NH_EB:
  case FTW_NH_EL:
  case FTW_NH_NATIVE:
  case FTW_NH_SWAP:
    if (DT_NH[Format] == 0) {
      UTwarn ("AFopnWrite - %s: \"%s\"", AFM_NH_UnsData, AF_DTN[Format]);
      ErrCode = 1;
    }
    break;
  default:
    UTwarn ("AFopnWrite - %s: %d", AFM_BadFTypeC, Ftype);
    ErrCode = 1;
    break;
  }
  return ErrCode;
}

/* Check for IEEE data format */

static int
AF_checkIEEE (int Ftype, int Format)

{
  int ErrCode;

  ErrCode = 0;
  switch (Ftype) {
  case FTW_AU:
    if ((Format == FD_FLOAT32 || Format == FD_FLOAT64) && ! UTcheckIEEE ()) {
      UTwarn ("AFwrAUhead - %s", AFM_NoIEEE);
      ErrCode = 1;
    }
    break;
  case FTW_WAVE:
  case FTW_WAVE_NOEX:
    if ((Format == FD_FLOAT32 || Format == FD_FLOAT64) && ! UTcheckIEEE ()) {
      UTwarn ("AFwrWVhead - %s", AFM_NoIEEE);
      ErrCode = 1;
    }
    break;
  case FTW_AIFF_C:
  case FTW_AIFF:
    if ((Format == FD_FLOAT32 || Format == FD_FLOAT64) && ! UTcheckIEEE ()) {
      UTwarn ("AFwrAIhead - %s", AFM_NoIEEE);
      ErrCode = 1;
    }
    break;
  default:
    break;
  }

  return ErrCode;
}

/* Set the number of bits per sample */


static int
AF_setNbS (int Format, int NbS)

{
  int Res;

  Res = 8 * AF_DL[Format];
  if (NbS == 0)
    NbS = Res;

  /* Res == 0 for text files */
  if (Res > 0 && (NbS <= 0 || NbS > Res)) {
    UTwarn (AFMF_InvNbS, "AFopnWrite -", NbS, Res);
    NbS = Res;
  }
  else if (Res == 0 && NbS != 0) {
    UTwarn ("AFopnWrite - %s", AFM_BadNbS);
    NbS = Res;
  }

  /* Warn and reset NbS for inappropriate formats */
  if (NbS != Res &&
      ! (Format == FD_UINT8 || Format == FD_INT8 || Format == FD_INT16 ||
	 Format == FD_INT24 || Format == FD_INT32)) {
    UTwarn ("AFopnWrite - %s", AFM_InaNbS);
    NbS = Res;
  }

  return NbS;
}

/* Set the speaker configuration */


static void
AF_setSpeaker (const unsigned char *SpkrX, int Nchan,
	       unsigned char *SpkrConfig)

{
  int Nspkr;

  SpkrConfig[0] = AF_X_SPKR;
  if (SpkrX != NULL) {
    SpkrConfig[0] = '\0';
    Nspkr = strlen ((const char *) SpkrX);
    if (Nspkr > Nchan)
      Nspkr = Nchan;
    if (Nspkr > AF_MAXN_SPKR)
      UTwarn ("AFopnWrite - %s", AFM_XSpkr);
    else {
      strncpy ((char *) SpkrConfig, (const char *) SpkrX, Nspkr);
      SpkrConfig[Nspkr] = '\0';
    }
  }

  return;
}

#define MAX_SINFO	320


static struct AF_infoX
AF_genInfo (const char *Uinfo, const struct AF_write *AFw)

{
  char Sinfo[MAX_SINFO];
  int Nc, ns, nu;
  static struct AF_infoX InfoX = {NULL, 0, 0};

  if (Uinfo == NULL || Uinfo[0] == '\n' ||
      (Uinfo[0] == '\\' && Uinfo[1] == 'n'))
    ns = AF_stdInfo (AFw, Sinfo);
  else {
    ns = 0;
    Sinfo[0] = '\0';
  }

/* Size of the user specified information */
  if (Uinfo != NULL)
    nu = strlen (Uinfo);
  else
    nu = 0;

/* Allocate storage */
  Nc = nu + ns + 1;	/* Leave room for a null */
  UTfree ((void *) InfoX.Info);
  InfoX.Info = (char *) UTmalloc (Nc);
  InfoX.Nmax = Nc;

/* Form the output string.
   - Uinfo == NULL
       Info <= Sinfo
   - Uinfo empty ("")
       Info <= empty
   - Uinfo starts with '\n'
       Info <= Sinfo + Uinfo
       In this case, the leading '\n' in Uinfo overlays the trailing null
       in Sinfo.
   - Uinfo does not start with '\n'
       Info <= Uinfo
*/
  strcpy (InfoX.Info, Sinfo);
  if (Uinfo != NULL)
    strcpy (&InfoX.Info[ns], Uinfo);

/* Change newlines to nulls */
  InfoX.N = AF_nlNull (InfoX.Info, InfoX.Info);

/* Return a pointer to the information records */
  return InfoX;
}

/* Generate the standard information string */
/* "date: xxx"
   + "\n" + "sample_rate: xxx" (only if non-integer)
   + "\n" + "user: xxx"
   + "\n" + "bits_per_sample: xx" (only if not equal to full precision)
   + "\n" + "loudspeakers: xxx" (only if specified)

   Lines are separated by newline characters.  The overall string is null
   terminated.  The returned function value is the number of characters not
   including the terminating null.
*/

#define NSPKR_EXTRA 0
#define NC_SPKR ((AF_NC_SPKR + 1) * (AF_MAXN_SPKR + NSPKR_EXTRA) - 1)

static int
AF_stdInfo (const struct AF_write *AFw, char *Sinfo)

{
  int N, NbS, Res;
  char *p;
  double Sfreq;
  const unsigned char *SpkrConfig;
  char SpkrNames[NC_SPKR+1];

/* String lengths
  Record          Fixed    Variable
  Name            Length   Max       Total
   Date             6        40       46
   Program Name    10        40       50
   Sampling Freq.  14        15       29
   Number of Bits  19         4       23
   Loudspeakers    15        76       91
   null             1                  1
                                     ---
                                     240
*/
  N = sprintf (Sinfo, "date: %.40s", UTdate(3));

  p = UTgetProg ();
  if (*p != '\0')
    N += sprintf (&Sinfo[N], "\nprogram: %.40s", p);

  Sfreq = AFw->Sfreq;
  if (Sfreq > 0.0 && Sfreq != floor (Sfreq))
    N += sprintf (&Sinfo[N], "\nsample_rate: %.7g", Sfreq);

  Res = 8 * AF_DL[AFw->DFormat.Format];
  NbS = AFw->DFormat.NbS;
  if (NbS != 0 && NbS != Res)
    N += sprintf (&Sinfo[N], "\nbits_per_sample: %d/%d", NbS, Res);

  SpkrConfig = AFw->SpkrConfig;
  if (SpkrConfig[0] != AF_X_SPKR) {
    AFspeakerNames (AFw->Nchan, SpkrConfig, NSPKR_EXTRA, SpkrNames);
    N += sprintf (&Sinfo[N], "\nloudspeakers: %s", SpkrNames);
  }

  assert (N < MAX_SINFO);

  return N;
}

/* Change newlines to nulls (unless escaped) */
/* - Escaped characters.  In Unix it is relatively easy to get a newline
     into a command line string.  In Windows, one can use the escape
     sequences instead.
     - <nl>  => <nul>  newline
     - \n    => <nul>  newline
     - \r    => <nl>   escaped newline
     - <cr>  => <nl>   escaped newline
     - \<nl> => <nl>   escaped newline
   - If the length of the input string is not zero and the last character in
     the input string was not a newline (changed to a null), a null character
     is added to the string.
   - The count of the number of characters includes the null character at the
     end of the string.
   - If the number of input characters is zero, the returned value is zero.
*/


static int
AF_nlNull (const char *InfoX, char *Info)

{
  int i, n;

  for (i = 0, n = 0; InfoX[i] != '\0'; ++i, ++n) {
    if (InfoX[i] == '\n')
      Info[n] = '\0';
    else if (InfoX[i] == '\\' && InfoX[i+1] == 'n') {
      Info[n] = '\0';
      ++i;
    }
    else if (InfoX[i] == '\\' && InfoX[i+1] == 'r') {
      Info[n] = '\n';
      ++i;
    }
    else if (InfoX[i] == '\r')
      Info[n] = '\n';
    else if (InfoX[i] == '\\' && InfoX[i+1] == '\n') {
      Info[n] = '\n';
      ++i;
    }
    else
      Info[n] = InfoX[i];
  }

  /* Add a terminating null (included in the character count) if one is not
     present */
  if (n > 0 && Info[n-1] != '\0') {
    Info[n] = '\0';
    ++n;
  }

  return n;
}
