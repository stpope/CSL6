/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  ESpar.h

Description:
  Declarations for ESPS sampled data feature file headers

Author / revision:
  P. Kabal  Copyright (C) 2001
  $Revision: 1.18 $  $Date: 2001/10/27 21:44:11 $

----------------------------------------------------------------------*/

#ifndef ESpar_h_
#define ESpar_h_

#include <libtsp/UTtypes.h>		/* typedef for uint4_t, etc */

#define	ES_LHMIN	333		/* start of generic items */
/* Generic item, "record freq"
   15             short
   3              short   generic item identifier
   record_freq\0  char    identifier (null terminated, multiple of 4 bytes)
   1              long    data count
   1              short   data type (double)
*/
#define	ES_MAXGENERIC	8192
#define ES_MAXINFO	256

/*  File magic value in file byte order.  ESPS files can be written in either
    big-endian or little-endian byte order.
*/
#define	FM_ESPS_BE     	"\0\0\152\32"	/* Big-endian data */
#define	FM_ESPS_LE	"\32\152\0\0"	/* Little-endian data */

/* File codes */
#define ES_FT_SD	9	/* (Old) Sampled data file */
#define ES_FT_FEA	13	/* Feature file */
#define ES_FEA_SPEC	7	/* Feature file - spectrum file subcode */
#define ES_FEA_SD	8	/* Feature file - sampled data file subcode */
#define ES_FEA_FILT	9	/* Feature file - filter file subcode */

/* Machine codes */
#define ES_SUN4_CODE	4	/* Sun sparc 4 machine code */
#define ES_DS3100_CODE	7	/* Decstation machine code */
#define ES_SG_CODE	9	/* SGI machine code */
#define ES_HP700_CODE	21	/* HP machine code */

/* Data codes for generic header items */
#define ES_DOUBLE	1
#define ES_FLOAT	2
#define ES_LONG		3
#define ES_SHORT	4

struct ES_preamb {
/*  int4_t Machine_code; */	/* machine which wrote file */
/*  int4_t Check_code;	 */	/* version check code (3000) */
  int4_t Data_offset;		/* data offset in bytes */
  int4_t Record_size;		/* record size */
  char Magic[4];		/* file magic */
/*  int4_t Edr; */		/* EDR_ESPS flag */
/*  int4_t Align_pad_size; */	/* alignment pad */
/*  int4_t Foreign_hd; */	/* pointer to foreign header */
};

struct ES_fixhead {
  int2_t Type;		/* File type (e.g. ES_FT_FEA) */
/*  int2_t pad1; */
  char Magic[4];	/* File magic */
/*  char Datetime[26]; */	/* File creation date */
/*  char Version[8];   */	/* Header version */
/*  char Prog[16];     */	/* Program name */
/*  char Vers[8];      */	/* Program version */
/*  char Progdate[26]; */	/* Program compile date */
  int4_t Ndrec;		/* Number of data records (often bogus) */
/*  int2_t Tag; */	/* Non-zero if data is tagged */
/*  int2_t nd1; */
  int4_t Ndouble;
  int4_t Nfloat;
  int4_t Nlong;
  int4_t Nshort;
  int4_t Nchar;
/*  int4_t Fixsiz; */	/* Fixed header structure size (40 longs) */
/*  int4_t Hsize; */	/* Variable header structure size (bytes) */
/*  char User[8]; */
/*  int2_t spares[10]; */
};

struct ES_FEAhead {
  int2_t Fea_type;	/* Feature file subcode (e.g. ES_FEA_SD) */
};

/* Structure for a Generic Item */
#define ES_MAX_gID	32
struct ES_genItem {
  uint2_t code;		/* Generic Item code */ 
  uint2_t ID_len;	/* Length of ID in 4-byte words */
  char ID[ES_MAX_gID];	/* ID */
  int4_t count;		/* Number of data items */
  uint2_t data_code;	/* Data type */
  /* data */
};

#endif	/* ESpar_h_ */
