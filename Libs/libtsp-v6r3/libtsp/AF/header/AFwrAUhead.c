/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  AFILE *AFwrAUhead (FILE *fp, struct AF_write *AF)

Purpose:
  Write header information to an AU audio file

Description:
  This routine opens and writes header information to an AU audio file.  The
  additional header information is written as part of the file header.

  AU audio file header:
      Bytes     Type    Contents
     0 ->  3    int    File identifier
     4 ->  7    int    Header size H (bytes)
     8 -> 11    int    Audio data length (bytes)
    12 -> 15    int    Data encoding format
    16 -> 19    int    Sample rate (samples per second)
    20 -> 23    int    Number of interleaved channels
    24 -> 27    int    AFsp identifier ("AFsp)")
    28 -> H-1   --     Additional header information

Parameters:
  <-  AFILE *AFwrAUhead
      Audio file pointer for the audio file.  In case of error, the audio file
      pointer is set to NULL.
   -> FILE *fp
      File pointer for the audio file
  <-> struct AF_write *AFw
      Structure containing file parameters

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.78 $  $Date: 2003/11/03 18:46:21 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <setjmp.h>
#include <string.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFmsg.h>
#define AF_DATA_LENGTHS
#include <libtsp/AFpar.h>
#include <libtsp/AUpar.h>

#define ICEILV(n,m)	(((n) + ((m) - 1)) / (m))	/* int n,m >= 0 */
#define RNDUPV(n,m)	((m) * ICEILV (n, m))		/* Round up */
#define WRPAD(fp,size,align) \
     AFwriteHead (fp, NULL, 1, (int) (RNDUPV(size, align) - (size)), \
		  DS_NATIVE);

#define MCOPY(src,dest)		memcpy ((void *) (dest), \
					(const void *) (src), sizeof (dest))

#define ALIGN		4	/* Header length is a multiple of ALIGN */

/* setjmp / longjmp environment */
extern jmp_buf AFW_JMPENV;

static int
AF_setDencod (int Format);


AFILE *
AFwrAUhead (FILE *fp, struct AF_write *AFw)

{
  struct AU_head Fhead;
  AFILE *AFp;
  int Lhead;

/* Set the long jump environment; on error return a NULL */
  if (setjmp (AFW_JMPENV))
    return NULL;	/* Return from a header write error */

/* Leave room for the header information */
  if (AFw->InfoX.N > 0)
    Lhead = RNDUPV (AU_LHMIN + (sizeof FM_AFSP) - 1 + AFw->InfoX.N, ALIGN);
  else
    Lhead = RNDUPV (AU_LHMIN + 4L, ALIGN);

/* Set up the fixed header parameters */
  MCOPY (FM_AU, Fhead.Magic);
  Fhead.Lhead = Lhead;
  if (AFw->Nframe == AF_NFRAME_UNDEF)
    Fhead.Ldata = AU_NOSIZE;
  else
    Fhead.Ldata = AFw->Nframe * AFw->Nchan * AF_DL[AFw->DFormat.Format];
  Fhead.Dencod = AF_setDencod (AFw->DFormat.Format);
  Fhead.Srate = (uint4_t) (AFw->Sfreq + 0.5);	/* Rounding */
  Fhead.Nchan = (uint4_t) AFw->Nchan;

/* Write out the header */
  AFw->DFormat.Swapb = DS_EB;
  WHEAD_S (fp, Fhead.Magic);
  WHEAD_V (fp, Fhead.Lhead, DS_EB);
  WHEAD_V (fp, Fhead.Ldata, DS_EB);
  WHEAD_V (fp, Fhead.Dencod, DS_EB);
  WHEAD_V (fp, Fhead.Srate, DS_EB);
  WHEAD_V (fp, Fhead.Nchan, DS_EB);

/* AFsp information records */
  if (AFw->InfoX.N > 0) {
    WHEAD_SN (fp, FM_AFSP, (sizeof FM_AFSP) - 1);	/* Omit null char */
    WHEAD_SN (fp, AFw->InfoX.Info, AFw->InfoX.N);
    WRPAD (fp, AFw->InfoX.N, ALIGN);
  }
  else
    WHEAD_SN (fp, "\0\0\0\0", 4);

/* Set the parameters for file access */
  AFp = AFsetWrite (fp, FT_AU, AFw);

  return AFp;
}

/* Set up the encoding parameters */


static int
AF_setDencod (int Format)

{
  int Dencod;

  switch (Format) {
  case FD_MULAW8:
    Dencod = AU_MULAW8;
    break;
  case FD_ALAW8:
    Dencod = AU_ALAW8;
    break;
  case FD_INT8:
    Dencod = AU_LIN8;
    break;
  case FD_INT16:
    Dencod = AU_LIN16;
    break;
  case FD_INT24:
    Dencod = AU_LIN24;
    break;
  case FD_INT32:
    Dencod = AU_LIN32;
    break;
  case FD_FLOAT32:
    Dencod = AU_FLOAT32;
    break;
  case FD_FLOAT64:
    Dencod = AU_DOUBLE64;
    break;
  default:
    Dencod = AU_UNSPEC;  /* Error */
    assert (0);
  }

  return Dencod;
}
