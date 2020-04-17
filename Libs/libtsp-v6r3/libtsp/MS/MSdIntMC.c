/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void MSdIntMC (const double x[], const double y[], int N, const double xi[],
		 double yi[], int Ni)

Purpose:
  Interpolate a table of values using a piecewise monotonic cubic interpolant

Description:
  This routine calculates interpolated values for a function defined by a table
  of reference data values.  The interpolated values are found by passing a
  piecewise monotonic cubic interpolant through the reference data values.  The
  monotonicity property guarantees that the interpolated data values do not go
  outside the range of the bracketing reference data values.

  The procedure defines the cubic interpolant between any two adjacent
  reference points in terms of the values and the derivatives at the reference
  points.  The derivatives at the reference points are determined by averaging
  the inverse slopes of the straight line segments joining the reference
  points to the left and right of a given reference point.  These slope values
  are changed if necessary to guarantee that the cubic interpolants are
  monotonic between reference points.  For instance a reference point is a
  local extremum (the slopes of the straight line segments joining reference
  points on either side of it are of different sign), the slope at that
  reference point is set to zero.  The slope of end points is determined from
  the slopes of the two straight lines joining the reference points beside the
  end point.

  References:
      Subroutine PCHIM, F. N. Fritsch, Lawrence Livermore National Laboratory.
      F. N. Fritsch and J. Butland, "A method for constructing local monotone
      piecewise cubic interpolants", SIAM J. Sci. Stat. Comput., vol. 5,
      pp. 300-304, June 1984.

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
      particular order, although some efficiency is gained if they are in
      increasing or decreasing order.
  <-  double yi[]
      Resulting interpolated ordinate values
   -> int Ni
      Number of interpolated values

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.5 $  $Date: 2003/11/11 18:45:35 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/MSmsg.h>


void
MSdIntMC (const double x[], const double y[], int N, const double xi[],
	  double yi[], int Ni)

{
  int i, k, kp;
  double dh, dl;

/* Check the reference abscissa values */
  for (k = 1; k < N; ++k) {
    if (x[k] < x[k-1])
      UThalt ("MSdIntMC: %s", MSM_NonIncrAbs);
  }

  /* Special case: only one reference point */
  if (N == 1) {
    for (i = 0; i < Ni; ++i) {
      if (xi[i] != x[0])
	UThalt ("MSdIntMC: %s: \"%g\"", MSM_BadAbsVal, xi[i]);
      yi[i] = y[0];
    }
  }

  /* General case: 2 or more reference points */
  /* N reference points -> N-1 intervals */
  else {
    kp = -1;
    dl = 0.0;	/* Define to stop compiler warnings */
    dh = 0.0;
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
	UThalt ("MSdIntMC: %s: \"%g\"", MSM_BadAbsVal, xi[i]);

/* Interval found: x[k-1] <= xi[i] <= x[k], with 1 <= k <= N-1 */
/*
   Choose the derivatives at the bracketing points and then evaluate the cubic
   at the interpolation point
*/
      if (kp == k - 1) {
	dl = dh;	/* dh has been defined previously */
	dh = MSdSlopeMC (k, x, y, N);
      }
      else if (kp == k + 1) {
	dh = dl;
	dl = MSdSlopeMC (k - 1, x, y, N);
      }
      else if (k != kp) {
	dl = MSdSlopeMC (k - 1, x, y, N);
	dh = MSdSlopeMC (k, x, y, N);
      }
      yi[i] = MSevalMC (xi[i], x[k-1], x[k], y[k-1], y[k], dl, dh);
      kp = k;
    }
  }
  return;
}
