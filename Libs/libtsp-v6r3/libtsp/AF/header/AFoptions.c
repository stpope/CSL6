/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  struct AF_opt *AFoptions (void)
  struct AF_opt *AFresetOptions (unsigned int Cat)

Purpose:
  Get a pointer to the audio file options stucture

Description:
  This routine returns a pointer to the audio file options structure.  The
  options structure is defined in the header file AFpar.h.  Routines are
  provided to set and access some of the fields.

  Program Behaviour:
  Error Control (ErrorHalt):
    The top level AF routines (AFopnRead, AFopnWrite, AFdReadData, etc.)
    normally print an error message and halt when an error is passed from a
    lower level routine.  Optionally, errors can also be signalled by a return
    value such as NULL file pointer or other error flag.  The error options are
    as follows.
      0 - Continue on error, returning an error code
      1 - Stop on error (default behaviour)

  Options for Input Files:
  Number of samples (NsampND):
    The number of samples in a speech file opened for read is normally
    returned by AFopnRead.  In some cases, the number of samples can only
    be determined from the actual number of records in the file.  This
    determination is not possible for input streams which are not random
    access.  For such cases, the number of samples value can be set to a value
    which indicates that this value is not available.  The NsampND options are
    as follows.
      0 - Always return the number of samples in the file.
	  An error condition exists if the number of samples cannot be
	  determined (default behaviour).
      1 - Return the number of samples if the file is seekable or the
	  number of samples is specified in the file header.  Otherwise
	  return a value indicating that this value is undefined
	  (AF_NSAMP_UNDEF).
  Random access requirement (RAccess):
    Some programs require the ability to reposition the file to an earlier
    position (to reread data, for instance).  This parameter controls whether
    a file will be opened if it does not support random access (viz. file
    streams from a pipe are not random access).
      0 - Allow input files which are not random access (default behaviour).
	  If an attempt is made to reposition such a file during a read
	  operation, an error condition will occur.
      1 - Do not open input audio files which are not random access.
  Input File Type (FtypeI):
    Input audio file type (default FT_AUTO).  This parameter can be set with
    symbolic parameters using the routine AFsetFileType.
  File parameters for headerless files (struct AF_NHpar):
    Parameters for noheader audio files.  These parameters can be set with
    symbolic values using the routine AFsetNHpar.

  Options for Output Files:
  Number of frames (long int Nframe):
    This value specifies the number of frames in an output audio file.  This
    value is used to set the number of frames information in a file header
    before opening the file.  It is used for non-random access output files
    which cannot update the header after writing the audio data.  If this value
    is AF_NFRAME_UNDEF, the number of frames is not pre-defined.  For some
    types of output files, this setting means that the output file must be
    random acesss.
  Number of bits per sample (int NbS):
    This value affects a corresponding value in some file headers.  Each
    sample is in a container with a size which is a multiple of 8.  The
    container size is determined from the data format.  The number of bits per
    sample indicates that some of the bits are not significant.  The number of
    bits per sample rounded up to a multiple of eight must equal the container
    size.  The default value of zero indicates that the number of bits is the
    same as the container size for the data.
  Speaker configuration (char *SpkrConfig):
    This null-terminated string contains the speaker location codes.  This
    string can be set with symbolic values using the routine AFsetSpkr.  This
    string is allocated as needed.  It can be deallocated by invoking
    AFsetSpeaker ("").
  Header information string (char *Uinfo):
    This string is the information string supplied by the user via the routine
    AFsetInfo.  This string is allocated as needed.  It can be deallocated by
    invoking AFsetInfo (NULL).

Parameters:
  <-  struct AF_opt *
      Pointer to the option structure
   -> unsigned int Cat
      For AFresetOptions: Categories of options to be reset.  Cat is the sum
      of the following sub-categories:
        AF_OPT_GENERAL - General options (ErrorHalt)
        AF_OPT_INPUT   - Input file options
        AF_OPT_OUTPUT  - Output file options

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.19 $  $Date: 2003/11/03 18:52:00 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/AFpar.h>

/* Default options */
#define AF_NHPAR_DEFAULT \
	{ FD_UNDEF, 0L, AF_SFREQ_DEFAULT, DS_NATIVE, 1L, AF_SF_DEFAULT }
#define AF_OPT_DEFAULT \
	{ 1, \
	  0, 0, FT_AUTO , AF_NHPAR_DEFAULT, \
	  AF_NFRAME_UNDEF, 0, NULL, NULL }

/* Initialize to default values */
static struct AF_opt AFopt = AF_OPT_DEFAULT;
static struct AF_NHpar NHpar_default = AF_NHPAR_DEFAULT;


struct AF_opt *
AFoptions (void)

{
  return &AFopt;
}

/* Reset the options */


struct AF_opt *
AFresetOptions (unsigned int Cat)

{
  if (Cat & AF_OPT_GENERAL) {
    AFopt.ErrorHalt = 1;
  }

  if (Cat & AF_OPT_INPUT) {
    AFopt.NsampND = 0;
    AFopt.RAccess = 0;
    AFopt.FtypeI = FT_AUTO;
    AFopt.NHpar = NHpar_default;
  }

  if (Cat & AF_OPT_OUTPUT) {
    AFopt.Nframe = AF_NFRAME_UNDEF;
    AFopt.NbS = 0;
    UTfree (AFopt.SpkrConfig);
    AFopt.SpkrConfig = NULL;
    UTfree (AFopt.Uinfo);
    AFopt.Uinfo = NULL;
  }

  return &AFopt;
}
