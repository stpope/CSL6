/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double MSdPolyInt (double x, const double xa[], const double ya[], int N,
                     double buff[])

Purpose:
  Polynomial interpolation given a set of points

Description:
  Given a set of points (xa and ya values), this routine evaluates the
  polynomial passing through those points.  The calculation uses
  Neville's algorithm.
    W. H. Press, S. A. Teukolsky, W. T. Vetterling,  and B. P. Flannery,
    "Numerical Recipes in C", 2nd Ed., Cambridge University Press, 1992.
    P. Burger, D. Gillies, "Interactive Computer Graphics", Addison-Wesley,
    1989.

  The computational complexity is proportional to N^2.  If the same polynomial
  is to be evaluated at several points, other schemes which pre-compute a set
  of coefficients for the polynomial may be preferable.

Parameters:
  <-  double MSdPolyInt
      Value of the polynomial evaluated at x
   -> double x
      Evaluation point
   -> const double xa[]
      Array of N abscissa values
   -> const double ya[]
      Array of N ordinate values
   -> int N
      Number of reference points
   -> double buff[]
      Buffer (2N values) for use internally.  If buff is set to NULL,
      a temporary buffer will be created internally.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.3 $  $Date: 2003/05/09 02:29:38 $

-------------------------------------------------------------------------*/

#include <math.h>
#include <float.h>

#include <libtsp.h>

/*
  This routine uses Neville's algorithm.  The reference points are (x(i), y(i))
  for i = 0, ... , N-1.  The polynomial P[m](i) is a polynomial in x which
  interpolates the m points at x(i), ... , x(i+m-1).  The values can be
  arranged in a tableau.

    m=1           m=2           m=3          ...        m=N-1         m=N
  P[1](N-1) -
             \                               P[m-1](i+1) ------
              \                                                \ D1[m](i)
  P[1](N-2) --- P[2](N-2) -                                     \
             \             \                   P[m-1](i) -------- P[m](i)
              \             \                            D0[m](i)
  P[1](N-3) --- P[2](N-3) --- P[3](N-3) -
             \             \             \
   ...           ...           ...
              \             \             \
  P[1](3)   --- P[2](2)   --- P[3](2)   ---  ...  -
             \             \             \         \
              \             \             \         \
  P[1](1)   --- P[2](1)   --- P[3](1)   ---  ...  --- P[N-1](1) -
             \             \             \         \             \
              \             \             \         \             \
  P[1](0)   --- P[2](0)   --- P[3](0)   ---  ...  --- P[N-1](0) --- P[N](0)

  For stage m (m point interpolation), the interpolation polynomials are
  P[m](i) for i = 0, ... , N-m.

  The initial conditions (1-point interpolation) are
    P[1](i) = y(i),  for i = 0, ... , N-1.
  The iteration to calculate column m+1 from column m is
                (x - x(i+m)) P[m](i) + (x(i) - x) P[m](i+1)
    P[m+1](i) = -------------------------------------------   i=0,...,N-m-1
                            x(i) - x(i+m)
  Given that polynomial P[m](i) passes through the m values at x(i), ... ,
  x(i+m-1), and P[m](i+1) passes through the m values at x(i+1), ... ,
  x(i+m), then P[m+1](i) passes through the m+1 values at x(i), ..., x(i+m).

  Define the column difference values (m = 2, ... , N),
    D0[m](i) = P[m](i) - P[m-1](i)       for i = 0, ... , N-m
    D1[m](i) = P[m](i) - P[m-1](i+1)

  The differences at stage m+1 are
    D0[m+1](i) = P[m+1](i) - P[m](i)
    D1[m+1](i) = P[m+1](i) - P[m](i+1)
  Subtracting these,
    D1[m+1](i) - D0[m+1](i) = P[m](i) - P[m](i+1)                (1)
  Using the recursion for P[m+1](i) from above,
    D0[m+1](i) + D1[m+1](i)
         = 2 P[m+1](i) - P[m](i) - P[m](i+1)
           2x - x(i+m) - x(i)
         = ------------------ (P[m](i) - P[m](i+1))              (2)
             x(i) - x(i+m)
  Adding and subtracting (1) and (2),
                    x - x(i)
    D0[m+1](i) = ------------- (P[m](i) - P[m](i+1))             (3)         
                 x(i) - x(i+m)
                   x - x(i+m)
    D1[m+1](i) = ------------- (P[m](i) - P[m](i+1))             (4)
                 x(i) - x(i+m)
  Now consider
    D0[m](i+1) = P[m](i+1) - P[m-1](i+1)
    D1[m](i)   = P[m](i) - P[m-1](i+1)
  Then
    P[m](i) - P[m](i+1) = D1[m](i) - D0[m](i+1)                  (5)
  Substituting (5) into (3) and (4) gives
                   x - x(i)
    D0[m+1](i) = ------------- (D1[m](i) - D0[m](i+1))
                 x(i) - x(i+m)
                   x - x(i+m)
    D1[m+1](i) = ------------- (D1[m](i) - D0[m](i+1))
                 x(i) - x(i+m)

  The final value can be calculated as a sum of differences through the
  tableau.  Starting at any point P[0](i), the interpolated value at x
  is
    P[N](0) = P[1](i) + Dn[2](.) + ... + Dn[N](.)
  The actual differences used in this calculation depend on the path
  taken through the tableau.  There is no requirement that the input points
  be ordered, though in many practical instances they will be.  If the
  evaluation point lies between two reference points, we will choose a path
  through the tableau that tries to keep the evalution polynomials centered
  on this value.
  1.  Choose the starting value to be the value y(i) corresponding to x(i)
      where x(i) is the grid value closest to the evaluation point x.
      Denote the corresponding point to be I.
  2.  Determine whether x is closer to x(I-1) or x(I+1).  Denote the result
      as I- or I+.
  3.  The polynomial P[m](i) interpolates values i, i+1, ... , i+m-1.  It
      is centered around point i+(m-1)/2 for m odd.  We can choose i such
      that that P[m}(i) is centered around point I.  For m even, we have a
      choice of two polynomials that are approximately centered.  The
      selection will be made depending on which situation is true: I- or I+.
  4.  The path from I through the tableau will consist of alternating
      horizontal and diagonal lines.  Eventually the path will be limited
      by the i=0 lower limit or i=N-m upper limit.
*/


double
MSdPolyInt (double x, const double xa[], const double ya[], int N,
	    double buff[])

{
  int i, m, I, H;
  double den, d, dt, h0, hm, dy, y;
  double *D0, *D1;

  if (buff == NULL) {
    D0 = (double *) UTmalloc (2 * N * sizeof (double));
    D1 = D0 + N;
  }
  else {
    D0 = buff;
    D1 = buff + N;
  }

  /* Find the nearest abscissa value to x */
  I = 0;
  d = DBL_MAX;
  for (i = 0; i < N; ++i) {
    dt = fabs(x - xa[i]);
    if (dt < d) {
      I = i;
      d = dt;
    }
    D1[i] = ya[i];
    D0[i] = ya[i];
  }

  /* Decide whether the first piece of the path is horizontal or diagonal  */
  y = ya[I];
  if (I == 0)
    H = 1;
  else if (I == N-1)
    H = 0;
  else if (fabs(x - xa[I-1]) < fabs(x - xa[I+1]))
    H = 0;
  else
    H = 1;

  for (m = 2; m <= N; ++m) {

    /* Update a column in the tableau */
    for (i = 0; i <= N - m; ++i) {

      h0 = x - xa[i];
      hm = x - xa[i+m-1];
      den = hm - h0;
      if (den == 0.0)
	UTerror ("MSdPolyInt: Abscissa values not distinct");
      D0[i] = h0 * (D1[i] - D0[i+1]) / den;
      D1[i] = hm * (D1[i] - D0[i+1]) / den;
    }

    /* Add an increment to the value, moving horizontally or diagonally */
    if ((H && I > 0) || I > N - m) {
      I = I - 1;	/* Diagonal segment */
      dy = D1[I];
    }
    else
      dy = D0[I];	/* Horizontal segment */

    y += dy;
    H = ~H;		/* Alternate horizontal and diagonal moves */
  }

  /* Deallocate buffers */
  if (buff == NULL)
    UTfree (D0);

  return y;
}
