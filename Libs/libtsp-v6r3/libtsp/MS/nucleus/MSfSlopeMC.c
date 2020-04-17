/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double MSfSlopeMC (int k, const float x[], const float y[], int N)

Purpose:
  Calculate a slope at a point (for fitting a cubic)

Description:
  This routine finds a slope value that can be used to generate piecewise
  monotone cubic interpolants to the given data values.

  The derivatives at the reference points are determined by averaging the
  inverse slopes of the straight line segments joining the reference points to
  the left and right of a given reference point.  These slope values are
  changed if necessary to guarantee that the cubic interpolant is monotonic
  between reference points.  For instance if a reference point is a local
  extremum (the slopes of the straight line segments joining reference points
  on either side of it are of different sign), the slope at that reference
  point is set to zero.  The slope of end points is determined from the slopes
  of the two straight lines joining the reference points beside the end point.

Parameters:
  <-  double MSfSlopeMC
      Slope at point k
   -> int k
      Reference point at which the derivative is to be determined (0 to N-1)
   -> const float x[]
      Abscissa values for the reference points.  These values must be in
      increasing order.
   -> const float y[]
      Ordinate values for the reference points
   -> int N
      Number of reference points.  Only values in the neighborhood of
      reference point k will be used to determine the derivative.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.18 $  $Date: 2003/11/11 18:46:42 $

-------------------------------------------------------------------------*/

#include <assert.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>

#define ABSV(x)		(((x) < 0) ? -(x) : (x))

static double
MS_slMidMC (double Dxl, double Dxr, double Dyl, double Dyr);
static double
MS_slEndMC (double Dx, double Dxx, double Dy, double Dyy);


double
MSfSlopeMC (int k, const float x[], const float y[], int N)

{
  double S;

  S = 0.0;

/* Middle point */
  if (k > 0 && k < N - 1)
    S = MS_slMidMC ((double) (x[k] - x[k-1]), (double) (x[k+1] - x[k]),
		    (double) (y[k] - y[k-1]), (double) (y[k+1] - y[k]));

/* End point: beginning */
  else if (k == 0 && N > 2)
    S = MS_slEndMC ((double) (x[1] - x[0]), (double) (x[2] - x[1]),
		    (double) (y[1] - y[0]), (double) (y[2] - y[1]));

/* End point: end */
  else if (k == N - 1 && N > 2)
    S = MS_slEndMC ((double) (x[N-1] - x[N-2]), (double) (x[N-2] - x[N-3]),
		    (double) (y[N-1] - y[N-2]), (double) (y[N-2] - y[N-3]));

/* Special case: linear interpolation */
  else if (N == 2 && (k == 0 || k == 1))
    S = MS_slMidMC ((double) (x[1] - x[0]), (double) (x[1] - x[0]),
		    (double) (y[1] - y[0]), (double) (y[1] - y[0]));

/* Single reference point */
  else if (k == 0 && N == 1)
    S = 0.0;

  else
    assert (0);		/* Invalid input */
  
  return S;
}

/*
  Calculate a slope at a middle point

   -> double Dxl
      Increment in abscissa value to the left of the reference point
   -> double  Dxr
      Increment in abscissa value to the right of the reference point
   -> double Dyl
      Increment in ordinate value to the left of the reference point
   -> double Dyr
      Increment in ordinate value to the right of the reference point
*/


static double
MS_slMidMC (double Dxl, double Dxr, double Dyl, double Dyr)

{
  double Ds;
  double Smin, Smax, Sl, Sr;
  double a0, a1;

/* Check for increasing X values */
  assert (Dxl > 0.0 && Dxr > 0.0);

/* Sl is the slope to the left of point (x0,y0)
   Sr is the slope to the right of point (x0,y0) */
  Sl = Dyl / Dxl;
  Sr = Dyr / Dxr;

/* Use Brodlie modification of Butland formula (for same sign slopes)

              Sl Sr                  1       xl-x0
     d = --------------- , where a = - (1 + -------) .
         a Sl + (1-a) Sr             3      xl - xr

   This formula gives the harmonic mean of the slopes when x0-xl=xr-x0.  The
   factor a modifies the formula to take into account the relative spacing of
   the values (a varies from 1/3 to 2/3).  This formula satisfies the
   following properties.
   (a) the formula is invariant to an exchange of the the left and right
        points,
   (b) min(Sl,Sr) <= d <= max(Sl,Sr),
   (c) |d| <= 3 min(|Sl|,|Sr|).  This is a sufficient condition for
       monotonicity if d is calculated in this way for neighboring points.

  The following calculation is designed to avoid overflows, but is equivalent
  to the above formula.
         Sl Sr                  Smin
   ---------------  =  -------------------------
   a Sl + (1-a) Sr     a Sl/Smax + (1-a) Sr/Smax
  */

  if ((Sl > 0.0 && Sr > 0.0) || (Sl < 0.0 && Sr < 0.0)) {

    /* Slopes of the same sign */
    if (ABSV (Sl) > ABSV (Sr)) {
      Smax = Sl;
      Smin = Sr;
    }
    else {
      Smax = Sr;
      Smin = Sl;
    }
    Ds = Dxl + Dxr;
    a0 = (Ds + Dxl) / (3.0 * Ds);
    a1 = (Ds + Dxr) / (3.0 * Ds);
    return (Smin / ( a0 * (Sl / Smax) + a1 * (Sr / Smax) ));
  }

/*
   Slopes of opposite signs - force the derivative to zero to give a local
   maximum at the middle data point
*/
  else
    return 0.0;
}

/*
  Calculate a slope at an end point

   -> double Dx
      Increment in abscissa value for the interval next to the reference point
   -> double Dxx
      Increment in abscissa value for the interval one removed from the
      reference point
   -> double Dy
      Increment in ordinate value for the interval next to the reference point
   -> double Dyy
      Increment in ordinate value for the interval one removed from the
      reference point
*/


static double
MS_slEndMC (double Dx, double Dxx, double Dy, double Dyy)

{
  double Sr, Srr, S;
  double Ds;

/* Check for increasing X values */
  assert (Dx > 0.0 && Dxx > 0.0);

/* Sr is the slope to the right of point 0, Srr is the slope to the far
   right of point 0 */
  Sr = Dy / Dx;
  Srr = Dyy / Dxx;

/*
   Set the derivative using a non-centered three-point formula, adjusted to be
   shape-preserving
                                          x1-x0
     d = a Sr + (1-a) Srr , where a = 1 + ----- .
                                          x2-x0

   The factor a modifies the formula to take into account the relative spacing
   of the values (a varies from 2 to 1).
*/
  Ds = Dxx + Dx;
  S = Sr * ((Ds + Dx) / Ds) - Srr * (Dx / Ds);

/* Modify the value if necessary to preserve monotonicity */
  if ((S >= 0.0 && Sr <= 0.0) || (S <= 0.0 && Sr >= 0.0))
    S = 0.0;
  else if ((Sr > 0.0 && Srr < 0.0) || (Sr < 0.0 && Srr > 0.0)) {
    if (ABSV (S) > ABSV (3.0 * Sr))
      S = 3.0 * Sr;
  }

  return S;
}
