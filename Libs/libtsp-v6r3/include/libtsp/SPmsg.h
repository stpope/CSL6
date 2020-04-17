/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  SPmsg.h

Description:
  Message texts for the TSP SP routines.

Author / revision:
  P. Kabal  Copyright (C) 1997
  $Revision: 1.2 $  $Date: 1997/10/11 01:29:44 $

----------------------------------------------------------------------*/

#ifndef SPmsg_h_
#define SPmsg_h_

/* Error messages */
#define SPM_BadFrLen	"Invalid frame length"
#define SPM_BadTrLen	"Invalid transform length"
#define SPM_NegEnergy	"Negative signal energy"
#define SPM_NonPosDef	"Non-positive definite correlation values"
#define SPM_NonUnity	"Non-unity first error filter coefficient"
#define SPM_NotAllLSF	"Failed to find all LSFs"
#define SPM_TooManyCep	"Too many cepstral coefficients"
#define SPM_TooManyCoef	"Too many coefficients"

/* Warning messages */
#define SPM_FixNegEnergy "Fixup for negative error energy"
#define SPM_NonMinPhase	"Non-minimum phase prediction error filter"
#define SPM_RedOrder	"Using reduced order"

#endif	/* SPmsg_h_ */
