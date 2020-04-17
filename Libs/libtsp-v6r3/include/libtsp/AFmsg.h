/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  AFmsg.h

Description:
  Message texts for the TSP audio file routines.

Author / revision:
  P. Kabal  Copyright (C) 2004
  $Revision: 1.42 $  $Date: 2004/03/29 01:49:14 $

----------------------------------------------------------------------*/

#ifndef AFmsg_h_
#define AFmsg_h_

/* File parameter printout formats */
#define AFM_Headerless	"Headerless audio file"
#define AFMF_Desc	"   Description: %s\n"
#define AFMF_NumSampUnk	"   Number of samples : <unknown>"
#define AFMF_NumSamp	"   Number of samples : %ld"
#define AFMF_SFreq	"   Sampling frequency: %.6g Hz\n"
#define AFMF_NumChan	"   Number of channels: %ld"

/* Error message text */

/* Warning messages for parameter setting routines */
/* AFsetFileType
   AFsetHinfo
   AFsetNHpar
   AFsetSpeaker
*/
#define AFM_BadFormat	"Invalid format keyword"
#define AFM_BadFType	"Invalid file type"
/*      AFM_BadNChan */
#define AFM_BadSFactor	"Invalid scale factor"
#define AFM_BadSFreq	"Invalid sampling frequency"
#define AFM_BadSpkr	"Invalid loudspeaker location"
#define AFM_BadStart	"Invalid start value"
#define AFM_BadSwap	"Invalid swap keyword"
#define AFM_DupSpkr	"Duplicate loudspeaker position"
#define AFM_LongInfo	"Information string too long"
#define AFM_TooManyPar	"Too many parameters"
#define AFM_XSpkr	"Too many speaker locations"

/* Input audio file: Error messages */
#define AFM_BadDLen	"Invalid data length"
#define AFM_BadHeadLen	"Invalid header length"
#define AFM_BadNChan	"Invalid number of channels"
#define AFM_BadNSamp	"Invalid number of samples"
/*      AFM_BadSpkr */
#define AFM_DataErr	"Data format error"
#define AFM_ErrLoDNSamp	"Error, no. samples less than data length"
#define AFM_ErrHiDLen	"Error, data length greater than file length"
#define AFM_ErrHiNSamp	"Error, no. samples greater than data length"
#define AFM_NoNSamp	"Unable to determine number of samples"
#define AFM_OpenRErr	"Error opening input audio file"
#define AFM_RAuto	"Input audio file must be random access for \"auto\""
#define AFM_RRAccess	"Input audio file must be random access"
#define AFM_UnkFType	"Unknown audio file type"
#define AFM_UnsupFType	"Unsupported audio file type"

/* Output audio file: Error messages */
#define AFM_BadDataC	"Invalid data format code"
#define AFM_BadFTypeC	"Invalid output audio file type code"
/*      AFM_BadNChan */
#define AFM_BadNFrame	"Invalid number of sample frames"
#define AFM_OpenWErr	"Error opening output audio file"

/* I/O errors */
#define AFM_FilePosErr	"File positioning error"
#define AFM_LongLine	"Input line too long"
#define AFM_NoFilePos	"Cannot determine file position"
#define AFM_MoveBack	"Cannot move back in file; File must be random access"
#define AFM_ReadErr	"Read error"
#define AFM_ReadErrOffs	"Error while reading, sample offset:"
#define AFM_ReopenErr	"Cannot reopen file as text file"
#define AFM_UEoF	"Unexpected end-of-file"
#define AFM_UEoFOffs	"Unexpected end-of-file, sample offset:"
#define AFM_UpdHeadErr	"Error updating file header"	
#define AFM_WriteErr	"Write error"

/* Warning messages */
#define AFM_BadNbS	"Invalid bits/sample"
#define AFM_BadSRate	"Invalid \"sample_rate\" record"
#define AFM_BadSpkrConfig "Invalid speaker configuration"
#define AFM_FixLoNSamp	"Fixup, no. samples less than data length"
#define AFM_FixHiNSamp	"Fixup, no. samples greater than data length"
#define AFM_FixHiDLen	"Fixup, data length greater than file length"
#define AFM_InaNbS	"Inappropriate bits/sample setting"
#define AFM_MisSRate	"Sampling frequency / \"sample_rate\" record mismatch"
#define AFM_NPSFreq	"Non-positive sampling frequency"
#define AFM_NSampNChan	"No. samples not a multiple of no. channels"
#define AFM_NoIEEE	"Host does not use IEEE float format"
#define AFM_NoRBinMode	"Cannot set input to binary mode"
#define AFM_NoRTextMode	"Cannot set input to text mode"
#define AFM_NoWBinMode	"Cannot set output to binary mode"
#define AFM_NoWTextMode	"Cannot set output to text mode"
#define AFM_NonIntNSamp	"Fixup, non-integer number of samples"
#define AFM_OClip	"Output data clipped"
#define AFM_TruncInfo	"Header information string too long, truncated"

/* Informational message formats */
#define AFMF_BadSFreq	"%s Invalid sampling frequency: \"%g\"; using %g"
#define AFMF_InvNbS	"%s Invalid bits/sample: %d; using %d"
#define AFMF_NSampNChan	"%s %ld samples; %ld channels"
#define AFMF_NClip	"%s %ld data values clipped"

/* Error/warning messages for different audio file types */
#define AFM_AIFF_BadHead "Invalid AIFF/AIFF-C file header"
#define AFM_AIFF_BadId	"Invalid AIFF/AIFF-C file identifier"
#define AFM_AIFF_BadVer	"AIFF-C file: Unrecognized version number"
#define AFM_AIFF_BadSpkr "AIFF/AIFF-C file: Non-standard loudspeaker configuration"
#define AFM_AIFF_BadSize "AIFF/AIFF-C file: Header stucture size mismatch"
#define AFM_AIFF_FixCOMM "AIFF/AIFF-C file: Fixup for invalid COMM chunk size"
#define AFM_AIFF_FixFORM "AIFF/AIFF-C file: Fixup for invalid FORM chunk size"
#define AFM_AIFF_NoSSND	"AIFF/AIFF-C file: Missing SSND chunk"
#define AFM_AIFF_UnsComp "AIFF-C file: Unsupported compression type"
#define AFM_AIFF_UnsData "AIFF/AIFF-C file: Unsupported data format"
#define AFM_AIFF_UnsSSize "AIFF/AIFF-C file: Unsupported sample size"
#define AFM_AIFF_WRAccess "AIFF/AIFF-C file: Output file must be random access"
#define AFMF_AIFF_InvNbS "%s AIFF/AIFF-C file: Invalid bits/sample: %d; using %d"

#define AFM_AU_BadId	"Invalid AU audio file identifier"
#define AFM_AU_BadSRate	"AU audio file: Invalid AFsp sample_rate record"
#define AFM_AU_UnsData	"AU audio file: Unsupported data format"

#define AFM_BL_BadId	"Invalid SPPACK file identifier"
#define AFM_BL_UnsComp	"SPPACK file: Unsupported companding option"
#define AFM_BL_UnsData	"SPPACK file: Unsupported data format"
#define AFM_BL_UnsDomain "SPPACK file: Unsupported domain"
#define AFM_BL_UnsWLen	"SPPACK file: Unsupported data word length"

#define AFM_ES_BadId	"Invalid ESPS file identifier"
#define AFM_ES_IdMatch	"ESPS file: Identifier values do not match"
#define AFM_ES_NoSFreq	"ESPS file: Missing sampling frequency"
#define AFM_ES_UnsData	"ESPS file: Unsupported data format"
#define AFM_ES_UnsEncod	"ESPS file: Unsupported data encoding"
#define AFM_ES_UnsFea	"ESPS file: Unsupported feature file subtype"
#define AFM_ES_UnsType	"ESPS file: Unsupported file type code"

#define AFM_NH_InvSwap	"Headerless file: Invalid data format"
#define AFM_NH_NoFormat	"Headerless file: Data format not specified"
#define AFM_NH_UnsData	"Headerless file: Unsupported data type"

#define AFM_NS_BadHead	"Invalid CSL NSP file header"
#define AFM_NS_BadId	"Invalid CSL NSP file identifier"
#define AFM_NS_BadHEDR	"CSL NSP file: Invalid HEDR/HDR8 chunk size"
#define AFM_NS_BadFORM	"CSL NSP file: Invalid FORM-DS16 block size"
#define AFM_NS_FixFORM	"CSL NSP file: Fixup for invalid FORM-DS16 block size"

#define AFM_INRS_BadId	"Invalid INRS-Telecom file identifier"

#define AFM_SF_BadId	"Invalid IRCAM soundfile identifier"
#define AFM_SF_UnsData	"IRCAM soundfile: Unsupported data format"

#define AFM_SP_BadId	"Invalid NIST SPHERE audio file identifier"
#define AFM_SP_BadHLen	"NIST SPHERE audio file: Invalid header length"
#define AFM_SP_Damaged	"Damaged NIST SPHERE audio file"
#define AFM_SP_NoInter	"NIST SPHERE audio file: Channels may not be interleaved"
#define AFM_SP_UnsByte	"NIST SPHERE audio file: Unsupported byte format"
#define AFM_SP_UnsData	"NIST SPHERE audio file: Unsupported data format"
#define AFM_SP_UnsMulaw	"NIST SPHERE audio file: Invalid mu-law word length"
#define AFM_SP_UnsPCM	"NIST SPHERE audio file: Unsupported PCM word length"

#define AFMF_SP_NoMatch	"%s No \"%s\" record in NIST SPHERE audio file header"
#define AFMF_SP_BadField \
	"%s NIST SPHERE audio file: Invalid field in \"%s\" record"

#define AFM_SW_AscCmplx	"Comdisco SPW Signal file: Complex ASCII data not supported"
#define AFM_SW_BadId	"Invalid SPW Signal file identifier"
#define AFM_SW_BadNSamp	"SPW Signal file: Invalid number of samples"
#define AFM_SW_BadSFreq	"SPW Signal file: Invalid sampling frequency"
#define AFM_SW_BadSTime	"SPW Signal file: Invalid starting time"
#define AFM_SW_LongHead	"SPW Signal file: Header too long"
#define AFM_SW_NZSTime	"SPW Signal file: Non-zero starting time"
#define AFM_SW_NoData	"SPW Signal file: Data type not specified"
#define AFM_SW_NoSFreq	"SPW Signal file: Sampling frequency not specified"
#define AFM_SW_UnkCOM	"SPW Signal file: Unknown COMMON_INFO field"
#define AFM_SW_UnkKey	"SPW Signal file: Unknown keyword"
#define AFM_SW_UnkSys	"SPW Signal file: Unknown system type"
#define AFM_SW_UnsCmplx	"SPW Signal file: Unsupported complex format"
#define AFM_SW_UnsData	"SPW Signal file: Unsupported data type"
#define AFM_SW_UnsDInfo	"SPW Signal file: Unsupported DATA_INFO record"
#define AFM_SW_UnsFixP	"SPW Signal file: Unsupported fixed point format"

#define AFM_TA_BadId	"Invalid Text file identifier"
#define AFM_TA_LongHead	"Text file: Header too long"
#define AFM_TA_NoNSamp	"Text file: Missing sampling frequency"

#define AFM_WV_BadHead	"Invalid WAVE file header"
#define AFM_WV_BadAlaw	"WAVE file: Invalid A-law data size"
#define AFM_WV_BadBlock	"WAVE file: Invalid block align value"
#define AFM_WV_BadBytesSec \
		"WAVE file: Inconsistent header value AvgBytesPerSec ignored"
#define AFM_WV_BadFACT  "WAVE file: Invalid fact chunk size"
#define AFM_WV_BadFloat	"WAVE file: Invalid float data size"
#define AFM_WV_BadHeader "Malformed WAVE file header"
#define AFM_WV_BadId	"Invalid WAVE file identifier"
#define AFM_WV_BadMulaw	"WAVE file: Invalid mu-law data size"
#define AFM_WV_BadfmtEx	"WAVE file: Invalid fmt (EXTENSIBLE) chunk"
#define AFM_WV_BadfmtSize "WAVE file: Invalid fmt chunk size"
#define AFM_WV_BadNbS	"WAVE file: Invalid bits/sample"
#define AFM_WV_BadNChan	"WAVE file: Invalid number of channels"
#define AFM_WV_BadRIFF	"WAVE file: Invalid RIFF chunk size"
#define AFM_WV_BadSpkr	"WAVE file: Invalid speaker order"
#define AFM_WV_BadSize	"WAVE file: Header stucture size mismatch"
#define AFM_WV_BadSS	"WAVE file: Inconsistent sample size"
#define AFM_WV_FixRIFF	"WAVE file: Fixup for invalid RIFF chunk size"
#define AFM_WV_UnkChannel "WAVE file: Unknown Channel/Speaker combination"
#define AFM_WV_UnsDSize	"WAVE file: Unsupported PCM data size"
#define AFM_WV_UnsData	"WAVE file: Unsupported data format"
#define AFM_WV_WRAccess	"WAVE file: Output file must be random access"
#define AFMF_WV_InvNbS "%s WAVE file: Invalid bits/sample: %d; using %d"

#endif	/* AFmsg_h_ */
