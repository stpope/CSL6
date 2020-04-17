/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFupdAUhead (AFILE *AFp)

Purpose:
  Update header information in an AU audio file

Description:
  This routine updates the data length field of an AU format audio file.  The
  file is assumed to have been opened with routine AFopnWrite.

Parameters:
  <-  int AFupdAUhead
      Error code, zero for no error
   -> AFILE *AFp
      Audio file pointer for an audio file opened by AFopnWrite

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.38 $  $Date: 2003/05/09 01:21:35 $

-------------------------------------------------------------------------*/

#include <setjmp.h>

#include <libtsp.h>
#include <libtsp/AFdataio.h>
#include <libtsp/AFheader.h>
#define AF_DATA_LENGTHS
#include <libtsp/AFpar.h>
#include <libtsp/AUpar.h>

/* setjmp / longjmp environment */
extern jmp_buf AFW_JMPENV;


int
AFupdAUhead (AFILE *AFp)

{
  uint4_t val;
  long int Ldata;

/* Set the long jump environment; on error return a 1 */
  if (setjmp (AFW_JMPENV))
    return 1;		/* Return from a header write error */

  Ldata = AF_DL[AFp->Format] * AFp->Nsamp;

/* Move to the header data length field */
  val = (uint4_t) Ldata;
  if (AFseek (AFp->fp, 8L, NULL))
    return 1;
  WHEAD_V (AFp->fp, val, DS_EB);

  return 0;
}
