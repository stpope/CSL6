/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  AFILE *AFrdSWhead (FILE *fp)

Purpose:
  Get file format information from a Cadence SPW Signal file

Description:
  This routine reads the header for a Cadence SPW Signal file.  The header
  information is used to set the file data format information in the audio file
  pointer structure.

  Cadence SPW Signal file header:
   Offset Length Type    Contents
      0    12    char   File identifier ("$SIGNAL FILE 9\n")
  The remaining header consists of lines of text data, with each line
  terminated by a newline character.  The header is divided into sections with
  a section header marked by a string starting with a "$" character.  An
  example header is shown below.
    $USER_COMMENT
    <comment line(s)>
    $COMMON_INFO
    SPW Version        = 3.10
    System Type        = <machine> (e.g. "sun4")
    Sampling Frequency = <Sfreq>   (e.g. "8000")
    Starting Time      = 0
    $DATA_INFO
    Number of points   = <Nsamp>   (e.g. "2000")
    Signal Type        = <type>    ("Double", "Float", "Fixedpoint")
    Fixed Point Format = <16.0,t>  (optional)
    Complex Format     = Real_Imag (optional)
    $DATA <data_type>              ("ASCII", "BINARY")
  This routine does not support files with time stamps.

Parameters:
  <-  AFILE *AFrdSWhead
      Audio file pointer for the audio file
   -> FILE *fp
      File pointer for the file

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.39 $  $Date: 2003/11/03 18:56:26 $

-------------------------------------------------------------------------*/

#include <string.h>

#include <libtsp.h>
#include <libtsp/AFdataio.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFmsg.h>
#include <libtsp/AFpar.h>

#define NCHEAD_MAX	1024
#define SW_MAXINFO	NCHEAD_MAX

/*
 Minimal header              Number of chars
$SIGNAL_FILE 9			14
$USER_COMMENT			13
				 0
$COMMON_INFO			12
SPW Version        = 3		22
System Type        = ?		22
Sampling Frequency = 1		22
Starting Time      = 0		22
$DATA_INFO			10
Number of points   = 1		22
Signal Type        = Float	26
$DATA ASCII			11

                   total       196  + 12 newlines
*/

#define SW_FD_INT32		128

#define SW_UNDEF		-1
#define SW_SIGNAL_FILE_9	0
#define SW_USER_COMMENT		1
#define SW_COMMON_INFO		2
#define SW_DATA_INFO		3
#define SW_DATA_ASCII		4
#define SW_DATA_BINARY		5
static const char *SW_SectTab[] = {
  "$SIGNAL_FILE 9",
  "$USER_COMMENT",
  "$COMMON_INFO",
  "$DATA_INFO",
  "$DATA* ASCII",	/* "ASCII" is optional */
  "$DATA BINARY",
  NULL
};
static const int SW_sect[] = {
  SW_SIGNAL_FILE_9,
  SW_USER_COMMENT,
  SW_COMMON_INFO,
  SW_DATA_INFO,
  SW_DATA_ASCII,
  SW_DATA_BINARY
};

static const char *SW_CITab[] = {
  "SPW Version",
  "System Type",
  "Sampling Frequency",
  "Starting Time",
  NULL
};
static const char *SW_DITab[] = {
  "Number of points",
  "Signal Type",
  "fixed point Format",
  "Complex Format",
  NULL
};

AF_READ_DEFAULT(AFr_default);	/* Define the AF_read defaults */

static int
AF_CommonInfo (char line[], int *Fbo, double *Sfreq);
static int
AF_DataInfo (char line[], struct AF_read *AFr);


AFILE *
AFrdSWhead (FILE *fp)

{
  AFILE *AFp;
  int sect, n, nc, ErrCode;
  char *line;
  char Info[SW_MAXINFO];
  struct AF_read AFr;

/* Defaults and inital values */
  AFr = AFr_default;
  AFr.InfoX.Info = Info;
  AFr.InfoX.Nmax = SW_MAXINFO;

  /* Defaults */
  AFr.Sfreq = -1.0;

  ErrCode = 0;
  nc = 0;
  sect = SW_UNDEF;

  while (sect != SW_DATA_ASCII && sect != SW_DATA_BINARY) {

    if (nc >= NCHEAD_MAX) {
      UTwarn ("AFrdSWhead - %s", AFM_SW_LongHead);
      return NULL;
    }

    /* Read a line from the header */
    line = AFgetLine (fp, &ErrCode);
    if (line == NULL && ! ErrCode) {
      UTwarn ("AFrdSWhead - %s", AFM_UEoF);
      return NULL;
    }
    nc += strlen (line);
    ++nc;	/* Account for newline */

    /* Compare with section headers */
    n = STkeyMatch (line, SW_SectTab);
    if (n >= 0) {
      sect = SW_sect[n];
    }
    else {
      switch (sect) {

      case SW_UNDEF:
	UTwarn ("AFrdSWhead - %s", AFM_SW_BadId);
	return NULL;

      case SW_SIGNAL_FILE_9:
	break;

      case SW_USER_COMMENT:
	if (strlen (line) > 0)
	  AFaddAFspRec ("user_comment: ", line, strlen (line), &AFr.InfoX);
	break;

      case SW_COMMON_INFO:
	if (AF_CommonInfo (line, &AFr.DFormat.Swapb, &AFr.Sfreq))
	  return NULL;
	break;

      case SW_DATA_INFO:
	if (AF_DataInfo (line, &AFr))
	  return NULL;
	break;

      default:
	UTwarn ("AFrdSWhead - %s: \"%.30s\"", AFM_SW_UnkKey, line);
	return NULL;
      }
    }
  }
  if (sect == SW_DATA_ASCII)
    AFr.DFormat.Format = FD_TEXT;
    
/* Check for missing header information */
  if (AFr.DFormat.Format == FD_UNDEF) {
    UTwarn ("AFrdSWhead - %s", AFM_SW_NoData);
    return NULL;
  }
  if (AFr.Sfreq <= 0.0) {
    UTwarn ("AFrdSWhead - %s", AFM_SW_NoSFreq);
    return NULL;
  }

/* Check for incompatible options */
  if (AFr.DFormat.Format == FD_TEXT && AFr.NData.Nchan == 2) {
    UTwarn ("AFrdSWhead - %s", AFM_SW_AscCmplx);
    return NULL;
  }

  AFp = AFsetRead (fp, FT_SPW, &AFr, AF_NOFIX);

  return AFp;
}

/* Decode common info */


static int
AF_CommonInfo (char line[], int *Fbo, double *Sfreq)

{
  int n;
  float STime;

  n = STkeyXpar (line, "=", "", SW_CITab, line);
  switch (n) {
  case 0:
    /* SPW Version - ignore */
    break;
  case 1:
    /* System Type */
    if (strcmp (line, "sun4") == 0 || strcmp (line, "hp700") == 0)
      *Fbo = DS_EB;
    else {
      *Fbo = DS_NATIVE;
      UTwarn ("AFrdSWhead - %s: \"%.30s\"", AFM_SW_UnkSys, line);
    }
    break;
  case 2:
    /* Sampling Frequency */
    if (STdec1double (line, Sfreq) || *Sfreq <= 0.0) {
      UTwarn ("AFrdSWhead - %s: \"%.30s\"", AFM_SW_BadSFreq, line);
      return 1;
    }
    break;
  case 3:
    /* Starting Time */
    if (STdec1float (line, &STime)) {
      UTwarn ("AFrdSWhead - %s: \"%.30s\"", AFM_SW_BadSTime, line);
      return 1;
    }
    if (STime != 0.0F)
      UTwarn ("AFrdSWhead - %s: \"%g\"", AFM_SW_NZSTime, STime);
    break;
  default:
    UTwarn ("AFrdSWhead - %s: \"%.30s\"", AFM_SW_UnkCOM, line);
    break;
  }

  return 0;
}

/* Decode data info */


static int
AF_DataInfo (char line[], struct AF_read *AFr)

{
  int n;

  n = STkeyXpar (line, "=", "", SW_DITab, line);
  switch (n) {
  case 0:
    /* Number of points */
    if (STdec1long (line, &AFr->NData.Nsamp) || AFr->NData.Nsamp <= 0) {
      UTwarn ("AFrdSWhead - %s: \"%.30s\"", AFM_SW_BadNSamp, line);
      return 1;
    }
    break;
  case 1:
    /* Signal Type */
    if (strcmp (line, "Double") == 0) {
      AFr->DFormat.Format = FD_FLOAT64;
      AFr->DFormat.ScaleF = 32768.;
    }
    else if (strcmp (line, "Float") == 0) {
      AFr->DFormat.Format = FD_FLOAT32;
      AFr->DFormat.ScaleF = 32768.;
    }
    else if (strcmp (line, "Fixed-point") == 0) {
      AFr->DFormat.Format = FD_INT16;
      AFr->DFormat.ScaleF = 1.;
    }
    else if (strcmp (line, "Integer") == 0) {
      AFr->DFormat.Format = SW_FD_INT32;
      AFr->DFormat.ScaleF = 1.;
    }
    else {
      UTwarn ("AFrdSWhead - %s: \"%.30s\"", AFM_SW_UnsData, line);
      return 1;
    }
    break;
  case 2:
    /* fixed point Format */
    if (strcmp (line, "<16,16,t>") == 0) {
      AFr->DFormat.Format = FD_INT16;
      AFr->DFormat.ScaleF = 1.;
    }
    if (strcmp (line, "<16,0,t>") == 0) {
      AFr->DFormat.Format = FD_INT16;
      AFr->DFormat.ScaleF = 32768;
    }
    else if (strcmp (line, "<8,8,t>") == 0) {
      AFr->DFormat.Format = FD_INT8;
      AFr->DFormat.ScaleF = 128.;
    }
    else {
      UTwarn ("AFrdSWhead - %s: \"%.30s\"", AFM_SW_UnsFixP, line);
      return 1;
    }
    break;
  case 3:
    /* Complex Format */
    if (strcmp (line, "Real_Imag") == 0)
      AFr->NData.Nchan = 2;
    else {
      UTwarn ("AFrdSWhead - %s: \"%.30s\"", AFM_SW_UnsCmplx, line);
      return 1;
    }
    break;
  default:
    UTwarn ("AFrdSWhead - %s: \"%.30s\"", AFM_SW_UnsDInfo, line);
    return 1;
  }

  return 0;
}
