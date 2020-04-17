/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFfWriteData (AFILE *AFp, const float Dbuff[], int Nval)

Purpose:
  Write data to an audio file (float input values)

Description:
  This routine writes a specified number of samples to an audio file.  The
  float input data is converted to the audio file data representation.  The
  file data representation is set on opening the file with routine AFopnWrite.
  This routine writes data sequentially to the file.  A warning message is
  printed if the input values exceed the dynamic range of the file data
  representation. 

Parameters:
  <-  int AFfWriteData
      Number of samples written
   -> AFILE *AFp
      Audio file pointer for an audio file opened by AFopnWrite
   -> const float Dbuff[]
      Array of floats with Nval samples
   -> int Nval
      Number of samples to be written

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.3 $  $Date: 2003/05/13 01:50:31 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <stdlib.h>		/* EXIT_FAILURE */

#include <libtsp.h>
#include <libtsp/AFdataio.h>
#include <libtsp/AFmsg.h>
#include <libtsp/AFpar.h>

#define MAXV(a, b)	(((a) > (b)) ? (a) : (b))

/* Writing routines */
static int
(*AF_Write[NFD])(AFILE *AFp, const float Dbuff[], int Nval) =
  { NULL,    	AFfWrMulaw,	AFfWrAlaw,	AFfWrU1,       	AFfWrI1,
    AFfWrI2,	AFfWrI3,	AFfWrI4,	AFfWrF4,	AFfWrF8,
    AFfWrTA };


int
AFfWriteData (AFILE *AFp, const float Dbuff[], int Nval)

{
  int Nw;
  long int Novld;
  struct AF_opt *AFopt;

  assert (AFp->Op == FO_WO);

/* The file writing routines write scaled data to the file.  They write to the
   current file position.  They use the following AFp fields:
     AFp->fp - file pointer
     AFp->Swapb - data swap flag
     AFp->ScaleF - data scaling factor
     AFp->Ovld - overload counter

  This routine updates the following AFp values
    AFp->Error - error flag
    AFp->Isamp - current data sample.  This value is incremented by the
      number of samples written.
    AFp->Nsamp - last sample (updated if AFp->Isamp is beyond it)
*/

/* Transfer data to the audio file */
  Novld = AFp->Novld;	/* Save the value before writing */
  Nw = (*AF_Write[AFp->Format]) (AFp, Dbuff, Nval);
  AFp->Isamp += Nw;
  AFp->Nsamp = MAXV (AFp->Isamp, AFp->Nsamp);

/* Check for an error */
  if (Nw < Nval) {
    UTsysMsg ("AFfWriteData: %s", AFM_WriteErr);
    AFopt = AFoptions ();
    if (AFopt->ErrorHalt)
      exit (EXIT_FAILURE);
    AFp->Error = AF_IOERR;
  }

/* Check for overloads (print a message the first time only) */
  if (Novld == 0L && AFp->Novld != 0L)
    UTwarn ("AFfWriteData - %s", AFM_OClip);

  return Nw;
}
