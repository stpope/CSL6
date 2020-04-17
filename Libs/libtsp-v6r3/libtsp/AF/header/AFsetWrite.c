/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  AFILE *AFsetWrite (FILE *fp, int Ftype, const struct AF_write *AFw)

Purpose:
  Set up the parameters in an audio file structure for writing

Description:
  This routine checks the input parameters and puts them into a new audio file
  structure.

  This routine checks for a "sample_rate:" record in the information string.
  This record can specify a non-integer sampling frequency.

Parameters:
  <-  AFILE *AFsetWrite
      Audio file pointer for the audio file.  This routine allocates the space
      for this structure.
   -> FILE *fp
      File pointer for the audio file
   -> int Ftype
      File type: FT_NH, FT_AU, FT_WAVE, or FT_AIFF_C
   -> const struct AF_write *AFw
      Structure with data and file parameters

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.46 $  $Date: 2003/11/03 18:49:05 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFdataio.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFmsg.h>
#define AF_DATA_LENGTHS
#include <libtsp/AFpar.h>

static void
AF_setInfo (const struct AF_infoX *InfoX, struct AF_info *InfoS);
static int
AF_setWNbS (int NbS, int Format);


AFILE *
AFsetWrite (FILE *fp, int Ftype, const struct AF_write *AFw)

{
  AFILE *AFp;
  long int Start;
  int ErrCode, Nspkr;

  assert (Ftype == FT_WAVE || Ftype == FT_AU ||
	  Ftype == FT_AIFF || Ftype == FT_AIFF_C ||
	  Ftype == FT_NH);
  assert (AFw->DFormat.Format > 0 && AFw->DFormat.Format < NFD);
  assert (AFw->Nchan > 0);

/* Warnings */
  if (AFw->Sfreq <= 0.0)
    UTwarn ("AFsetWrite - %s", AFM_NPSFreq);

/* Set the start of data */
  if (FLseekable (fp)) {
    ErrCode = 0;
    Start = AFtell (fp, &ErrCode);
    if (ErrCode)
      return NULL;
  }
  else
    Start = 0;

/* Set the parameters for file access */
  AFp = (AFILE *) UTmalloc (sizeof (AFILE));
  AFp->fp = fp;
  AFp->Op = FO_WO;
  AFp->Error = AF_NOERR;
  AFp->Novld = 0;
  AFp->Sfreq = AFw->Sfreq;
  AFp->Ftype = Ftype;
  AFp->Format = AFw->DFormat.Format;
  if (AF_DL[AFw->DFormat.Format] <= 1)
    AFp->Swapb = DS_NATIVE;
  else
    AFp->Swapb = UTswapCode (AFw->DFormat.Swapb);
  AFp->NbS = AF_setWNbS (AFw->DFormat.NbS, AFp->Format);
  AFp->ScaleF = AFw->DFormat.ScaleF;
  if (AFp->ScaleF == AF_SF_DEFAULT)
    AFp->ScaleF = 1. / AF_SF[AFp->Format];
  AFp->Nchan = AFw->Nchan;

  AFp->SpkrConfig = NULL;
  Nspkr = strlen ((const char *) AFw->SpkrConfig);
  if (Nspkr > 0) {
    AFp->SpkrConfig = (unsigned char *) UTmalloc (Nspkr + 1);
    strcpy ((char *) AFp->SpkrConfig, (const char *) AFw->SpkrConfig);
  }

  AFp->Start = Start;
  AFp->Isamp = 0;
  AFp->Nsamp = 0;
  AF_setInfo (&AFw->InfoX, &AFp->InfoS);

  return AFp;
}

/* Fill in the AFsp Information structure */


static void
AF_setInfo (const struct AF_infoX *InfoX, struct AF_info *InfoS)

{
  int N;

  if (InfoX == NULL || InfoX->N <= 0) {
    InfoS->Info = NULL;
    InfoS->N = 0;
  }
  else {
    N = InfoX->N;
    if (InfoX->Info[N-1] != '\0')
      ++N;
    InfoS->Info = (char *) UTmalloc (N);
    memcpy (InfoS->Info, InfoX->Info, InfoX->N);
    InfoS->Info[N-1] = '\0';	/* Make sure there is a terminating null */
    InfoS->N = N;
  }

  return;
}

/* Set the bits/sample value */


static int
AF_setWNbS (int NbS, int Format)

{
  int NbSx;

  NbSx = 8 * AF_DL[Format];
  if (NbS < 0 || NbS > NbSx)
    UTwarn (AFMF_InvNbS, "AFsetWrite -", NbS, NbSx);
  else if (NbS != 0)
    NbSx = NbS;

  return NbSx;
}
