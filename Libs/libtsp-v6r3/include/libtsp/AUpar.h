/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  AUpar.h

Description:
  Declarations for AU audio file headers

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.27 $  $Date: 2003/05/13 01:48:11 $

----------------------------------------------------------------------*/

#ifndef AUpar_h_
#define AUpar_h_

#include <libtsp/UTtypes.h>		/* typedef for uint4_t */

/* The AU header size is mandated to be at least 28 bytes long (the last 4
   bytes being comments).  However there are files extant which have only 24
   byte headers.
*/
#define AU_LHMIN	24

/* File magic values in file byte order */
#define	FM_AU		".snd"

enum {
  AU_UNSPEC	= 0,	/* unspecified format */
  AU_MULAW8	= 1,	/* mu-law 8-bit data */
  AU_LIN8	= 2,	/* linear two's complement 8-bit data */
  AU_LIN16	= 3,	/* linear two's complement 16-bit data */
  AU_LIN24	= 4,	/* linear two's complement 24-bit data */
  AU_LIN32	= 5,	/* linear two's complement 32-bit data */
  AU_FLOAT32	= 6,	/* IEEE 32-bit float data (-1. to +1.) */
  AU_DOUBLE64	= 7,	/* IEEE 64-bit float data */
  AU_G721	= 23,	/* CCITT G.721 4-bit ADPCM */
  AU_G722	= 24,	/* CCITT G.722 */
  AU_G723_3	= 25,	/* CCITT G.723 3-bit ADPCM */
  AU_G723_5	= 26,	/* CCITT G.723 5-bit ADPCM */
  AU_ALAW8	= 27	/* A-law 8-bit data */
};

#define	AU_NOSIZE	(~((uint4_t) 0))	/* Indeterminate data length */

struct AU_head {
  char Magic[4];	/* File magic */
  uint4_t Lhead;	/* Length of header in bytes */
  uint4_t Ldata;	/* Length of data portion in bytes */
  uint4_t Dencod;	/* Encoding type */
  uint4_t Srate;	/* Sampling rate */
  uint4_t Nchan;	/* Number of channels */
  char AFspID[4];	/* AFsp identifier */
};

#endif	/* AUpar_h_ */
