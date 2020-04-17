/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void VRfShift (float x[], int Nkeep, int Nshift)

Purpose:
  Shift the elements of an array of floats

Description:
  This routine shifts elements of a float array.  If the number of elements to
  be retained is zero, no action is taken.  For a shift down (Nshift positive),
  the elements of the array are shifted down to the bottom of the array.  The
  input array must have at least Nkeep + Nshift elements.

    x[i+Nshift] --> x[i], for i from 0 to Nkeep-1

  For a shift up (Nshift negative), the elements of the array are shifted up to
  the top of the array.  The input array must have at least Nkeep - Nshift
  elements (Nshift being negative).

    x[i] --> x[i-Nshift], for i from Nkeep-1 to 0

Parameters:
  <-> float x[]
      Array of floats (Nkeep + |Nshift| elements)
   -> int Nkeep
      Number of elements to be retained
   -> int Nshift
      Number of positions to be shifted.  Nshift is positive for a shift down
      and negative for a shift up.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.11 $  $Date: 2003/05/09 03:33:10 $

-------------------------------------------------------------------------*/

#include <libtsp.h>


void
VRfShift (float x[], int Nkeep, int Nshift)

{
  int i;

  if (Nshift > 0) {
    /* Shift down */
    for (i = 0; i < Nkeep; ++i)
      x[i] = x[i+Nshift];
  }
  else if (Nshift < 0) {
    /* Shift down */
    for (i = Nkeep - 1; i >= 0; --i)
      x[i-Nshift] = x[i];	/* note Nshift < 0 */
  }
  return;
}
