/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  FImsg.h

Description:
  Message texts for the TSP FI routines.

Author / revision:
  P. Kabal  Copyright (C) 2002
  $Revision: 1.5 $  $Date: 2002/06/26 18:37:52 $

----------------------------------------------------------------------*/

#ifndef FImsg_h_
#define FImsg_h_

/* I/O errors */
#define FIM_OpenRErr	"Error opening input file"

/* Error messages */
#define FIM_BadWAdv	"Invalid window advance value"
#define FIM_BadWPar	"Invalid window parameter(s)"
#define FIM_IIRNCoefErr	"IIR filter: No. coef. must be a multiple of 5"
#define FIM_NonCausal	"Non-causal filter, first coefficient is zero"

/* Filter file information printout */
#define FIMF_FiltFile	" Filter file: %s\n"
#define FIMF_FiltType	"   %s  %s\n"
#define FIMF_FiltSubType "   %s (%s)  %s\n"
#define FIMF_NumCoef	"   Number of coefficients: %d\n"
#define FIMF_NumIIR1	"   Number of coefficients: %d  (1 section)\n"
#define FIMF_NumIIRN	"   Number of coefficients: %d  (%d sections)\n"

#define FIM_FiltDirect	"direct form"
#define FIM_FiltSym	"direct form, symmetric"
#define FIM_FiltAsym	"direct form, anti-symmetric"
#define FIM_FiltBiquad	"cascade biquad sections"

/* Filter type descriptions */
#define FI_UNDEF_desc	"Undefined filter type"
#define FI_FIR_desc	"FIR filter"
#define FI_IIR_desc	"IIR filter"
#define FI_ALL_desc	"All-pole filter"
#define FI_WIN_desc	"Window coefficients"
#define FI_CAS_desc	"Analog filter"

#endif	/* FImsg_h_ */
