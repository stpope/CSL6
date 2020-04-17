/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  AFILE *AFsetRead (FILE *fp, int Ftype, const struct AF_read *AFr, int Fix)

Purpose:
  Set up the parameters in an audio file structure for reading

Description:
  This routine checks the input parameters and puts them into a new audio file
  structure.  The data length (Ldata) and number of samples (Nsamp) parameters
  are checked for consistency and modified if necessary.

  The data length is the size of the area (in bytes) in the file reserved for
  for storing the data.  The data need not fully occupy this area.  If Ldata
  is undefined (equal to AF_LDATA_UNDEF), it is set to the size of the file,
  less the size of the header.  Ldata is checked against the actual size of
  the file.  It is an error for this value to exceed the actual space in the
  file.

  The number of samples Nsamp is used to calculate the actual amount of space
  space occupied by the data (Nsamp is multiplied by the data element size).
  If this value exceeds the space available, this is an error.  If Nsamp is
  undefined (equal to AF_NSAMP_UNDEF), it is determined from Ldata, setting it
  equal to the integer number of samples that fit into Ldata.  If Nsamp is so
  determined, a warning message is issued if Ldata is not a multiple of the
  data element size.  A warning message is also printed if Nsamp is not a
  multiple of the number of channels.

  For certain file types, the Nsamp value needs to be "fixed".  Optional fixes
  can be specified for the case that Nsamp is larger or smaller than the data
  space available in the file.  The latter case is not normally an error.
  If a fixup is done, a warning message is printed.  A fixup can also be
  specified for values of Nsamp which are too large.

  If the data does not have fixed length elements (for instance, text data),
  Nsamp must be specified.  If defined, Ldata is checked against the file size.

  The file must be positioned at the start of the audio data.

Parameters:
  <-  AFILE *AFp
      Audio file pointer for the audio file.  This routine allocates the
      space for this structure.  If an error is detected, a NULL pointer is
      returned.
   -> int Ftype
      File type: FT_NH, FT_AU, FT_WAVE, etc.
   -> const struct AF_read *AFr
      Structure with data and file parameters
   -> int Fix
      Fix flag, sum of the following subcodes
         AF_FIX_NSAMP_HIGH - Reduce Nsamp if it larger than the data
                             space available
         AF_FIX_NSAMP_LOW  - Increase Nsamp if it is smaller than the
                             data space available
         AF_FIX_LDATA_HIGH - Reduce Ldata if it is larger than the data
                             space available

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.46 $  $Date: 2003/11/03 18:50:17 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <math.h>
#include <string.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFdataio.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFmsg.h>
#define AF_DATA_LENGTHS
#include <libtsp/AFpar.h>

#define ICEILV(n,m)	(((n) + ((m) - 1)) / (m))	/* int n,m >= 0 */

static int
AF_checkLdata (FILE *fp, int Lw, long int *Dstart, struct AF_ndata *NData,
	       int Fix);
int
AF_setLdata (FILE *fp, long int Dstart, struct AF_ndata *NData, int Fix);
int
AF_setNsamp (FILE *fp, int Lw, struct AF_ndata *NData, int Fix);
static int
AF_checkNsamp (int Lw, struct AF_ndata *NData, int Fix);
static long int
AF_Nrec (FILE *fp);
/* ---------- */
static struct AF_info
AF_setInfo (const struct AF_infoX *InfoX);
static double
AF_srateInfo (const struct AF_info *InfoS, double Sfreq);
static int
AF_NbSInfo (const struct AF_info *InfoS, const struct AF_dformat *DFormat);
static void
AF_spkrInfo (const struct AF_info *InfoS, unsigned char *SpkrConfig);
static unsigned char *
AF_setSpeaker (const unsigned char *SpkrX, int Nchan);


AFILE *
AFsetRead (FILE *fp, int Ftype, const struct AF_read *AFr, int Fix)

{
  AFILE *AFp;
  long int Dstart;
  int Lw;
  struct AF_ndata NData;

  assert (Ftype > 0 && Ftype < NFT);
  assert (AFr->DFormat.Format > 0 && AFr->DFormat.Format < NFD);

/* Check data consistency */
  NData = AFr->NData;
  Lw = AF_DL[AFr->DFormat.Format];
  if (AF_checkLdata (fp, Lw, &Dstart, &NData, Fix))
    return NULL;

/* Set the parameters for file access */
  AFp = (AFILE *) UTmalloc (sizeof (AFILE));
  AFp->fp = fp;
  AFp->Op = FO_RO;
  AFp->Error = AF_NOERR;
  AFp->Novld = 0;
  AFp->Ftype = Ftype;
  AFp->Format = AFr->DFormat.Format;
  AFp->Nchan = NData.Nchan;
  AFp->ScaleF = AFr->DFormat.ScaleF;
  if (AFp->ScaleF == AF_SF_DEFAULT)
    AFp->ScaleF = AF_SF[AFp->Format];
  AFp->Start = Dstart;
  AFp->Isamp = 0;
  AFp->Nsamp = NData.Nsamp;
  if (Lw <= 1)
    AFp->Swapb = DS_NATIVE;
  else
    AFp->Swapb = UTswapCode (AFr->DFormat.Swapb);

  AFp->InfoS = AF_setInfo (&AFr->InfoX);
  AFp->Sfreq = AF_srateInfo (&AFp->InfoS, AFr->Sfreq);
  AFp->NbS = AF_NbSInfo (&AFp->InfoS, &AFr->DFormat);
  AF_spkrInfo (&AFp->InfoS, NData.SpkrConfig);
  AFp->SpkrConfig = AF_setSpeaker (NData.SpkrConfig, NData.Nchan);

  return AFp;
}

/* Check file data size and number of samples */


static int
AF_checkLdata (FILE *fp, int Lw, long int *Dstart, struct AF_ndata *NData,
	       int Fix)

{
  int ErrCode;

/* Preliminary checks */
  if (NData->Ldata != AF_LDATA_UNDEF && NData->Ldata < 0) {
    UTwarn ("AFsetRead - %s: \"%ld\"", AFM_BadDLen, NData->Ldata);
    return 1;
  }
  if (NData->Nsamp != AF_NSAMP_UNDEF && NData->Nsamp < 0) {
    UTwarn ("AFsetRead - %s: \"%ld\"", AFM_BadNSamp, NData->Nsamp);
    return 1;
  }

/* Save the current (Start-of-Data) position */
  *Dstart = 0;
  if (FLseekable (fp)) {
    ErrCode = 0;
    *Dstart = AFtell (fp, &ErrCode);
    if (ErrCode)
      return 1;

    /* Find Ldata and Dstart */
    if (Lw != 0) {
      /* Binary, seekable file */
      if (AF_setLdata (fp, *Dstart, NData, Fix))
	return 1;
    }
  }

/* Set Nsamp if necessary */
  if (AF_setNsamp (fp, Lw, NData, Fix))
    return 1;

/* Samples / channels consistency check */
  if (NData->Nchan <= 0) {
    UTwarn ("AFsetRead - %s: \"%ld\"", AFM_BadNChan, NData->Nchan);
    return 1;
  }
  if (NData->Nsamp != AF_NSAMP_UNDEF && (NData->Nsamp % NData->Nchan) != 0) {
    UTwarn ("AFsetRead - %s", AFM_NSampNChan);
    UTwarn (AFMF_NSampNChan, "           ", NData->Nsamp, NData->Nchan);
  }

  return 0;
}

/* Set Ldata for binary, seekable files */


int
AF_setLdata (FILE *fp, long int Dstart, struct AF_ndata *NData, int Fix)

{
  long int Dspace;

  Dspace = FLfileSize (fp) - Dstart;
  if (Dspace < 0)
    return 1;

  /* Set Ldata if necessary */
  if (NData->Ldata == AF_LDATA_UNDEF)
    NData->Ldata = Dspace;
  else if (NData->Ldata > Dspace) {
    if (Fix & AF_FIX_LDATA_HIGH) {
      UTwarn ("AFsetRead - %s", AFM_FixHiDLen);
      NData->Ldata = Dspace;
    }
    else {
      UTwarn ("AFsetRead - %s", AFM_ErrHiDLen);
      return 1;
    }
  }

  return 0;
}

/* Set and check Nsamp */

int
AF_setNsamp (FILE *fp, int Lw, struct AF_ndata *NData, int Fix)

{
  struct AF_opt *AFopt;

/* Set Nsamp if necessary */
  if (Lw != 0) {
    if (NData->Nsamp == AF_NSAMP_UNDEF && NData->Ldata != AF_LDATA_UNDEF) {
      NData->Nsamp = NData->Ldata / Lw;
      if (Lw * NData->Nsamp != NData->Ldata)
	UTwarn ("AFsetRead - %s", AFM_NonIntNSamp);
    }
    else if (NData->Nsamp != AF_NSAMP_UNDEF) {

      /* Check Nsamp against Ldata */
      if (AF_checkNsamp (Lw, NData, Fix))
	return 1;
    }

  }
  else if (NData->Nsamp == AF_NSAMP_UNDEF && FLseekable (fp)) {
    NData->Nsamp = AF_Nrec (fp);
    if (NData->Nsamp == AF_NSAMP_UNDEF)
      return 1;	/* Error in AF_Nrec */
  }

  /* Check if an "unknown" Nsamp is allowed */
  AFopt = AFoptions ();
  if (NData->Nsamp == AF_NSAMP_UNDEF && ! AFopt->NsampND) {
    UTwarn ("AFsetRead - %s", AFM_NoNSamp);
    return 1;
  }

  return 0;
}

/* Check Nsamp against Ldata and fix if so desired */


static int
AF_checkNsamp (int Lw, struct AF_ndata *NData, int Fix)

{
  if (NData->Nsamp > LONG_MAX / Lw || NData->Nsamp * Lw > NData->Ldata) {
    if (Fix & AF_FIX_NSAMP_HIGH) {
      UTwarn ("AFsetRead - %s", AFM_FixHiNSamp);
      NData->Nsamp = NData->Ldata / Lw;
    }
    else {
      UTwarn ("AFsetRead - %s", AFM_ErrHiNSamp);
      return 1;
    }
  }
  else if (NData->Nsamp * Lw < NData->Ldata && (Fix & AF_FIX_NSAMP_LOW)) {
    UTwarn ("AFsetRead - %s", AFM_FixLoNSamp);
    NData->Nsamp = NData->Ldata / Lw;
  }

  return 0;
}

/*
  This routine finds the number of text records in a file, starting at the
  current position in the file.  The file position is restored to the current
  position.
*/


static long int
AF_Nrec (FILE *fp)

{
  long int pos, Nrec;
  char *p;
  int ErrCode;

  ErrCode = 0;

/* Save the file position */
  pos = AFtell (fp, &ErrCode);

/* Read until End-of-file */
  for (Nrec = 0; ; ++Nrec) {

    /* Read a line or partial line */
    p = AFgetLine (fp, &ErrCode);
    if (p == NULL || ErrCode)
      break;
  }

/* Reposition the file */
  if (AFseek (fp, pos, &ErrCode))
    return AF_NSAMP_UNDEF;

  return Nrec;
}

/* -------------------- */
/* Fill in the AFsp Information structure */

/* This routine allocates the space for the information string.  It is expected
   that the calling routine take care of freeing up the space.
*/


static struct AF_info
AF_setInfo (const struct AF_infoX *InfoX)

{
  int N;
  struct AF_info InfoS;

  N = InfoX->N;
  if (N <= 0) {
    InfoS.Info = NULL;
    InfoS.N = 0;
  }
  else {
    if (InfoX->Info[N-1] != '\0')
      ++N;
    InfoS.Info = (char *) UTmalloc (N);
    memcpy (InfoS.Info, InfoX->Info, InfoX->N);
    if (N != InfoX->N)
      InfoS.Info[N-1] = '\0';	/* Make sure there is a terminating null */
    InfoS.N = N;
  }

  return InfoS;
}

/* Scan the header information string for the sampling frequency */

#define ABSV(x)		(((x) < 0) ? -(x) : (x))


static double
AF_srateInfo (const struct AF_info *InfoS, double Sfreq)

{
  const char *Csf;
  double Fv;

  /* Check "sample_rate:", if the sampling frequency is integer-valued */
  if (Sfreq == floor(Sfreq)) {

    /* Scan the information string for a "sample_rate:" record */
    Csf = AFgetInfoRec ("sample_rate:", InfoS);
    if (Csf != NULL) {
      if (STdec1double (Csf, &Fv))
	UTwarn ("AFsetRead - %s", AFM_BadSRate);
      else if (ABSV(Fv - Sfreq) <= 0.5)
	Sfreq = Fv;
      else
	UTwarn ("AFsetRead - %s, %g : %g", AFM_MisSRate, Sfreq, Fv);
    }
  }

  if (Sfreq <= 0.0) {
    UTwarn (AFMF_BadSFreq, "AFsetRead -", Sfreq, AF_SFREQ_DEFAULT);
    Sfreq = AF_SFREQ_DEFAULT;
  }

  return Sfreq;
}

/* Set the bits/sample value */


static int
AF_NbSInfo (const struct AF_info *InfoS, const struct AF_dformat *DFormat)

{
  int Res, NbS, ResX, Format;
  const char *Csf;

  Format = DFormat->Format;
  Res = 8 * AF_DL[Format];
  NbS = DFormat->NbS;

  /* Pick up NbS from the Info string */
  if (NbS == 0) {
    NbS = Res;
    Csf = AFgetInfoRec ("bits_per_sample:", InfoS); 
    if (Csf != NULL) {
      ResX = Res;
      if (STdecIfrac (Csf, &NbS, &ResX))
	UTwarn ("AFsetRead - %s", AFM_BadNbS);
      else {
	if (ResX == 1)
	  ResX = NbS;
	if (ResX != Res){
	  UTwarn (AFMF_InvNbS, "AFsetRead -", ResX, Res);
	  NbS = Res;
	}
      }
    }
  }

  /* Res == 0 for text files */
  if (Res > 0 && (NbS <= 0 || NbS > Res)) {
    UTwarn (AFMF_InvNbS, "AFsetRead -", NbS, Res);
    NbS = Res;
  }
  else if (Res == 0 && NbS != 0) {
    UTwarn ("AFsetRead - %s", AFM_BadNbS);
    NbS = Res;
  }

  /* Warn and reset NbS for inappropriate formats */
  if (NbS != Res &&
      ! (Format == FD_UINT8 || Format == FD_INT8 || Format == FD_INT16 ||
	 Format == FD_INT24 || Format == FD_INT32)) {
    UTwarn ("AFsetRead - %s", AFM_InaNbS);
    NbS = Res;
  }

  return NbS;
}

/* Scan the header information string for the loudspeaker configuration */


static void
AF_spkrInfo (const struct AF_info *InfoS, unsigned char *SpkrConfig)

{
  const char *Csf;

  /* Check "loudspeakers:", if SpkrConfig is empty */
  if (SpkrConfig[0] == AF_X_SPKR) {

    /* Scan the information string for a "loudspeakers:" record */
    Csf = AFgetInfoRec ("loudspeakers:", InfoS);
    if (Csf != NULL)
      AFdecSpeaker (Csf, SpkrConfig, AF_MAXN_SPKR);
  }

  return;
}

/* Allocate space for and set the loudspeaker configuration */


static unsigned char *
AF_setSpeaker (const unsigned char *SpkrX, int Nchan)

{
  int Nspkr;
  unsigned char *SpkrConfig;

  SpkrConfig = NULL;
  Nspkr = strlen ((const char *) SpkrX);
  if (Nspkr > Nchan)
    Nspkr = Nchan;
  if (Nspkr > 0) {
    SpkrConfig = (unsigned char *) UTmalloc (Nspkr + 1);
    strncpy ((char *) SpkrConfig, (const char *) SpkrX, Nspkr);
    SpkrConfig[Nspkr] = '\0';
  }

  return SpkrConfig;
}
