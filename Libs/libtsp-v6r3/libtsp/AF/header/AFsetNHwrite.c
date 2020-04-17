/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  AFILE *AFsetNHwrite (FILE *fp, struct AF_write *AF)

Purpose:
  Set file format information for a headerless audio file

Description:
  This routine sets the file format information for a headerless audio file.

Parameters:
  <-  AFILE AFsetNHwrite
      Audio file pointer for the audio file.  In case of error, the audio file
      pointer is set to NULL.
   -> FILE *fp
      File pointer for the audio file
  <-> struct AF_write *AFw
      Structure containing file parameters

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.34 $  $Date: 2003/05/09 01:21:35 $

-------------------------------------------------------------------------*/

#include <string.h>

#include <libtsp.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFpar.h>
#include <libtsp/AFmsg.h>
#include <libtsp/UTpar.h>


AFILE *
AFsetNHwrite (FILE *fp, struct AF_write *AFw)

{
  AFILE *AFp;

/* Set the parameters for file access */
  AFw->DFormat.ScaleF = 1. / AF_SF[AFw->DFormat.Format];
  AFw->DFormat.Swapb = AFw->Ftype / FTW_SUBTYPE_MOD;

/* Check the swap code */
  if (! (AFw->DFormat.Swapb == DS_EB     || AFw->DFormat.Swapb == DS_EL ||
	 AFw->DFormat.Swapb == DS_NATIVE || AFw->DFormat.Swapb == DS_SWAP)) {
    UTwarn ("AFsetNHwrite - %s", AFM_NH_InvSwap);
    return NULL;
  }
 
  AFp = AFsetWrite (fp, FT_NH, AFw);

  return AFp;
}
