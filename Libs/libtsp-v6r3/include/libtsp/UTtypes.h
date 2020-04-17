/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  UTtypes.h

Description:
  Type declarations for size dependent storage elements

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.13 $  $Date: 2003/04/27 03:11:19 $

----------------------------------------------------------------------*/

#ifndef UTtypes_h_
#define UTtypes_h_

#include <float.h>		/* DBL_MAX, FLT_MAX */
#include <limits.h>

typedef double double8_t;		/* 8-byte double */
typedef float float4_t;			/* 4-byte float */
#define DOUBLE8_MAX	DBL_MAX
#define FLOAT4_MAX	FLT_MAX

/*
 Assume that the lengths of integers are as follows
 (short int, int, long int) are (2,2,4), (2,4,4) or (2,4,8) bytes
*/
#if (INT_MAX == SHRT_MAX)	/* (2,2,4) case */
   typedef unsigned long int uint4_t;	/* 4-byte unsigned int */
   typedef long int int4_t;		/* 4-byte int */
#  define UINT4_MAX	ULONG_MAX
#  define INT4_MAX	LONG_MAX
#  define INT4_MIN	LONG_MIN
#else
   typedef unsigned int uint4_t;	/* 4-byte unsigned int */
   typedef int int4_t;			/* 4-byte int */
#  define UINT4_MAX	UINT_MAX
#  define INT4_MAX	INT_MAX
#  define INT4_MIN	INT_MIN
#endif

typedef unsigned short int uint2_t;	/* 2-byte unsigned int */
typedef short int int2_t;		/* 2-byte int */
typedef unsigned char uint1_t;		/* 1-byte unsigned int */
typedef signed char int1_t;		/* 1-byte signed int */
#define UINT2_MAX	USHRT_MAX
#define INT2_MAX	SHRT_MAX
#define INT2_MIN	SHRT_MIN
#define UINT1_MAX	UCHAR_MAX
#define INT1_MAX	SCHAR_MAX
#define INT1_MIN	SCHAR_MIN

#endif	/* UTtypes_h_ */
