/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void AFprAFpar (AFILE *AFp, const char Fname[], FILE *fpinfo)

Purpose:
  Print information about an audio file

Description:
  This routine optionally prints header information for an audio file.

Parameters:
   -> AFILE *AFp
      Audio file pointer for the audio file.  If AFp is NULL, this routine
      is a no-op.
   -> const char Fname[]
      File name.  If the file is stdin or stdout, this string is not used.
   -> FILE *fpinfo
      File pointer for printing the audio file identification information.  If
      fpinfo is NULL, no information is printed.

Author / revision:
  P. Kabal  Copyright (C) 2004
  $Revision: 1.53 $  $Date: 2004/03/31 13:26:04 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFmsg.h>
#define AF_DATA_TYPE_NAMES_X
#define AF_FILE_TYPE_NAMES
#define AF_DATA_LENGTHS
#include <libtsp/AFpar.h>

#define NSPKR_EXTRA 7
#define NC_SPKR ((AF_NC_SPKR + 1) * (AF_MAXN_SPKR + NSPKR_EXTRA) - 1)

static const char *
AF_findRec (const char *Id[], const struct AF_info *InfoS);


void
AFprAFpar (AFILE *AFp, const char Fname[], FILE *fpinfo)

{
  const char *ft;
  const char *Datetime, *Title;
  char FullName[FILENAME_MAX];
  char FStr[64];
  char SpkrNames[NC_SPKR+1];
  int Res;
  static const char *IDtitle[] = {"title:", "display_name:", "name:",
				  "user_comment:", NULL};
  static const char *IDdate[] = {"date:", "recording_date:", "creation_date:",
				 NULL};

  if (AFp == NULL)
    return;

  assert (AFp->Op == FO_RO || AFp->Op == FO_WO);
  assert (AFp->Format > 0 && AFp->Format < NFD);
  assert (AFp->Ftype > 0 && AFp->Ftype < NFT);

/* Print the header information */
  if (fpinfo != NULL) {

    /* ---------------- */
    /* File type */
    if (AFp->Ftype == FT_NH && (AFoptions ())->NHpar.Start == 0L)
      ft = AFM_Headerless;
    else
      ft = AF_FTN[AFp->Ftype];
    fprintf (fpinfo, " %s: ", ft);

    /* File name */
    if (AFp->fp == stdin)
      fprintf (fpinfo, "<stdin>\n");
    else if (AFp->fp == stdout)
      fprintf (fpinfo, "<stdout>\n");
    else {
      FLfullName (Fname, FullName);
      fprintf (fpinfo, "%s\n", FullName);
    }

    /* ---------------- */
    /* Title */
    Title = AF_findRec (IDtitle, &AFp->InfoS);
    if (Title != NULL)
      fprintf (fpinfo, AFMF_Desc, STstrDots (Title, 56));

    /* ---------------- */
    /* Number of samples or frames */
    if (AFp->Op == FO_RO) {
      if (AFp->Nsamp == AF_NSAMP_UNDEF)
	fprintf (fpinfo, AFMF_NumSampUnk);
      else
	fprintf (fpinfo, AFMF_NumSamp, AFp->Nsamp / AFp->Nchan);

      /* Duration */
      if (AFp->Nsamp != AF_NSAMP_UNDEF && AFp->Sfreq > 0.0)
	fprintf (fpinfo, " (%.4g s)", (AFp->Nsamp / AFp->Nchan) / AFp->Sfreq);

      /* Date */
      Datetime = AF_findRec (IDdate, &AFp->InfoS);
      if (Datetime == NULL) {
	Datetime = "";
	if (AFp->fp != stdin)
	  Datetime = FLfileDate (AFp->fp, 3);
      }
      fprintf (fpinfo, "  %.30s\n",Datetime);
    }

    /* ---------------- */
    /* Sampling frequency */
    fprintf (fpinfo, AFMF_SFreq, AFp->Sfreq);

    /* ---------------- */
    /* Number of channels and data format */
    fprintf (fpinfo, AFMF_NumChan, AFp->Nchan);
    Res = 8 * AF_DL[AFp->Format];
    if (AFp->NbS == Res)
      sprintf (FStr, AF_DTX[AFp->Format], Res);
    else
      sprintf (FStr, AF_DTXX[AFp->Format], AFp->NbS, Res);
    fprintf (fpinfo, " (%s)", FStr);

    /* Speaker configuration */
    AFspeakerNames (AFp->Nchan, AFp->SpkrConfig, NSPKR_EXTRA, SpkrNames);
    if (strlen (SpkrNames) > 0)
      fprintf (fpinfo, " [%s]", SpkrNames);
    fprintf (fpinfo, "\n");
  }
  return;
}

/* Search for named records in an AFsp information structure */


static const char *
AF_findRec (const char *Id[], const struct AF_info *InfoS)

{
  int i;
  const char *p;

  p = NULL;
  for (i = 0; Id[i] != NULL; ++i) {
    p = AFgetInfoRec (Id[i], InfoS);
    if (p != NULL)
      break;
  }

  if (p != NULL)
    p = STtrimIws (p);

  return p;
}
