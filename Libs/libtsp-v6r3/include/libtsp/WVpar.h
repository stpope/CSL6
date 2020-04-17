/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  WVpar.h

Description:
  Declarations for WAVE file headers

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.28 $  $Date: 2003/05/13 01:48:11 $

----------------------------------------------------------------------*/

#ifndef WVpar_h_
#define WVpar_h_

#include <libtsp/UTtypes.h>		/* typedef for uint2_t, etc */

/* Format codes purged of many compressed formats */
#define WAVE_FORMAT_UNKNOWN		(0x0000)
#define WAVE_FORMAT_PCM			(0x0001) 
#define WAVE_FORMAT_ADPCM		(0x0002)
#define WAVE_FORMAT_IEEE_FLOAT		(0x0003)
	/*  IEEE754: range (+1, -1]  */
	/*  32-bit/64-bit format as defined by */
	/*  MSVC++ float/double type */
#define WAVE_FORMAT_ALAW		(0x0006)
#define WAVE_FORMAT_MULAW		(0x0007)
#define WAVE_FORMAT_G723_ADPCM		(0x0014)
#define WAVE_FORMAT_DOLBY_AC2		(0x0030)
#define WAVE_FORMAT_GSM610		(0x0031)
#define WAVE_FORMAT_G721_ADPCM		(0x0040)
#define WAVE_FORMAT_G728_CELP		(0x0041)
#define WAVE_FORMAT_MSG723		(0x0042)	/* Not in MMREG.H */
#define WAVE_FORMAT_MPEG		(0x0050)
#define WAVE_FORMAT_MPEGLAYER3		(0x0055)
#define WAVE_FORMAT_G726_ADPCM		(0x0064)
#define WAVE_FORMAT_G722_ADPCM		(0x0065)

#define WAVE_FORMAT_EXTENSIBLE          (0xFFFE)	/* extensible format */
#define WAVE_FORMAT_DEVELOPMENT         (0xFFFF)

/* Speaker Positions */
#define WV_SPEAKER_FRONT_LEFT              0x1
#define WV_SPEAKER_FRONT_RIGHT             0x2
#define WV_SPEAKER_FRONT_CENTER            0x4
#define WV_SPEAKER_LOW_FREQUENCY           0x8
#define WV_SPEAKER_BACK_LEFT               0x10
#define WV_SPEAKER_BACK_RIGHT              0x20
#define WV_SPEAKER_FRONT_LEFT_OF_CENTER    0x40
#define WV_SPEAKER_FRONT_RIGHT_OF_CENTER   0x80
#define WV_SPEAKER_BACK_CENTER             0x100
#define WV_SPEAKER_SIDE_LEFT               0x200
#define WV_SPEAKER_SIDE_RIGHT              0x400
#define WV_SPEAKER_TOP_CENTER              0x800
#define WV_SPEAKER_TOP_FRONT_LEFT          0x1000
#define WV_SPEAKER_TOP_FRONT_CENTER        0x2000
#define WV_SPEAKER_TOP_FRONT_RIGHT         0x4000
#define WV_SPEAKER_TOP_BACK_LEFT           0x8000
#define WV_SPEAKER_TOP_BACK_CENTER         0x10000
#define WV_SPEAKER_TOP_BACK_RIGHT          0x20000

/* Reserved for future use */
#define WV_SPEAKER_RESERVED                0x7FFC0000

/* Any speaker configuration */
#define WV_SPEAKER_ALL                     0x80000000

#define WV_SPEAKER_KNOWN \
  ( WV_SPEAKER_FRONT_LEFT           | WV_SPEAKER_FRONT_RIGHT           | \
    WV_SPEAKER_FRONT_CENTER         | WV_SPEAKER_LOW_FREQUENCY         | \
    WV_SPEAKER_BACK_LEFT            | WV_SPEAKER_BACK_RIGHT            | \
    WV_SPEAKER_FRONT_LEFT_OF_CENTER | WV_SPEAKER_FRONT_RIGHT_OF_CENTER | \
    WV_SPEAKER_BACK_CENTER          | WV_SPEAKER_SIDE_LEFT             | \
    WV_SPEAKER_SIDE_RIGHT           | WV_SPEAKER_TOP_CENTER            | \
    WV_SPEAKER_TOP_FRONT_LEFT       | WV_SPEAKER_TOP_FRONT_CENTER      | \
    WV_SPEAKER_TOP_FRONT_RIGHT      | WV_SPEAKER_TOP_BACK_LEFT         | \
    WV_SPEAKER_TOP_BACK_CENTER      | WV_SPEAKER_TOP_BACK_RIGHT )
  

#ifdef WV_CHANNEL_MAP
#define NELEM(array)	((int) ((sizeof array) / (sizeof array[0])))
#define WV_N_CHANNEL_MAP NELEM(WV_ChannelMap)
static const unsigned long int WV_ChannelMap[] =
{
  WV_SPEAKER_FRONT_LEFT,           WV_SPEAKER_FRONT_RIGHT,
  WV_SPEAKER_FRONT_CENTER,         WV_SPEAKER_LOW_FREQUENCY,
  WV_SPEAKER_BACK_LEFT,            WV_SPEAKER_BACK_RIGHT,
  WV_SPEAKER_FRONT_LEFT_OF_CENTER, WV_SPEAKER_FRONT_RIGHT_OF_CENTER,
  WV_SPEAKER_BACK_CENTER,          WV_SPEAKER_SIDE_LEFT,
  WV_SPEAKER_SIDE_RIGHT,           WV_SPEAKER_TOP_CENTER,
  WV_SPEAKER_TOP_FRONT_LEFT,       WV_SPEAKER_TOP_FRONT_CENTER,
  WV_SPEAKER_TOP_FRONT_RIGHT,      WV_SPEAKER_TOP_BACK_LEFT,
  WV_SPEAKER_TOP_BACK_CENTER,      WV_SPEAKER_TOP_BACK_RIGHT
};
#endif

struct WV_GUID {
  uint2_t wFormatTag;
  uint1_t guidx[14];
};

#ifdef WAVEFORMATEX_SUBTYPE
static const struct WV_GUID WAVEFORMATEX_TEMPLATE =
{ 0x0000,
  { 0x00, 0x00,
    0x00, 0x00,
    0x10, 0x00,		/* In file byte order */
    0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
};
#define IS_VALID_WAVEFORMATEX_GUID(Guid) \
    (!memcmp(&WAVEFORMATEX_TEMPLATE.guidx, &Guid.guidx, 14))
#endif

/* Text flag used in the DISP chunk */
#define CF_TEXT		((uint4_t) 1)

struct WV_CKpreamb {
  char ckID[4];
  uint4_t ckSize;
};

struct WV_CKfmt {
  char ckID[4];
  uint4_t ckSize;
  uint2_t wFormatTag;		/* Format type */
  uint2_t nChannels;		/* Number of channels */
  uint4_t nSamplesPerSec;	/* Sample rate */
  uint4_t nAvgBytesPerSec;	/* nBlockAlign * nSamplesPerSec */
  uint2_t nBlockAlign;		/* Block size (bytes), multiple of nChannels */
/* End of common area */
  uint2_t wBitsPerSample;	/* Bits/sample: for WAVE_FORMAT_EXTENSIBLE,
                                   this is the sample container size */
/* Start of EXTENSIBLE part */
  uint2_t cbSize;		/* Number of bytes following */
  uint2_t wValidBitsPerSample;	/* Actual number of bits in each sample */
  uint4_t dwChannelMask;	/* Channel assignment mask */
  struct WV_GUID SubFormat;
};

struct WV_CKfact {
  char ckID[4];
  uint4_t ckSize;
  uint4_t dwSampleLength;
};

struct WV_CKdata {
  char ckID[4];
  uint4_t ckSize;
};

struct WV_CKRIFF {
  char ckID[4];
  uint4_t ckSize;
  char WAVEID[4];
  struct WV_CKfmt CKfmt;
  struct WV_CKfact CKfact;
  struct WV_CKpreamb CKdata;	/* Not including audio data */
};

struct WV_DISP {
  char ckid[4];
  uint4_t cksize;
  uint4_t type;
  const char *text;
};

struct WV_AFSP {
  char ckid[4];
  uint4_t cksize;
  char AFspID[4];
  const char *text;
};

#define WV_N_LIST_INFO	18	/* Maximum number of LIST/INFO items */
struct WV_LIST_INFO {
  char ckid[4];
  uint4_t cksize;
  char listid[4];
  int N;
  struct {
    char ckid[4];
    uint4_t cksize;
    const char *text;
  } listitem[WV_N_LIST_INFO];
};

#endif	/* WVpar_h_ */
