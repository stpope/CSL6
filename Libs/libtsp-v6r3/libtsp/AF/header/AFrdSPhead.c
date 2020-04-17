/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  AFILE *AFrdSPhead (FILE *fp)

Purpose:
  Get file format information from a NIST SPHERE audio file

Description:
  This routine reads the header for a NIST SPHERE audio file.  The header
  information is used to set the file data format information in the audio
  file pointer structure.

  NIST SPHERE audio file header:
   Offset Length Type    Contents
      0     8    char   File identifier ("NIST_1A\n")
      8     8    char   Header length ("   1024\n")
     16    ...   struct Object-oriented records, e.g.
                          "speaker_id -s3 001\n
                           sample_rate -i 16000\n
                          ...
                           end_head\n"
   1024    ...   --     Audio data

Parameters:
  <-  AFILE *AFrdSPhead
      Audio file pointer for the audio file
   -> FILE *fp
      File pointer for the file

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.80 $  $Date: 2003/11/03 18:56:14 $

-------------------------------------------------------------------------*/

#include <setjmp.h>
#include <string.h>

#include <libtsp.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFmsg.h>
#include <libtsp/AFpar.h>

#define SAME_CSTR(str,ref) 	(memcmp (str, ref, sizeof (str)) == 0)
#define GET_REC(Ident,Rec,Lr,Type,vp,Errcont) \
	AF_getSPrec (Ident, Type, (void *) vp, Rec, Lr, Errcont)

/* NIST SPHERE file identifier as a character string */
#define SP_ident	"NIST_1A\n"
#define SP_identX	"NIST_1A\r\n"	/* Damaged by text file conversion */
#define SP_hsize	"   1024\n"

#define LHEAD		1024
#define SP_LI		((sizeof (SP_ident)) - 1)
#define SP_LH		((sizeof (SP_hsize)) - 1)
#define SP_LR		(LHEAD - SP_LI - SP_LH)

#define T_INTEGER	0
#define T_REAL		1
#define T_STRING	2
#define T_STRVAL	3
#define T_POINTER		4

#define MAXRECORD	1024
#define MAXIDENT	80

#define ERR_NOMSG	0
#define ERR_MSG		1

#define RET_NOMATCH	1
#define RET_BADVALUE	2

#define REQUIRED	(ERR_MSG)
#define OPTIONAL	(ERR_NOMSG)

#define SP_MAXINFO	LHEAD

struct SP_head {
  char ident[SP_LI];
  char hsize[SP_LH];
  char rec[SP_LR];
};

/* ID's */
/* A list of ID's is in stdfield.c in the SPHERE 2.6a distribution,
     ftp://jaguar.ncsl.nist.gov.
   Some of ID's are used as comments; others are used to decode the data
   format. In addition, there are ID's which appear in files but are
   not in the standard list.
*/
static const char *CommentID[] = {
  /* Standard comments */
  "database_id",
  "database_version",
  "utterance_id",
  "sample_min",
  "sample_max",
  /* Additional comments */
  "conversation_id",
  "microphone",
  "prompt_id",
  "recording_date",
  "recording_environment",
  "recording_site",
  "recording_time",
  "sample_checksum",
  "speaker_sentence_number",
  "speaker_session_number",
  "speaker_utterance_number",
  "speaker_id",
  "speaker_id_a",
  "speaker_id_b",
  "speaking_mode",
  "utterance_id",
  NULL
};

/* Data format identifiers
  "channel_count"	required
  "sample_count"	required
  "sample_rate"		required
  "sample_n_bytes"	required
  "sample_byte_format"
  "sample_sig_bits"
*/

/* setjmp / longjmp environment */
extern jmp_buf AFR_JMPENV;

AF_READ_DEFAULT(AFr_default);	/* Define the AF_read defaults */

static int
AF_getFormat (const char Rec[], int Lr, struct AF_dformat *DFormat);
static int
AF_getNchan (const char Rec[], int Lr, long int *Nchan);
static int
AF_getComment (const char *ID[], const char Rec[], int Lr,
	       struct AF_infoX *InfoX);
static const char *
AF_sepSPrec (const char name[], const char head[], int Lr);
static int
AF_getSPrec (const char Ident[], int Type, void *data,
	     const char Rec[], int Lr, int Erropt);


AFILE *
AFrdSPhead (FILE *fp)

{
  AFILE *AFp;
  long int Srate;
  int Lr;
  char *p;
  char Info[SP_MAXINFO];
  struct SP_head Fhead;
  struct AF_read AFr;

/* Set the long jump environment; on error return a NULL */
  if (setjmp (AFR_JMPENV))
    return NULL;	/* Return from a header read error */

/* Defaults and inital values */
  AFr = AFr_default;
  AFr.InfoX.Info = Info;
  AFr.InfoX.Nmax = SP_MAXINFO;

/* Read in the header */
  RHEAD_S (fp, Fhead.ident);
  RHEAD_S (fp, Fhead.hsize);
  RHEAD_S (fp, Fhead.rec);

/* Check the file identifier */
  if (! SAME_CSTR (Fhead.ident, SP_ident)) {
    if (SAME_CSTR (Fhead.ident, SP_identX))
      UTwarn ("AFrdSPhead - %s", AFM_SP_Damaged);
    else
      UTwarn ("AFrdSPhead - %s", AFM_SP_BadId);
    return NULL;
  }
  if (! SAME_CSTR (Fhead.hsize, SP_hsize)) {
    UTwarn ("AFrdSPhead - %s: \"%s\"", AFM_SP_BadHLen, Fhead.hsize);
    return NULL;
  }
  if (GET_REC ("end_head", Fhead.rec, SP_LR, T_POINTER, &p, REQUIRED))
    return NULL;
  Lr = p - Fhead.rec + 1;

/* Nsamp, Sfreq and Nchan */
  if (GET_REC ("sample_count", Fhead.rec, Lr, T_INTEGER, &AFr.NData.Nsamp,
	       REQUIRED))
    return NULL;
  if (GET_REC ("sample_rate", Fhead.rec, Lr, T_INTEGER, &Srate, REQUIRED))
    return NULL;
  AFr.Sfreq = (double) Srate;
  if (AF_getNchan (Fhead.rec, Lr, &AFr.NData.Nchan))
    return NULL;

/* Sample format */
  if (AF_getFormat (Fhead.rec, Lr, &AFr.DFormat))
    return NULL;

/* Comment fields */
  if (AF_getComment (CommentID, Fhead.rec, Lr, &AFr.InfoX))
    return NULL;

/* Set the parameters for file access */
  AFp = AFsetRead (fp, FT_SPHERE, &AFr, AF_NOFIX);

  return AFp;
}

/* Decode and check the coding format 
   Return 0, success
          1, error
*/


static int
AF_getFormat (const char Rec[], int Lr, struct AF_dformat *DFormat)

{
  char Sval[MAXRECORD+1];
  long int Lw, NbS;
  int ErrCode;

  /* Get the coding format (default "pcm") */
  strcpy (Sval, "pcm");	/* Default value */
  if (GET_REC ("sample_coding", Rec, Lr, T_STRING, Sval, OPTIONAL)
      == RET_BADVALUE)
    return 1;

  if (GET_REC ("sample_n_bytes", Rec, Lr, T_INTEGER, &Lw, REQUIRED))
    return 1;

  NbS = 0;
  if (GET_REC ("sample_sig_bits", Rec, Lr, T_INTEGER, &NbS, OPTIONAL)
      == RET_BADVALUE)
    return 1;
  DFormat->NbS = (int) NbS;

  /* Check the data format fields */
  if (strcmp (Sval, "pcm") == 0) {

    /* PCM */
    DFormat->Format = FD_INT16;

    if (Lw != 2) {
      UTwarn ("AFrdSPhead - %s: \"%ld\"", AFM_SP_UnsPCM, Lw);
      return 1;
    }

    DFormat->Swapb = DS_NATIVE;
    ErrCode = GET_REC ("sample_byte_format", Rec, Lr, T_STRING, Sval,
		       OPTIONAL);
    if (ErrCode == RET_BADVALUE)
      return 1;
    else if (ErrCode == 0) {
      if (strcmp (Sval, "10") == 0)
	DFormat->Swapb = DS_EB;
      else if (strcmp (Sval, "01") == 0)
	DFormat->Swapb = DS_EL;
      else {
	UTwarn ("AFrdSPhead - %s: \"%.20s\"", AFM_SP_UnsByte, Sval);
	return 1;
      }
    }

  }

  else if (strcmp (Sval, "ulaw") == 0 || strcmp (Sval, "mu-law") == 0) {

    /* Mu-law */
    DFormat->Format = FD_MULAW8;
    DFormat->Swapb = DS_NATIVE;
    if (Lw != 1) {
      UTwarn ("AFrdSPhead - %s: \"%d\"", AFM_SP_UnsMulaw, Lw);
      return 1;
    }
  }

  else {
    UTwarn ("AFrdSPhead - %s: \"%s\"", AFM_SP_UnsData, Sval);
    return 1;
  }

  return 0;
}

/* Get and check the number of channels
   Return 0, success
          1, error
*/


static int
AF_getNchan (const char Rec[], int Lr, long int *Nchan)

{
  char Sval[MAXRECORD+1];

  if (GET_REC ("channel_count", Rec, Lr, T_INTEGER, Nchan, REQUIRED))
    return 1;

  if (*Nchan > 1) {
    strcpy (Sval, "TRUE");
    if (GET_REC ("channels_interleaved", Rec, Lr, T_STRING, Sval, OPTIONAL)
	== RET_BADVALUE)
      return 1;
    if (strcmp (Sval, "TRUE") != 0)
      UTwarn ("AFrdSPhead - %s", AFM_SP_NoInter);
  }

  return 0;
}

/* Put comment strings into the AFsp information structure */


static int
AF_getComment (const char *ID[], const char Rec[], int Lr,
	       struct AF_infoX *InfoX)

{
  char Sval[MAXRECORD+1];
  char Ident[MAXIDENT+3];
  int i;

  for (i = 0; ID[i] != NULL; ++i) {
    if (GET_REC (ID[i], Rec, Lr, T_STRVAL, Sval, OPTIONAL) == 0) {
      strcpy (Ident, ID[i]);
      strcat (Ident, ": ");
      AFaddAFspRec (Ident, Sval, strlen (Sval), InfoX);
    }
  }

  return 0;
}

/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AF_getSPrec (const char Ident[], int Type, void *data,
                   const char Rec[], int Lr, int Erropt)

Purpose:
  Find a named record in a NIST SPHERE audio file header

Description:
  This routine searches through records in the header of an NIST SPHERE audio
  file looking for a record with a given name field.  The data value for that
  record is returned.  The data value is an integer value, a double value or a
  string.  For strings, the returned string is null terminated.  This routine
  should only be used for retrieving strings which are known not to contain
  imbedded null characters or imbedded newline characters.  Except for these
  limitations, this routine returns values from correctly formed records.  It
  does not attempt to verify full syntactic correctness of records.

Parameters:
  <-  int AF_getSPrec
      Returned error status,
        0 for no error,
        1 for no keyword match,
        2 for decoding error
   -> const char Ident[]
      Record name to be matched
   -> int Type
      Data type,
       - T_INTEGER for long integer
       - T_REAL for double
       - T_STRING for string
       - T_STRVAL for string (undecoded integer or double, or string)
       - T_POINTER for a pointer (const char *) to the start of the Ident
         string in the header text
  <-  void *data
      Pointer to where the data is to be written.
        - T_INTEGER: data is a pointer to a long int value.
	- T_REAL:    data is a pointer to a double value.
	- T_STRING:  data is a pointer to a character string with room for
	             Lh+1 characters.
        - T_STRVAL:  data is a pointer to a character string with room for
	             Lh+1 characters
        - T_POINTER: data is pointer to a pointer to const char.
      Nothing is stored if an error is detected.
   -> const char Rec[]
      Header text with the header records (Lh characters)
   -> int Lh
      Number of characters in the header text
   -> int Erropt
      Error option flag.  This parameter controls how this routine
      reacts if a matching name is not found.
        0  No match: no error message
        1  No match: error message

-------------------------------------------------------------------------*/

static int
AF_getSPrec (const char Ident[], int Type, void *data,
	     const char Rec[], int Lr, int Erropt)

{
  const char *p, *dataf, *valuef;
  int Dtype, Nd, Ns, Nsmax;
  long int Iv;
  double Dv;

/* Search for a matching record */
  p = AF_sepSPrec (Ident, Rec, Lr);
  if (p == NULL) {
    if (Erropt == ERR_MSG)
      UTwarn (AFMF_SP_NoMatch, "AFrdSPhead -", Ident);
    return RET_NOMATCH;
  }

  /* Fill in a pointer to the Ident string */
  if (Type == T_POINTER) {
    *((const char **) data) = p;
    return 0;
  }
    
/* Find the end of the type field */
  dataf = p;
  Nsmax = Lr - (dataf - Rec);
  p = (const char *) memchr (dataf, ' ', Nsmax);
  if (p != NULL)
    valuef = p + 1;
  else
    valuef = &Rec[Lr];
  Nd = valuef - dataf - 1;
  Nsmax = Lr - (valuef - Rec);

/* Decode the type field */
  if (Nd == 2 && strncmp (dataf, "-i", 2) == 0)
    Dtype = T_INTEGER;
  else if (Nd == 2 && strncmp (dataf, "-r", 2) == 0)
    Dtype = T_REAL;
  else if (Nd > 2 && strncmp (dataf, "-s", 2) == 0)
    Dtype = T_STRING;
  else {
    UTwarn (AFMF_SP_BadField, "AFrdSPhead -", Ident);
    return RET_BADVALUE;
  }

/* Check for a type match */
  if (Type == T_STRVAL && Dtype == T_STRING)
    Type = T_STRING;
  if (Type != T_STRVAL && Dtype != Type) {
    UTwarn (AFMF_SP_BadField, "AFrdSPhead -", Ident);
    return RET_BADVALUE;
  }

/* Decode values */
  if (Type == T_INTEGER) {
    if (sscanf (valuef, "%ld", &Iv) == 1) {
      *((long int *) data) = Iv;
      return 0;
    }
  }
  else if (Type == T_REAL) {
    if (sscanf (valuef, "%lg", &Dv) == 1) {
      *((double *) data) = Dv;
      return 0;
    }
  }
  else if (Type == T_STRING) {
    if (sscanf (&dataf[2], "%d", &Ns) == 1) {
      STcopyNMax (valuef, (char *) data, Ns, Nsmax);
      return 0;
    }
  }
  else if (Type == T_STRVAL) {
    p = (const char *) memchr (valuef, '\n', Nsmax);
    if (p != NULL) {
      Ns = p - valuef;
      STcopyNMax (valuef, (char *) data, Ns, Nsmax);
    }
    return 0;
  }

/* Error messages */
  UTwarn (AFMF_SP_BadField, "AFrdSPhead -", Ident);
  return RET_BADVALUE;
}

/*
  This routine searches through records in the header of an NIST SPHERE audio
  file, looking for a record with a given name field.  For the purpose of this
  routine, a name is the leading part of a record, separated from the type and
  value fields by a blank.  This routine returns a pointer to the beginning of
  the type field.

  In the syntax for NIST SPHERE headers, a newline character marks the end of
  a record.  But the header syntax also allows for any character, including
  a newline character, to appear within a string value.  To avoid parsing
  each record to find the end of record, this routine looks for the name at
  the beginning of data and following each newline.  This requires that a
  newline character imbedded in a string value must not be followed by a
  sequence of characters which would be mistaken for the name field at the
  beginning of a record.
*/

/* If Ident record is found, this routine returns a pointer to the start
   of the data type field. If the Ident record is not found, the return
   value is NULL.
*/

static const char *
AF_sepSPrec (const char Ident[], const char Rec[], int Lr)

{
  const char *p;
  int Nleft, Ni;

  Ni = strlen (Ident);
  p = Rec;
  Nleft = Lr;
  while (Nleft >= Ni + 1) {

    /* Try a match to ident + space/newline */
    if (memcmp (p, Ident, Ni) == 0 && (p[Ni] == ' ' || p[Ni] == '\n')) {
      p += Ni + 1;
      return (p);
    }

    /* Find the next putative record terminator */
    Nleft = Lr - (p - Rec);
    p = (const char *) memchr (p, '\n', Nleft);
    if (p == NULL)
      return (NULL);

    ++p; 	/* Skip past the newline */
    Nleft = Lr - (p - Rec);
  }

  return (NULL);
}
