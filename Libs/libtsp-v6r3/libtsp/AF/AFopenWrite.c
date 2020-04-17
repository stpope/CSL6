/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  AFILE *AFopenWrite (const char Fname[], int Fformat, long int Nchan,
                      double Sfreq, FILE *fpinfo)

Purpose:
  Open an audio file for writing

Description:
  Use AFopnWrite instead. The difference between the routines is the scaling
  of the data.

Parameters:
  <-  AFILE *AFopenWrite
      Audio file pointer for the audio file
   -> const char Fname[]
      Character string specifying the file name.  The file name "-" means
      standard output.
   -> int Fformat
      Audio file format code, evaluated as the sum of a data format code and a
      file type,
        Fformat = Dformat + 256 * Ftype
      Dformat: data format
        FD_MULAW8  = 1,  mu-law 8-bit data
        FD_ALAW8   = 2,  A-law 8-bit data
        FD_UINT8   = 3,  offset-binary 8-bit integer data
        FD_INT8    = 4,  two's-complement 8-bit integer data
        FD_INT16   = 5,  two's-complement 16-bit integer data
        FD_INT24   = 6,  two's-complement 24-bit integer data
        FD_INT32   = 7,  two's-complement 32-bit integer data
        FD_FLOAT32 = 8,  32-bit floating-point data
        FD_FLOAT64 = 9,  64-bit floating-point data
        FD_TEXT    = 10,  text data
      Ftype: output file type
        FTW_AU        = 1,      AU audio file
        FTW_WAVE      = 2,      WAVE file (auto extensible format)
        FTW_WAVE_NOEX = 2 + 16, WAVE file (no extensible format)
        FTW_AIFF_C    = 3,      AIFF-C sound file
        FTW_NH_EB     = 4 +  0, Headerless file (big-endian byte order)
        FTW_NH_EL     = 4 + 16, Headerless file (little-endian byte order)
        FTW_NH_NATIVE = 4 + 32, Headerless file (native byte order)
        FTW_NH_SWAP   = 4 + 48, Headerless file (byte-swapped byte order)
	FTW_AIFF      = 5,      AIFF sound file
   -> long int Nchan
      Number of channels
   -> double Sfreq
      Sampling frequency
   -> FILE *fpinfo
      File pointer for printing audio file information.  If fpinfo is not NULL,
      information about the audio file is printed on the stream selected by
      fpinfo.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.79 $  $Date: 2003/05/13 01:51:30 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/AFpar.h>

#define FTW_FTYPE_MOD	256
#define FTW_dformat(code)	((code) % FTW_FTYPE_MOD)
#define FTW_ftype(code)		((code) / FTW_FTYPE_MOD)


AFILE *
AFopenWrite (const char Fname[], int Fformat, long int Nchan, double Sfreq,
	     FILE *fpinfo)

{
  AFILE *AFp;
  int Ftype, Dformat;

  Ftype = FTW_ftype (Fformat);
  Dformat = FTW_dformat (Fformat);
  AFp = AFopnWrite (Fname, Ftype, Dformat, Nchan, Sfreq, fpinfo);
  AFp->ScaleF = AFp->ScaleF / 32768.;

  return AFp;
}
