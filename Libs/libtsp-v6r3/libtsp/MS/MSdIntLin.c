/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void MSfIntLin (const double x[], const double y[], int N, const double xi[],
		  double yi[], int Ni)

Purpose:
  Interpolate a table of values using a piecewise linear interpolant

Description:
  This routine calculates interpolated values for a function defined by a table
  of reference data values.  The interpolated values are found by passing a
  piecewise linear interpolant through the reference data values.

Parameters:
   -> const double x[]
      Abscissa values for the reference points.  These values must be in
      increasing order.
   -> const double y[]
      Ordinate values for the reference points
   -> int N
      Number of reference points
   -> const double xi[]
      Abscissa values for the interpolated points.  These values must be
      bounded by x[0] and x[N-1].  The values xi[i] need not be in any
      particular order.
  <-  double yi[]
      Resulting interpolated ordinate values
   -> int Ni
      Number of interpolated values

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.2 $  $Date: 2003/05/09 02:29:38 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/MSmsg.h>


void
MSdIntLin (const double x[], const double y[], int N, const double xi[],
	   double yi[], int Ni)

{
  int i, k;
  double dx;

/* Check the reference abscissa values */
  for (k = 1; k < N; ++k) {
    if (x[k] < x[k-1])
      UThalt ("MSdIntLin: %s", MSM_NonIncrAbs);
  }

  /* Special case: only one reference point */
  if (N == 1) {
    for (i = 0; i < Ni; ++i) {
      if (xi[i] != x[0])
	UThalt ("MSdIntLin: %s: \"%g\"", MSM_BadAbsVal, xi[i]);
      yi[i] = y[0];
    }
  }

  /* General case: 2 or more reference points */
  /* N reference points -> N-1 intervals */
  else {
    for (i = 0; i < Ni; ++i) {
      /* Search for the bracketing interval */
      k = MSdLocate (xi[i], x, N);

      /* Handle the end-points */
      if (xi[i] == x[0])
	k = 1;
      if (xi[i] == x[N-1])
	k = N - 1;

      /* Check for out-of-range */
      if (k <= 0 || k >= N)
	UThalt ("MSdIntLin: %s: \"%g\"", MSM_BadAbsVal, xi[i]);

/* Interval found: x[k-1] <= xi[i] <= x[k], with 1 <= k <= N-1 */
      dx = x[k] - x[k-1];
      yi[i] = ((x[k]-xi[i])/dx) * y[k-1] + ((xi[i]-x[k-1])/dx) * y[k];
    }
  }
  return;
}
