/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void SPpcXlsf (const float pc[], float lsf[], int Np)

Purpose:
  Convert predictor coefficients to line spectral frequencies

Description:
  The transfer function of the prediction error filter is formed from the
  predictor coefficients.  This polynomial is transformed into two reciprocal
  polynomials having roots on the unit circle.  The roots of these polynomials
  interlace.  The line spectral frequencies are the angles corresponding
  to these roots.  The two reciprocal polynomials are expressed as series
  expansions in Chebyshev polynomials with real roots in the range -1 to +1.
  The inverse cosine of the roots of the Chebyshev polynomial expansion gives
  the line spectral frequencies.

  If Np line spectral frequencies are not found, this routine stops with an
  error message.  This error occurs if the input coefficients do not give a
  prediction error filter with minimum phase or the line spectral frequencies
  are too close together.

  The LSF search algorithm uses an initial coarse search with a resolution of
  0.01 pi (corresponding to 60 Hz for a 8000 Hz sampling rate).  This search
  alternates between the two Chebyshev polynomials.  Roots may be missed if
  two roots of the same polynomial are spaced closer than 0.01 pi apart.  The
  LSF's correspond to roots taken alternately from the two polynomials.  Then
  LSF's may be missed if two non-adjacent LSF's are separated by less than
  0.01 pi.  The present resolution is adequate for predictor coefficients
  derived from speech signals.

  Line spectral frequencies and predictor coefficients are usually expressed
  algebraically as vectors with one-offset indexing.  The correspondence to the
  zero-offset C-arrays is as follows.
    l(1) <==> lsf[0]       first (lowest frequency) line spectral frequency
    l(i) <==> lsf[i-1]     1 <= i < Np
    p(1) <==> pc[0]        predictor coefficient corresponding to lag 1
    p(i) <==> pc[i-1]      1 <= i < Np

Parameters:
   -> const float pc[]
      Vector of predictor coefficients (Np values).  These are the coefficients
      of the predictor filter, with pc[0] being the predictor coefficient
      corresponding to lag 1, and pc[Np-1] corresponding to lag Np.  The
      predictor coefficients must correspond to a minimum phase prediction
      error filter.
  <-  float lsf[]
      Array of Np line spectral frequencies (in ascending order).  Each line
      spectral frequency lies in the range 0 to pi.
   -> int Np
      Number of coefficients (at most 50)

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.33 $  $Date: 2003/05/09 02:45:04 $

-------------------------------------------------------------------------*/

#include <math.h>

#include <libtsp.h>
#include <libtsp/SPmsg.h>

#define MAXNP	50
#define PI	3.14159265358979323846
#define NBIS	4		/* number of bisections */
#define DRES	0.0625F		/* 1/2^NBIS */
#define DW	(0.01 * PI)	/* resolution for the initial search */

/* one-offset indexing */
#define PC(i)		(pc[(i)-1])
#define LSF(i)		(lsf[(i)-1])

static void
SP_pcXcheb (const float pc[], int Np, float ta[], int *Nca, float tb[],
	    int *Ncb);
static float
SP_bisectRoot (float xlow, float ylow, float xhigh, float yhigh,
	       const float t[], int n);


void
SPpcXlsf (const float pc[], float lsf[], int Np)

{
  float t[2][(MAXNP+1)/2+1];
  float rc[MAXNP];
  float xlow, xhigh, ylow, yhigh, xmid, xroot;
  float dx, ss, aa;
  int ip, nf;
  int n[2];

  if (Np > MAXNP)
    UThalt ("SPpcXlsf: %s", SPM_TooManyCoef);

/*
   The LSF's are determined from the roots of two Chebyshev polynomials.
   The Chebyshev polynomials are found from the symmetric and anti-symmetric
   polynomials formed by adding and subracting the prediction error polynomial
   and its time reversal.  Let the Chebyshev polynomials be 
             Na-1                        Nb-1
     Ta(x) = SUM ta(n) T(n,x),   Tb(x) = SUM tb(n) T(n,x) .
             n=0                         n=0
   The domain of T.(x) is -1 < x < +1.  For a given root of T.(x), say x0,
   the corresponding line spectral frequency is acos(x0).
*/
/* Find the coefficients of the Chebyshev polynomials */
  SP_pcXcheb (pc, Np, t[0], &n[0], t[1], &n[1]);

/*
   To find the roots, we sample the polynomials Ta(x) and Tb(x) looking for
   sign changes.  Since the roots of the two polynomials interlace, the search
   for roots alternates between the polynomials Ta(x) and Tb(x).  The sampling
   interval must be small enough to avoid having two sign changes in the same
   interval.  Consider specifying the resolution in the LSF domain.  For an
   interval [xl, xh], the corresponding interval in frequency is [w1, w2], with
   xh=cos(w1) and xl=cos(w2) (note the reversal in order).  Let dw=w2-w1,
     dx = xh-xl = xh [1-cos(dw)] + sqrt(1-xh*xh) sin(dw).
   The calculation of the square root is overly time-consuming.  If instead, we
   use equal steps in the x-domain, the resolution in the LSF domain is best at
   at pi/2 and worst at 0 and pi.  As a compromise we fit a quadratic to the
   step size relationship.  At x=1, dx=(1-cos(dw); at x=0, dx=sin(dw).  Then
   the approximation is
     dx' = (a(1-cos(dw))-sin(dw)) x^2 + sin(dw)).
   For a=1, this value underestimates the step size for all x.  However, the
   step size for just below x=1 and just above x=-1 are well below the target
   step size.  To compensate for this, we use a=4.  Then at x=+1 and x=-1, the
   step sizes are too large by a factor of 4, but rapidly fall to about 60% of
   the desired values and then rise slowly to become equal to the desired step
   size at x=0.
*/
  nf = 0;
  ip = 0;	/* ip=0 for ta, ip=1 for tb */
  xroot = 2.0F;
  xlow = 1.0F;
  ylow = (float) FNevChebP (xlow, t[ip], n[ip]);

/*
   Define the step-size function parameters
   The resolution in the LSF domain is at least DW/2^NBIS (except near
   zero and pi), not counting the increase in resolution due to the linear
   interpolation step.  For DW=0.01*pi, and NBIS=4, and a sampling frequency
   equal to 8000, this corresponds to 2.5 Hz.
*/
  ss = (float) sin (DW);
  aa = 4.0F - 4.0F * (float) cos(DW)  - ss;

/* Root search loop */
  while (xlow > -1.0F && nf < Np) {

    /* New trial point */
    xhigh = xlow;
    yhigh = ylow;

    dx = aa * xhigh * xhigh + ss;
    xlow = xhigh - dx;
    if (xlow < -1.0F)
      xlow = -1.0F;
    ylow = (float) FNevChebP (xlow, t[ip], n[ip]);

    if (ylow * yhigh <= 0.0F) {
      xmid = SP_bisectRoot (xlow, ylow, xhigh, yhigh, t[ip], n[ip]);
      ++nf;
      LSF(nf) = (float) acos (xmid);

      /* Start the search for the roots of the next polynomial at the estimated
	 location of the root just found.  We have to catch the case that the
	 two polynomials have roots at the same place to avoid getting stuck at
	 that root.
      */
      if (xmid >= xroot)
	xmid = xlow - dx * DRES;
      xroot = xmid;
      ip = 1 - ip;	/* Flip the search from ta to tb or vice-versa */
      xlow = xmid;
      ylow = (float) FNevChebP (xlow, t[ip], n[ip]);
    }
  }

/* Error if Np roots have not been found */
  if (nf != Np) {
    SPpcXrc (pc, rc, Np);   /* Prints a message if non-minimum phase */
    UTwarn ("SPpcXlsf: %s", SPM_NotAllLSF);			// STP change, used to be UThalt
//    UThalt ("SPpcXlsf: %s", SPM_NotAllLSF);
  }
}

/* Convert predictor coefficients to Chebyshev polynomial coefficients
   (returns coefficients scaled by 0.5) */

static void
SP_pcXcheb (const float pc[], int Np, float ta[], int *Nca, float tb[],
	    int *Ncb)

{
  int odd;
  int i, j;
  int na, nb;
  float fa[(MAXNP+1)/2+1], fb[(MAXNP+1)/2+1];

/* Determine the number of coefficients in ta(.) and tb(.) */
  odd = (Np % 2 != 0);
  if (odd) {
    nb = (Np + 1) / 2;
    na = nb + 1;
  }
  else {
    nb = Np / 2 + 1;
    na = nb;
  }

/*
   Let D=z^(-1), the unit delay; the predictor filter with N coefficients is
             N        n
     P(D) = SUM p(n) D  .
            n=1
   The error filter polynomial is A(D)=1-P(D) with N+1 terms.  Two auxiliary
   polynomials are formed from the error filter polynomial,
     Fa(D) = A(D) + D^(N+1) A(D^(-1))  (N+2 terms, symmetric)
     Fb(D) = A(D) - D^(N+1) A(D^(-1))  (N+2 terms, anti-symmetric)
*/
  fa[0] = 1.0F;
  for (i = 1, j = Np; i < na; ++i, --j)
    fa[i] = -PC(i) - PC(j);

  fb[0] = 1.0F;
  for (i = 1, j = Np; i < nb; ++i, --j)
    fb[i] = -PC(i) + PC(j);

/*
    N even,  Fa(D)  includes a factor 1+D,
             Fb(D)  includes a factor 1-D
    N odd,   Fb(D)  includes a factor 1-D^2
  Divide out these factors, leaving even order symmetric polynomials, M is the
  total number of terms and Nc is the number of unique terms,
      N       polynomial            M         Nc=(M+1)/2
    even,  Ga(D) = F1(D)/(1+D)     N+1          N/2+1
           Gb(D) = F2(D)/(1-D)     N+1          N/2+1
    odd,   Ga(D) = F1(D)           N+2        (N+1)/2+1
           Gb(D) = F2(D)/(1-D^2)    N          (N+1)/2
*/
  if (odd) {
    for (i = 2; i < nb; ++i)
      fb[i] = fb[i] + fb[i-2];
  }
  else {
    for (i = 1; i < na; ++i) {
      fa[i] = fa[i] - fa[i-1];
      fb[i] = fb[i] + fb[i-1];
    }
  }

/*
   To look for roots on the unit circle, Ga(D) and Gb(D) are evaluated for
   D=exp(jw).  Since Ga(D) and Gb(D) are symmetric, they can be expressed in
   terms of a series in cos(nw) for D on the unit circle.  Since M is odd and
   D=exp(jw)
             M-1        n
     Ga(D) = SUM fa(n) D             (symmetric, fa(n) = fa(M-1-n))
             n=0
                                      Mh-1
           = exp(j Mh w) [ f1(Mh) + 2 SUM fa(n) cos((Mh-n)w) ]
                                      n=0
                         Mh
           = exp(j Mh w) SUM ta(n) cos(nw) ,
                         n=0

   where Mh=(M-1)/2=Nc-1.  The Nc=Mh+1 coefficients ta(n) are defined as

     ta(n) =   fa(Nc-1) ,    n=0,
           = 2 fa(Nc-1-n) ,  n=1,...,Nc-1.
   The next step is to identify cos(nw) with the Chebyshev polynomial T(n,x).
   The Chebyshev polynomials satisfy the relationship T(n,cos(w)) = cos(nw).
   Omitting the exponential delay term, the series expansion in terms of
   Chebyshev polynomials is
             Nc-1
     Ta(x) = SUM ta(n) T(n,x)
             n=0
   The domain of Ta(x) is -1 < x < +1.  For a given root of Ta(x), say x0,
   the corresponding position of the root of Fa(D) on the unit circle is
   exp(j acos(x0)).

   Calculate ta'(n) = 0.5 ta(n) and tb'(n) = 0.5 tb(n) to avoid the multiplies
   by the factor 2.  This of course leaves the roots unaffected.
*/
  ta[0] = 0.5F * fa[na-1];
  for (i = 1, j = na - 2; i < na; ++i, --j)
    ta[i] = fa[j];

  tb[0] = 0.5F * fb[nb-1];
  for (i = 1, j = nb - 2; i < nb; ++i, --j)
    tb[i] = fb[j];

  *Nca = na;
  *Ncb = nb;
}

/* Resolve a root in an interval with a sign change */

static float
SP_bisectRoot (float xlow, float ylow, float xhigh, float yhigh,
	       const float t[], int n)

{
  int i;
  float dx, xmid, ymid;

/* Successively bisect the interval containing the root */
  dx = xhigh - xlow;
  for (i = 1; i <= NBIS; ++i) {
    dx = 0.5F * dx;
    xmid = xlow + dx;
    ymid = (float) FNevChebP (xmid, t, n);
    if (ylow * ymid <= 0.0F) {
      yhigh = ymid;
      xhigh = xmid;
    }
    else {
      ylow = ymid;
      xlow = xmid;
    }
  }

/*
   Linear interpolation in the subinterval with a sign change
   (take care if yhigh=ylow=0)
*/
  if (yhigh != ylow)
    xmid = xlow + dx * ylow / (ylow - yhigh);
  else
    xmid = xlow + 0.5F * dx;

  return xmid;
}
