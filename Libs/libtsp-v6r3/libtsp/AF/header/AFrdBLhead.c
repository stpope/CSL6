/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  AFILE *AFrdBLhead (FILE *fp)

Purpose:
  Get file format information from an SPPACK file

Description:
  This routine reads the header for an SPPACK file.  The header information
  is used to set the file data format information in the audio file pointer
  structure.

  SPPACK sampled data file:
   Offset Length Type    Contents
      0   160    char   Text strings (2 * 80)
    160    80    char   Command line
    240     2    int    Domain
    242     2    int    Frame size
    244     4    float  Sampling frequency
    252     2    int    File identifier
    254     2    int    Data type
    256     2    int    Resolution
    258     2    int    Companding
    260   240    char   Text strings (3 * 80)
    500     6    int    DAT-Link information (3 * 2)
    512   ...    --     Audio data

Parameters:
  <-  AFILE *AFrdBLhead
      Audio file pointer for the audio file
   -> FILE *fp
      File pointer for the file

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.52 $  $Date: 2003/11/03 18:54:13 $

-------------------------------------------------------------------------*/

#include <ctype.h>
#include <setjmp.h>
#include <string.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFmsg.h>
#include <libtsp/AFpar.h>
#include <libtsp/UTtypes.h>

#define SAME_CSTR(str,ref) 	(memcmp (str, ref, sizeof (str)) == 0)
#define SWAPB(value) \
	VRswapBytes ((const void *) &(value), \
		     (void *) &(value), sizeof (value), 1)

#define LHEAD		512L
#define FM_SPPACK_BE	"\100\303"	/* Magic value in file byte order */
#define FM_SPPACK_LE	"\303\100"

#define S_SAMPLEDDATA	((uint2_t) 0xFC0E)	/* Sampled data file */
#define DLMAGIC		4567
#define BL_MAXINFO	LHEAD

enum {
  C_UNIFORM = 1,	/* uniform */
  C_ALAW = 2,		/* A-law companding */
  C_MULAW = 3		/* mu-law companding */
};
enum {
  D_TIME = 1,		/* time domain */
  D_FREQ = 2,		/* frequency domain */
  D_QUEFR = 3		/* quefrency domain */
};

struct SPPACK_head {
/*  char cs1[80]; */
/*  char cs2[80]; */
/*  char cmd[80]; */
  uint2_t Domain;	/* Domain */
/*  uint2_t FrameSize; */
  float4_t Sfreq;	/* Sampling frequency */
/*  uint2_t unused[2]; */
  char Magic[2];	/* File magic */
  uint2_t Dtype;	/* Data type */
  uint2_t Resolution;	/* Resolution in bits */
  uint2_t Compand;	/* Companding */
/*  char lstr1[80]; */
/*  char lstr2[80]; */
/*  char lstr3[80]; */
  uint2_t dl_magic;	/* DAT-Link magic */
  uint2_t left;		/* Left channel flag */
  uint2_t right;	/* Right channel flag */
  /* uint2_t unused2[2]; */
};

/* setjmp / longjmp environment */
extern jmp_buf AFR_JMPENV;

AF_READ_DEFAULT(AFr_default);	/* Define the AF_read defaults */


AFILE *
AFrdBLhead (FILE *fp)

{
  AFILE *AFp;
  int Lb, offs;
  char Info[BL_MAXINFO];
  struct SPPACK_head Fhead;
  struct AF_read AFr;

/* Set the long jump environment; on error return a NULL */
  if (setjmp (AFR_JMPENV))
    return NULL;	/* Return from a header read error */

/* Defaults and inital values */
  AFr = AFr_default;
  AFr.InfoX.Info = Info;
  AFr.InfoX.Nmax = BL_MAXINFO;

/* Read the first part of the header */
  offs = AFrdTextAFsp (fp, 80, "cs1: ", &AFr.InfoX, 1);
  offs += AFrdTextAFsp (fp, 80, "cs2: ", &AFr.InfoX, 1);
  offs += AFrdTextAFsp (fp, 80, "cmd: ", &AFr.InfoX, 1);
  offs += RHEAD_V (fp, Fhead.Domain, DS_NATIVE);
  offs += RSKIP (fp, 2);	/* skip FrameSize */
  offs += RHEAD_V (fp, Fhead.Sfreq, DS_NATIVE);
  offs += RSKIP (fp, 4);	/* skip two fill values */
  offs += RHEAD_S (fp, Fhead.Magic);

/* Check the preamble file magic */
  if (SAME_CSTR (Fhead.Magic, FM_SPPACK_BE))
    AFr.DFormat.Swapb = DS_EB;
  else if (SAME_CSTR (Fhead.Magic, FM_SPPACK_LE))
    AFr.DFormat.Swapb = DS_EL;
  else {
    UTwarn ("AFrdBLhead - %s", AFM_BL_BadId);
    return NULL;
  }

/* Fix up the words we have already read */
  if (UTswapCode (AFr.DFormat.Swapb) == DS_SWAP) {
    SWAPB (Fhead.Domain);
    SWAPB (Fhead.Sfreq);
  }
 
/* Continue reading the header */
  offs += RHEAD_V (fp, Fhead.Dtype, AFr.DFormat.Swapb);
  offs += RHEAD_V (fp, Fhead.Resolution, AFr.DFormat.Swapb);
  offs += RHEAD_V (fp, Fhead.Compand, AFr.DFormat.Swapb);
  offs += RSKIP (fp, 240);
  offs += RHEAD_V (fp, Fhead.dl_magic, AFr.DFormat.Swapb);
  offs += RHEAD_V (fp, Fhead.left, AFr.DFormat.Swapb);
  offs += RHEAD_V (fp, Fhead.right, AFr.DFormat.Swapb);
  RSKIP (fp, (LHEAD - offs));

/* Set up the data format parameters */
  if (Fhead.Dtype == S_SAMPLEDDATA) {
    switch (Fhead.Compand) {
    case C_MULAW:
      Lb = 8;
      AFr.DFormat.Format = FD_MULAW8;
      break;
    case C_ALAW:
      Lb = 8;
      AFr.DFormat.Format = FD_ALAW8;
      break;
    case C_UNIFORM:
      Lb = 16;
      AFr.DFormat.Format = FD_INT16;
      break;
    default:
      UTwarn ("AFrdBLhead - %s: \"%d\"", AFM_BL_UnsComp, (int) Fhead.Compand);
      return NULL;
    }
  }
  else {
    UTwarn ("AFrdBLhead - %s: \"%d\"", AFM_BL_UnsData, (int) Fhead.Dtype);
    return NULL;
  }

  /* Error checks */
  if (Fhead.Resolution != Lb) {
    UTwarn ("AFrdBLhead - %s: \"%d\"", AFM_BL_UnsWLen, (int) Fhead.Resolution);
    return NULL;
  }
  if (Fhead.Domain != D_TIME) {
    UTwarn ("AFrdBLhead - %s: \"%d\"", AFM_BL_UnsDomain, (int) Fhead.Domain);
    return NULL;
  }
  if (Fhead.dl_magic == DLMAGIC) {
    AFr.NData.Nchan = 0L;
    if (Fhead.right)
      ++AFr.NData.Nchan;
    else if (Fhead.left)
      ++AFr.NData.Nchan;
  }

/* Set the parameters for file access */
  AFr.Sfreq = (double) Fhead.Sfreq;

  AFp = AFsetRead (fp, FT_SPPACK, &AFr, AF_NOFIX);

  return AFp;
}
