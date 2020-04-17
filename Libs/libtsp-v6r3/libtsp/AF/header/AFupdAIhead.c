/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFupdAIhead (AFILE *AFp)

Purpose:
  Update header information in an AIFF or AIFF-C sound file

Description:
  This routine updates the data length fields of an AIFF or AIFF-C sound file.
  The file is assumed to have been opened with routine AFopnWrite.

Parameters:
  <-  int AFupdAIhead
      Error code, zero for no error
   -> AFILE *AFp
      Audio file pointer for an audio file opened by AFopnWrite

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.27 $  $Date: 2003/05/09 01:21:35 $

-------------------------------------------------------------------------*/

#include <setjmp.h>

#include <libtsp.h>
#include <libtsp/AFdataio.h>
#include <libtsp/AFheader.h>
#define AF_DATA_LENGTHS
#include <libtsp/AFpar.h>
#include <libtsp/AIpar.h>

/* setjmp / longjmp environment */
extern jmp_buf AFW_JMPENV;


int
AFupdAIhead (AFILE *AFp)

{
  uint4_t val;
  long int Nbytes, Ldata;
  static const uint1_t Pad = 0;

/* Set the long jump environment; on error return a NULL */
  if (setjmp (AFW_JMPENV))
    return 1;	/* Return from a header write error */

/* This routine assumes that the header has the following layout
   offset  contents
      0     "FORM"
      4     FORM chunk length
     12     FVER chunk (AIFF-C files only)
   22 or 34 COMM chunk number of sample frames field
    ...     other chunks
   D-12     SSND chunk
      D     data
*/

/* Add a padding byte to the sound data; this padding byte is not included
   in the SSND chunk ckDataSize field, but is included in the FORM chunk
   ckDataSize field
*/
  Ldata = AF_DL[AFp->Format] * AFp->Nsamp;
  Nbytes = AFp->Start + Ldata;
  if (Nbytes % 2 != 0)			/* Nbytes includes pad byte */
    Nbytes += WHEAD_V (AFp->fp, Pad, DS_EB);

/* Update the FORM chunk ckDataSize field */
  val = (uint4_t) (Nbytes - 8);
  if (AFseek (AFp->fp, 4L, NULL))
    return 1;
  WHEAD_V (AFp->fp, val, DS_EB);

/* Update the COMM chunk numSampleFrames field:
   - AIFF-C: The COMM chunk follows the FVER chunk in the FORM chunk
   - AIFF:   The COMM chunk is the first chunk
*/
  val = (uint4_t) (AFp->Nsamp / AFp->Nchan);
  if (AFp->Ftype == FT_AIFF_C) {
    if (AFseek (AFp->fp, 34L, NULL))
      return 1;
  }
  else {
    if (AFseek (AFp->fp, 22L, NULL))
      return 1;
  }
  WHEAD_V (AFp->fp, val, DS_EB);
   

/* Update the SSND chunk ckDataSize field (assume SSND.offset == 0)
   SSND.ckDataSize is 12 bytes before the start of the audio data
 */
  val = (uint4_t) (Ldata + 8L);
  if (AFseek (AFp->fp, AFp->Start - 12L, NULL))
    return 1;
  WHEAD_V (AFp->fp, val, DS_EB);

  return 0;
}
