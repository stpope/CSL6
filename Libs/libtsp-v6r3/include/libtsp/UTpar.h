/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  UTpar.h

Description:
  Declarations for the TSP utility routines.

Author / revision:
  P. Kabal  Copyright (C) 1997
  $Revision: 1.3 $  $Date: 1997/03/26 15:09:22 $

----------------------------------------------------------------------*/

#ifndef UTpar_h_
#define UTpar_h_

/* Machine and data byte order codes */
enum {
  DS_UNDEF	= -1,	/* undefined */
  DS_EB		= 0,	/* big-endian */
  DS_EL		= 1,	/* little-endian */
  DS_NATIVE	= 2,	/* native */
  DS_SWAP       = 3	/* byte-swapped */
};

#endif	/* UTpar_h_ */
