/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void MSratio (double Val, long int *N, long int *D, double tol,
                long int MaxN, long int MaxD)

Purpose:
  Find a ratio of integers approximation to a floating point value

Description:
  A continued fraction expansion approximation to a given floating point
  value is calculated iteratively.  At successive stages in the expansion,
  the approximation error for the convergents alternates in sign but with
  diminishing magnitude.  The expansion is terminated when either two
  conditions is satisfied.
  1: The magnitude of either the numerator or the denominator of the
     approximation is larger than a given limit.  If so, the secondary
     convergents are tested to see if one provides a smaller error than
     backing off to the convergent from the previous iteration.
  2: The absolute value of the difference between the approximation (N/D) and
     the given number is less than a specified value (tol).
  If the input value is exactly a ratio of integers, this routine will find
  those integers if tol is set to zero, and MaxN and MaxD are appropriately
  large.

  Consider that case that the iteration is stopped by either the numerator
  exceeding MaxN or the denominator exceeding MaxD.  The resulting fraction
  N/D is the fraction nearest Val amongst those fractions with numerator and
  denominator not exceeding the given limits.

  References:
  R. B. J. T. Allenby and E. J. Redfern, "Introduction to Number Theory with
  Computing", Edward Arnold, 1989.

  I. Niven and H. S. Zuckerman, "An Introduction to the Theory of Numbers",
  4th ed., John Wiley & Sons, 1980.

Parameters:
   -> double Val
      Value to be approximated.  This value should have a magnitude between
      1/MaxD and MaxN.
  <-  long int N
      Numerator in the rational approximation to Val.  N is negative if Val is
      negative.
  <-  long int D
      Denominator in the rational approximation to Val.
   -> double tol
      Error tolerance used to terminate the approximation
   -> long int MaxN
      Maximum value for N.  This value should be at least floor(|Val|).
   -> long int MaxD
      Maximum value for D.  This value should be at least floor(1/|Val|).

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.7 $  $Date: 2003/05/09 02:29:38 $

-------------------------------------------------------------------------*/

#include <math.h>

#include <libtsp.h>

#define ABSV(x)		(((x) < 0) ? -(x) : (x))


void
MSratio (double Val, long int *N, long int *D, double tol, long int MaxN,
	 long int MaxD)

{
  double pk, pkm1, pkm2, qk, qkm1, qkm2;
  double AV, AVS, ak;
  double rk, sk;
  double m, mp, mq;

/*
  The continued fraction expansion of a value x is of the form
    x = a[0] +  1
               -----------------
               a[1] +  1
                      ----------
                      a[2] + ...

      = {a[0]; a[1], a[2], ... }
  The a[k] are integers (all positive, except possibly for a[0]).  The value
  a[0] can be obtained from x,
    a[0] = floor(x).
  The continued fraction term represents x1 = 1/(x - floor(x)),
    a[1] = floor(x1).
  This process can be continued until the remainder is zero (x is exactly a
  a ratio of integers) or indefinitely (x is irrational).

  The truncated continued fraction expansion approximates x, with the error
  decreasing as more terms are added.  The value of the continued fraction with
  terms up to a[k] is the convergent C[k],
                                      p[k]
    C[k] = {a[0]; a[1], ... , a[k]} = ---- .
                                      q[k]
  The numerator and denominator satisfy the relationships (k >= 0)
    p[k] = a[k] p[k-1] + p[k-2],  with initial values p[-2] = 0, p[-1] = 1
    q[k] = a[k] q[k-1] + q[k-2],  with initial values q[-2] = 1, q[-1] = 0
  The even convergents C[0], C[2], C[4], ... form an increasing sequence that
  approaches the value x from below, while the odd convergents C[1], C[3}, ...
  form a decreasing sequence which approaches x from above.

  Properties of the convergent C[k] = p[k]/q[k]:
   1: The convergent C[k] has the property that among fractions with
      denominator less than or equal to q[k], the C[k] is the one nearest to x.
   2: The fraction C[k] is in reduced form, i.e. p[k] is relatively prime to
      q[k].

  The secondary convergents lie between two even or two odd convergents,
             p[n-1]  p[n-1]+p[n]  p[n-1]+2p[n]
    C[n-1] = ------, -----------, ------------, ... ,
             q[n-1]  q[n-1]+q[n]  q[n-1]+2q[n]

               p[n-1]+a[n+1]p[n]   p[n+1]
               ----------------- = ------ = C[n+1]
               q[n-1]+a[n+1]q[n]   q[n+1]
  The secondary convergents are either increasing (n odd) or decreasing
  (n even).  The denominators are increasing in this sequence.  No other
  fraction can be placed into this sequence and maintain these properties.

  The closest fraction to x which has a denominator less than a given value M
  is either a convergent or a secondary convergent.
*/

/* Initialization */
  pkm1 = 0.0;
  pk = 1.0;
  qkm1 = 1.0;
  qk = 0.0;		/* C[-1] = 1/0 */

  AVS = ABSV (Val);
  AV = AVS;

/* Main loop for the continued fraction expansion */
  while (1) {
    ak = floor (AV);

    /* Recursive calculation of the numerator and denominator */
    /* (all terms are integer-valued and positive) */
    pkm2 = pkm1;
    pkm1 = pk;
    pk = ak * pkm1 + pkm2;
    qkm2 = qkm1;
    qkm1 = qk;
    qk = ak * qkm1 + qkm2;

    if (pk > MaxN || qk > MaxD) {

      pk = pkm1;	/* Back off to the previous convergent */
      qk = qkm1;

      /* Try a secondary convergent */
      mp = floor ((MaxN - pkm2) / pkm1);
      mq = floor ((MaxD - qkm2) / qkm1);
      m = (mp <= mq) ? mp : mq;
      if (m > 0) {
	rk = m * pkm1 + pkm2;
	sk = m * qkm1 + qkm2;
	if (ABSV (AVS - rk/sk) < ABSV (AVS - pk/qk)) {
	  pk = rk;
	  qk = sk;
	}
      }
      break;
    }

    /* Check the approximation error */
    if (ABSV (AVS - pk / qk) <= tol)
      break;

    /* Next term
       (AV should be positive, since the denominator is (AV - floor(AV)).
       If it is negative, we are being limited by machine precision.)
    */
    AV = 1.0 / (AV - ak);
    if (AV < 0.0)
      break;
  }

  if (Val < 0.0)
    pk = -pk;
  *N = (long int) pk;
  *D = (long int) qk;

  return;
}
