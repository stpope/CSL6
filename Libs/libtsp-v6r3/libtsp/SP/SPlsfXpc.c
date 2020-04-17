/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void SPlsfXpc (const float lsf[], float pc[], int Np)

Purpose:
  Convert LSF's to predictor coefficients

Description:
  This routine converts a vector of line spectral frequencies to the
  corresponding vector of predictor coefficients.  The LSF's are the
  frequencies (angles) corresponding to roots on the unit circle.  These roots
  are the roots of two polynomials, one symmetric, the other anti-symmetric.
  The odd-numbered LSF's specify roots of one polynomial, while the even-
  numbered LSF's specify the roots of the other polynomial.  The polynomials
  are formed by the polynomial multiplication of the factors representing
  conjugate pairs of roots.  The predictor coefficients are formed by the
  addition of the two polynomials.

  Line spectral frequencies and predictor coefficients are usually expressed
  algebraically as vectors with one-offset indexing.  The correspondence to the
  zero-offset C-arrays is as follows.
    l(1) <==> lsf[0]       first (lowest frequency) line spectral frequency
    l(i) <==> lsf[i-1]     1 <= i < Np
    p(1) <==> pc[0]        predictor coefficient corresponding to lag 1
    p(i) <==> pc[i-1]      1 <= i < Np

Parameters:
   -> const float lsf[]
      Array of Np line spectral frequencies.  Each line spectral frequency lies
      in the range 0 to pi.  If the LSF's are well-ordered (ascending order),
      the resulting predictor corresponds to a minimum phase prediction error
      filter.
   -> int Np
      Number of coefficients (at most 50)
  <-  float pc[]
      Output array of Np predictor coefficients.  These are the coefficients
      of the predictor filter, with pc[0] being the predictor coefficient
      corresponding to lag 1, and pc[Np-1] corresponding to lag Np.
   -> int Np
      Number of coefficients (at most 50)

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.21 $  $Date: 2003/05/09 02:45:04 $

-------------------------------------------------------------------------*/

#include <math.h>
#include <libtsp.h>
#include <libtsp/SPmsg.h>


#define MAXNP		50

/* one-offset indexing */
#define PC(i)		pc[(i)-1]
#define LSF(i)		lsf[(i)-1]

static void
SP_SymPoly (double x[], const float lsf[], int start,int Np);


void
SPlsfXpc (const float lsf[], float pc[], int Np)

{
  double fa[(MAXNP+1)/2+1];
  double fb[MAXNP/2+1];
  int i, k, M;

/*
   Each line spectral frequency w contributes a second order polynomial of the
   form Y(D)=1-2*cos(w(i))*D+D^2.  These polynomials are formed for each line
   spectral frequency and then multiplied together.  Alternate line spectral
   frequencies are used to form two polynomials with interlacing roots on the
   unit circle.  These two polynomials are multiplied by 1+D and 1-D if Np is
   even or by 1 and 1-D^2 if Np is odd.  This gives the symmetric and anti-
   symmetric polynomials that are added to give the predictor coefficients.
*/
  if (Np > MAXNP)
    UThalt ("SPlsfXpc: %s", SPM_TooManyCoef);

/* Form a symmetric Fa(D) from w(1), w(3), ... */
/* Form a symmetric Fb(D) from w(2), w(4), ... */
  SP_SymPoly (fa, lsf, 1, Np);
  SP_SymPoly (fb, lsf, 2, Np);

/*
   Both Fa(D) and Fb(D) are symmetric, with leading coefficient equal to unity.
   Exclusive of the leading coefficient, the number of coefficients needed to
   specify Fa(D) and Fb(D) is:
     Np      Fa(D)     Fb(D)
    even     Np/2      Np/2
    odd    (Np+1)/2  (Np-1)/2
*/

  if (Np % 2 != 0) {

/* ----- Np odd */
/* Fb(D) is multiplied by the factor (1-D^2) */
    M = (Np - 1) / 2;
    for (i = M; i >= 2; --i)
      fb[i] = fb[i] - fb[i-2];

/*
   Form the predictor filter coefficients.  Fa(D) is symmetric and Fb(D) is
   anti-symmetric.  Since only the first half of the coefficients are available
   symmetries are used to get the other half.
*/
    for (i = 1, k = Np; i <= M; ++i, --k) {
      PC(i) = (float) (-0.5 * (fa[i] + fb[i]));
      PC(k) = (float) (-0.5 * (fa[i] - fb[i]));
    }
    PC(i) = (float) (-0.5 * fa[i]);
  }

  else {

/* ----- Np even */
/* Fa(D) is multiplied by the factor (1+D) */
/* Fb(D) is multiplied by the factor (1-D) */
    M = Np / 2;
    for (i = M; i >= 1; --i) {
      fa[i] = fa[i] + fa[i-1];
      fb[i] = fb[i] - fb[i-1];
    }

/* Form the predictor filter coefficients */
/* Fa(D) is symmetric and Fb(D) is anti-symmetric. */
    for (i = 1, k = Np; i <= M; ++i, --k) {
      PC(i) = (float) (-0.5 * (fa[i] + fb[i]));
      PC(k) = (float) (-0.5 * (fa[i] - fb[i]));
    }
  }
  return;
}

/*
   Form a symmetric polynomial from its roots

   Consider a symmmetric polynomial with an odd number of coefficients (2N+1).
   This polynomial can be specified by N+1 coefficients,
   f(0), f(1),...,f(N).  Let
     F(D) = f(0) + f(1) D + f(2) D^2 + ... + f(2N) D^(2N)
   Then if this polynomial is monic and symmetric,
     f(0) = f(2N)      (symmetric)
     f(1) = f(2N-1)
      ...    ...
     f(n) = f(2N-n)    (general term)
      ...    ...
     f(N-1) = f(N+1)
     f(N)              (not paired)
   Consider a monic symmetric polynomial with 3 coefficients,
     Y(D) = 1 + a D + D^2
   The convolution of the coefficients of X(D) and Y(D) gives another
   symmetric polynomial with an odd number of coefficients.  Let
   Z(D) = Y(D) f(D), then
     z(0) = f(0)
     z(1) = f(1) + a f(0)
     z(2) = f(2) + a f(1) + f(0)
     z(3) = f(3) + a f(2) + f(1)
     ...     ...
     z(n) = f(n) + a f(n-1) + f(n-2)     (general term)
     ...     ...
     z(N-1) = f(N-1) + a f(N-2) + f(N-3)
     z(N)   = f(N) + a f(N-1) + f(N-2)
     z(N+1) = f(N+1) + a f(N) + f(N-1)   (not paired)
     ...

   Notes:
    1) z(0) need not be calculated
    2) terms z(N+2),...,z(2N+2) need not be calculated since they can be
       obtained by symmetry
    3) terms z(2),...z(N) are of the same form as the general term.  If we
       set f(N+1) = f(N-1) (symmetry), then the term z(N+1) is also of this
       form.
    4) By choosing the order of the operations appropriately, the result can
       overlay the input array f(.).
    5) The polynomial should be double.  For 35 or more uniformly spaced LSF's,
       the resulting PC's are non-minimum phase if the intermediate
       calculations are done in single precision.
*/


static void
SP_SymPoly (double x[], const float lsf[], int start, int Np)

{
  int i, k, n;
  double A;

  x[0] = 1.0;
  if (start <= Np) {
    x[1] = 0.0;		/* for n=0 */
    for (n = 0, i = start; i <= Np; i += 2, ++n) {
      A = -2.0 * cos ((double) LSF(i));
      if (n > 0)
	x[n+1] = x[n-1];
      for (k = n+1; k >= 2; --k)
	x[k] = x[k] + A * x[k-1] + x[k-2];
      x[1] = x[1] + A;	/* Actually x[1] + A*x[0], but x[0] = 1.0 */
    }
  }
  return;
}
