/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  AFILE *AFrdSFhead (FILE *fp)

Purpose:
  Get file format information from an IRCAM soundfile

Description:
  This routine reads the header for an IRCAM soundfile.  The header information
  is used to set the file data format information in the audio file pointer
  structure.  A banner identifying the audio file and its parameters is
  printed.

  IRCAM soundfile header:
   Offset Length Type    Contents
      0     4    int    File identifier
      4     4    float  Sampling frequency
      8     4    int    Number of interleaved channels
     12     4    int    Data type
     16    ...   --     Additional header information
   1024    ...   --     Audio data

Parameters:
  <-  AFILE *AFrdSFhead
      Audio file pointer for the audio file
   -> FILE *fp
      File pointer for the file

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.72 $  $Date: 2003/11/03 18:56:00 $

-------------------------------------------------------------------------*/

#include <setjmp.h>
#include <string.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFmsg.h>
#include <libtsp/AFpar.h>
#include <libtsp/UTtypes.h>

#define SAME_CSTR(str,ref) 	(memcmp (str, ref, sizeof (str)) == 0)

#define LHEAD		1024L
/* Magic in file byte order */
/* - two byte magic, 1 byte machine type, zero byte */
#define FM_SF_VAX_L	"\144\243\001\0"	/* vax native data */
#define FM_SF_VAX_B	"\0\001\243\144"	/* vax - byte reversed data */
#define FM_SF_SUN_B	"\144\243\002\0"	/* sun native data*/
#define FM_SF_SUN_L	"\0\002\243\144"	/* sun - little-endian data */
#define FM_SF_MIPS_L	"\144\243\003\0"	/* little-endian data */
#define FM_SF_MIPS_B	"\0\003\243\144"	/* big-endian data */
#define FM_SF_NEXT	"\144\243\004\0"	/* big-endian data */

#define SF_CHAR		0x00001
#define SF_ALAW		0x10001
#define SF_ULAW		0x20001
#define SF_SHORT	0x00002
#define SF_24INT	0x00003
#define SF_LONG		0x40004
#define SF_FLOAT	0x00004
#define SF_DOUBLE	0x00008

#define SF_END		0
#define SF_MAXAMP	1
#define SF_COMMENT	2

#define SF_MAXINFO	LHEAD

struct SF_head {
  char Magic[4];	/* File magic */
  float4_t sf_srate;	/* Sampling frequency */
  uint4_t sf_chans;	/* Number of channels */
  uint4_t sf_packmode;	/* Encoding type */
};
struct SF_code {
  uint2_t code;
  uint2_t bsize;
};

/* setjmp / longjmp environment */
extern jmp_buf AFR_JMPENV;

AF_READ_DEFAULT(AFr_default);	/* Define the AF_read defaults */

static int
AF_getComment (FILE *fp, int Size, int Fbo, struct AF_infoX *InfoX);


AFILE *
AFrdSFhead (FILE *fp)

{
  AFILE *AFp;
  long int offs;
  char Info[SF_MAXINFO];
  struct SF_head Fhead;
  struct AF_read AFr;

/* Set the long jump environment; on error return a NULL */
  if (setjmp (AFR_JMPENV))
    return NULL;	/* Return from a header read error */

/* Defaults and inital values */
  AFr = AFr_default;
  AFr.InfoX.Info = Info;
  AFr.InfoX.Nmax = SF_MAXINFO;

/* Check the file magic */
  offs = RHEAD_S (fp, Fhead.Magic);

  if (SAME_CSTR (Fhead.Magic, FM_SF_VAX_L) ||
      SAME_CSTR (Fhead.Magic, FM_SF_SUN_L) ||
      SAME_CSTR (Fhead.Magic, FM_SF_MIPS_L))
    AFr.DFormat.Swapb = DS_EL;
  else if (SAME_CSTR (Fhead.Magic, FM_SF_VAX_B) ||
	   SAME_CSTR (Fhead.Magic, FM_SF_SUN_B) ||
	   SAME_CSTR (Fhead.Magic, FM_SF_MIPS_B) ||
	   SAME_CSTR (Fhead.Magic, FM_SF_NEXT))
    AFr.DFormat.Swapb = DS_EB;
  else {
    UTwarn ("AFrdSFhead - %s", AFM_SF_BadId);
    return NULL;
  }

/* Notes:
   - IRCAM files can be written by a number of different hosts.  The data is
     normally written out in native format (byte order and float format).
   - Early IRCAM software ran on VAX computers (little-endian) with float data
     (sampling frequency) in VAX float format.
   - Subsequent IRCAM software ran on Sun computers (big-endian).  The file
     data including the header values is byte-swapped with respect to the
     native VAX format.  The float format is IEEE.
   - The file magic for the VAX files is "\144\243\001\0".  The file magic for
     the corresponding Sun files is the byte reversal of this.
   - The MIT csound, the Princeton cmix and earlier sox systems support files
     as described above.
   - Subsequently, the magic in IRCAM files was been updated to indicate the
     host type used to create the file.  In file byte order the magic values
     and the corresponding host type are:
        "\144\243\001\0"   vax   little-endian data
        "\144\243\002\0"   sun   big-endian data
        "\144\243\003\0"   mips  little-endian data
        "\144\243\004\0"   next  big-endian data
   - The sox utility (as of version 11) has a compile time flag.  In the
     IRCAM mode (default), it writes files with either of two magic values
     depending on whether the host is big- or little-endian (shown in file
     byte order),
        "\144\243\001\0"   little-endian
        "\144\243\002\0"   big-endian
     In the non-IRCAM mode, the file magic is one of 4 values depending on
     the machine type (shown in file byte order),
        "\144\243\001\0"   vax   little-endian data
        "\144\243\002\0"   sun   big-endian data
        "\144\243\003\0"   mips  little-endian data
        "\144\243\004\0"   next  big-endian data
     The choice of machine is on the basis of the definition of the
     pre-processor symbols "vax", "sun", "mips" or "NeXT".  The mips choice
     is predicated on the machine being little-endian (DEC mips machines).
     However, SGI mips machines also set the mips symbol, but are big-endian
     and so will generate a byte swapped version of a "mips" file.
   - There are incompatibilities between the IRCAM data format codes and those
     used by the MIT Media lab csound package.  For instance, the csound
     program uses SF_ULAW as 0x00001.
   - There are examples of IRCAM files (bicsf files) with float data which
     have been scaled to +/-1.
*/

/* Read in the rest of the header */
  offs += RHEAD_V (fp, Fhead.sf_srate, AFr.DFormat.Swapb);
  offs += RHEAD_V (fp, Fhead.sf_chans, AFr.DFormat.Swapb);
  offs += RHEAD_V (fp, Fhead.sf_packmode, AFr.DFormat.Swapb);

/* Pick up comments */
  offs += AF_getComment (fp, (int) (LHEAD - offs), AFr.DFormat.Swapb,
			 &AFr.InfoX);

/* Position at the start of data */
  RSKIP (fp, LHEAD - offs);

/* Set up the decoding parameters */
  switch (Fhead.sf_packmode) {
  case SF_CHAR:
    AFr.DFormat.Format = FD_INT8;
    break;
  case SF_ULAW:
    AFr.DFormat.Format = FD_MULAW8;
    break;
  case SF_ALAW:
    AFr.DFormat.Format = FD_ALAW8;
    break;
  case SF_SHORT:
    AFr.DFormat.Format = FD_INT16;
    break;
  case SF_24INT:
    AFr.DFormat.Format = FD_INT24;
    break;
  case SF_LONG:
    AFr.DFormat.Format = FD_INT32;
    break;
  case SF_FLOAT:
    AFr.DFormat.Format = FD_FLOAT32;
    break;
  case SF_DOUBLE:
    AFr.DFormat.Format = FD_FLOAT64;
    break;
  default:
    UTwarn ("AFrdSFhead - %s: \"%ld\"", AFM_SF_UnsData, Fhead.sf_packmode);
    return NULL;
  }

/* Set the parameters for file access */
  AFr.Sfreq = (double) Fhead.sf_srate;
  AFr.NData.Nchan = (long int) Fhead.sf_chans;

  AFp = AFsetRead (fp, FT_SF, &AFr, AF_NOFIX);

  return AFp;
}

/* Pick up comments and place them in the AFsp information structure */


static int
AF_getComment (FILE *fp, int Size, int Fbo, struct AF_infoX *InfoX)

{
  int offs, nc;
  struct SF_code SFcode;

  offs = 0;
  while (offs < Size) {
    offs += RHEAD_V (fp, SFcode.code, Fbo);
    if (SFcode.code == SF_END)
      break;
    offs += RHEAD_V (fp, SFcode.bsize, Fbo);
    nc = SFcode.bsize - (sizeof SFcode.code) - (sizeof SFcode.bsize);
    if (SFcode.code == SF_COMMENT)
      offs += AFrdTextAFsp (fp, nc, "IRCAM comment: ", InfoX, 1);
    else
      offs += RSKIP (fp, nc);
  }

  return offs;
}
