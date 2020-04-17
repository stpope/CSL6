/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  long int AFtell (FILE *fp, int *ErrCode)

Purpose:
  Determine the position in a file

Description:
  This routine returns the position in a file.  For binary files this will be
  the number of bytes from the beginning of the file.  For non-binary files,
  the returned value does not necessarily have this interpretation.  However,
  in all cases the returned value can be used as an argument to AFseek to
  return to the same position.

Parameters:
  <-  long int AFtell
      Position in the file
   -> FILE *fp
      File pointer associated with the file
  <-> int ErrCode
      Error code.  This value is set if an error is detected, but otherwise
      remains unchanged.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.9 $  $Date: 2003/05/09 01:11:35 $

-------------------------------------------------------------------------*/

#include <errno.h>

#include <libtsp.h>
#include <libtsp/AFdataio.h>
#include <libtsp/AFmsg.h>
#include <libtsp/AFpar.h>


long int
AFtell (FILE *fp, int *ErrCode)

{
  long int pos;

  pos = -1L;
  if (! *ErrCode) {
    errno = 0;
    pos = ftell (fp);
    if (pos == -1L && errno) {
      UTsysMsg ("AFtell: %s", AFM_NoFilePos);
      *ErrCode = AF_IOERR;
    }
  }

  return pos;
}
