/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFwriteData (AFILE *AFp, const float Dbuff[], int Nval)

Purpose:
  Write data to an audio file (float input values)

Description:
  Use AFfWriteData instead.

Parameters:
  <-  int AFwriteData
      Number of samples written
   -> AFILE *AFp
      Audio file pointer for an audio file opened by AFopnWrite
   -> const float Dbuff[]
      Array of floats with Nval samples
   -> int Nval
      Number of samples to be written

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.28 $  $Date: 2003/05/09 01:03:46 $

-------------------------------------------------------------------------*/

#include <libtsp.h>

int
AFwriteData (AFILE *AFp, const float Dbuff[], int Nval)

{
  int Nw;

  Nw = AFfWriteData (AFp, Dbuff, Nval);

  return Nw;
}
