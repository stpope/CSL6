/*-------------- Telecommunications & Signal Processing Lab ---------------

Routine:
  AFILE *AFrdEShead (FILE *fp)

Purpose:
  Get file format information from an ESPS sampled data feature file

Description:
  This routine reads the header for an ESPS sampled data feature file.  The
  header information is used to set the file data format information in the
  audio file pointer structure.

  ESPS sampled data feature file header:
   Offset Length Type    Contents
      8     4    --     Header size (bytes)
     12     4    int    Sampled data record size
     16     4    int    File identifier
     40    26    char   File creation date
     66     8    char   Header version
     74    16    char   Program
     90     8    char   Program version
     98    26    char   Program date
    124     4    int    Number of samples (may indicate zero)
    132     4    int    Number of doubles in a data record
    136     4    int    Number of floats in a data record
    140     4    int    Number of longs in a data record
    144     4    int    Number of shorts in a data record
    148     4    int    Number of chars in a data record
    160     8    char   User name
    333    ...   --     Generic header items, including "record_freq"
      -    ...   --     Audio data

Parameters:
  <-  AFILE *AFrdEShead
      Audio file pointer for the audio file
   -> FILE *fp
      File pointer for the file

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.70 $  $Date: 2003/11/03 18:54:48 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <ctype.h>
#include <setjmp.h>
#include <string.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFmsg.h>
#define AF_DATA_LENGTHS
#include <libtsp/AFpar.h>
#include <libtsp/ESpar.h>
#include <libtsp/UTtypes.h>

#define ICEILV(n,m)	(((n) + ((m) - 1)) / (m))	/* int n,m >= 0 */
#define MINV(a, b)	(((a) < (b)) ? (a) : (b))
#define RNDUPV(n,m)	((m) * ICEILV (n, m))		/* Round up */

#define SAME_CSTR(str,ref) 	(memcmp (str, ref, sizeof (str)) == 0)
#define SWAPB(value) \
	VRswapBytes ((const void *) &(value), \
		     (void *) &(value), sizeof (value), 1)
#define SWAPBXY(x,y) \
	VRswapBytes ((const void *) &(x), (void *) &(y), sizeof (x), 1)

/* setjmp / longjmp environment */
extern jmp_buf AFR_JMPENV;

AF_READ_DEFAULT(AFr_default);	/* Define the AF_read defaults */

static int
AF_getGeneric (const char buff[], int Nbuff, const char ID[], int Fbo,
	       int Nval, int Type, void *Val);


AFILE *
AFrdEShead (FILE *fp)

{
  AFILE *AFp;
  int NgI, Nv;
  long int offs;
  double8_t STime;
  char Info[ES_MAXINFO];
  struct ES_preamb Fpreamb;
  struct ES_fixhead FheadF;
  struct ES_FEAhead FheadV;
  char GenItems[ES_MAXGENERIC];
  char str[20];
  struct AF_read AFr;

/* Set the long jump environment; on error return a NULL */
  if (setjmp (AFR_JMPENV))
    return NULL;	/* Return from a header read error */

/* Defaults and inital values */
  AFr = AFr_default;
  AFr.InfoX.Info = Info;
  AFr.InfoX.Nmax = ES_MAXINFO;

/* Read selected preamble values */
/* We do not know the byte order until after we have read the file magic */
  offs = RSKIP (fp, 8L);
  offs += RHEAD_V (fp, Fpreamb.Data_offset, DS_NATIVE);
  offs += RHEAD_V (fp, Fpreamb.Record_size, DS_NATIVE);
  offs += RHEAD_S (fp, Fpreamb.Magic);

/* Check the preamble file magic */
  if (SAME_CSTR (Fpreamb.Magic, FM_ESPS_BE))
    AFr.DFormat.Swapb = DS_EB;
  else if (SAME_CSTR (Fpreamb.Magic, FM_ESPS_LE))
    AFr.DFormat.Swapb = DS_EL;
  else {
    UTwarn ("AFrdEShead - %s", AFM_ES_BadId);
    return NULL;
  }

/* Fix up the words we have already read */
  if (UTswapCode (AFr.DFormat.Swapb) == DS_SWAP) {
    SWAPB (Fpreamb.Data_offset);
    SWAPB (Fpreamb.Record_size);
  }
 
/* Read selected values from the fixed part of the header */
  offs += RSKIP (fp, 32 - offs);
  offs += RHEAD_V (fp, FheadF.Type, AFr.DFormat.Swapb);
  offs += RSKIP (fp, 2);
  offs += RHEAD_S (fp, FheadF.Magic);
  offs += AFrdTextAFsp (fp, 26, "date: ", &AFr.InfoX, 1);
  offs += AFrdTextAFsp (fp,  8, "header_version: ", &AFr.InfoX, 1);
  offs += AFrdTextAFsp (fp, 16, "program_name: ", &AFr.InfoX, 1);
  offs += AFrdTextAFsp (fp,  8, "program_version: ", &AFr.InfoX, 1);
  offs += AFrdTextAFsp (fp, 26, "program_compile_date: ", &AFr.InfoX, 1);
  offs += RHEAD_V (fp, FheadF.Ndrec, AFr.DFormat.Swapb);
  offs += RSKIP (fp, 4);
  offs += RHEAD_V (fp, FheadF.Ndouble, AFr.DFormat.Swapb);
  offs += RHEAD_V (fp, FheadF.Nfloat, AFr.DFormat.Swapb);
  offs += RHEAD_V (fp, FheadF.Nlong, AFr.DFormat.Swapb);
  offs += RHEAD_V (fp, FheadF.Nshort, AFr.DFormat.Swapb);
  offs += RHEAD_V (fp, FheadF.Nchar, AFr.DFormat.Swapb);
  offs += RSKIP (fp, 8);
  offs += AFrdTextAFsp (fp, 8, "user: ", &AFr.InfoX, 1);

/* Read selected feature file header values */
  offs += RSKIP (fp, 188 - offs);
  offs += RHEAD_V (fp, FheadV.Fea_type, AFr.DFormat.Swapb);

/* Generic items */
  NgI = MINV (ES_MAXGENERIC, Fpreamb.Data_offset - ES_LHMIN);
  offs += RSKIP (fp, ES_LHMIN - offs);
  offs += RHEAD_SN (fp, GenItems, NgI);

/* Skip to the start of data */
  RSKIP (fp, Fpreamb.Data_offset - offs);

/* Error checks */
  if (FheadF.Type != ES_FT_FEA) {
    UTwarn ("AFrdEShead - %s: \"%d\"", AFM_ES_UnsType, (int) FheadF.Type);
    return NULL;
  }
  if (! SAME_CSTR (FheadF.Magic, Fpreamb.Magic)) {
    UTwarn ("AFrdEShead - %s", AFM_ES_IdMatch);
    return NULL;
  }
  if (FheadV.Fea_type != ES_FEA_SD) {
     UTwarn ("AFrdEShead - %s: \"%d\"", AFM_ES_UnsFea, (int) FheadV.Fea_type);
     return NULL;
  }

/* Determine the data format */
  if (FheadF.Nshort != 0) {
    AFr.NData.Nchan = FheadF.Nshort;
    AFr.DFormat.Format = FD_INT16;
  }
  else if (FheadF.Nlong != 0) {
    AFr.NData.Nchan = FheadF.Nlong;
    AFr.DFormat.Format = FD_INT32;
  }
  else if (FheadF.Nfloat != 0) {
    AFr.NData.Nchan = FheadF.Nfloat;
    AFr.DFormat.Format = FD_FLOAT32;
  }
  else if (FheadF.Ndouble != 0) {
    AFr.NData.Nchan = FheadF.Ndouble;
    AFr.DFormat.Format = FD_FLOAT64;
  }
  else {
    UTwarn ("AFrdEShead - %s", AFM_ES_UnsData);
    return NULL;
  }
  if (Fpreamb.Record_size != AF_DL[AFr.DFormat.Format] * AFr.NData.Nchan) {
    UTwarn ("AFrdEShead - %s", AFM_ES_UnsEncod);
    return NULL;
  }

/* Get the sampling frequency */
  if (! AF_getGeneric (GenItems, NgI, "record_freq", AFr.DFormat.Swapb,
		       1, ES_DOUBLE, &AFr.Sfreq)) {
    UTwarn ("AFrdEShead - %s", AFM_ES_NoSFreq);
    return NULL;
  }

  /* Other Generic Items */
  if (AF_getGeneric (GenItems, NgI, "start_time", AFr.DFormat.Swapb,
		     1, ES_DOUBLE, &STime)) {
    Nv = sprintf (str, "%.7g", STime);
    AFaddAFspRec ("start_time: ", str, Nv, &AFr.InfoX); 
  }
  /* Pick up "max_value" only if it is a single value */
  if (AF_getGeneric (GenItems, NgI, "max_value", AFr.DFormat.Swapb,
		     1, ES_DOUBLE, &STime)) {
    Nv = sprintf (str, "%.7g", STime);
    AFaddAFspRec ("max_value: ", str, Nv, &AFr.InfoX); 
  }

/* Set the parameters for file access */
  if (FheadF.Ndrec != 0)
    AFr.NData.Nsamp = FheadF.Ndrec * AFr.NData.Nchan;

  AFp = AFsetRead (fp, FT_ESPS, &AFr, AF_FIX_NSAMP_LOW | AF_FIX_NSAMP_HIGH);

  return AFp;
}

/* Get data from a Generic Item */


static int
AF_getGeneric (const char buff[], int Nbuff, const char ID[], int Fbo,
	       int Nval, int Type, void *Val)

{
  int ncID, ncIDX, k, nS, Found, Lw, Nr;
  char sstr[2+2+ES_MAX_gID+4+2+1];
  char *p;
  struct ES_genItem gItem;
  
  ncID = strlen (ID);
  ncIDX = RNDUPV (ncID + 1, 4); 	/* Length of ID rounded up */
  assert (ncIDX <= ES_MAX_gID);

  /* Fill in a Generic Item structure */
  gItem.code = 13;
  gItem.ID_len = (uint2_t) (ncIDX / 4);
  strcpy (gItem.ID, ID);
  for (k = ncID; k < ncIDX; ++k)
    gItem.ID[k] = '\0';
  gItem.count = (uint4_t) Nval;
  gItem.data_code = (uint2_t) Type;

  /* Form the search string */
  if (UTswapCode (Fbo) == DS_SWAP) {
    SWAPBXY (gItem.code, sstr[0]);
    SWAPBXY (gItem.ID_len, sstr[2]);
    memcpy (&sstr[4], gItem.ID, ncIDX);
    SWAPBXY (gItem.count, sstr[ncIDX + 4]);
    SWAPBXY (gItem.data_code, sstr[ncIDX + 8]);
  }
  else {
    memcpy (sstr, &gItem.code, 2);
    memcpy (&sstr[2], &gItem.ID_len, 2);
    memcpy (&sstr[4], &gItem.ID, ncIDX);
    memcpy (&sstr[ncIDX + 4], &gItem.count, 4);
    memcpy (&sstr[ncIDX + 8], &gItem.data_code, 2);
  }
  nS = ncIDX + 10;

  /* Search for the Generic Item in the buffr */
  p = STstrstrNM (buff, sstr, Nbuff, nS);
  Found = 0;
  if (p != NULL) {
    switch (Type) {
    case ES_DOUBLE:
      Lw = 8;
      break;
    case ES_FLOAT:
    case ES_LONG:
      Lw = 4;
      break;
    case ES_SHORT:
      Lw = 2;
      break;
    default:
      Lw = 0;
      assert (0);
      break;
    }
    p += nS;			/* Point to values */
    Nr = Nbuff - (p - buff);	/* No. bytes remaining in buffer */
    if (Nval * Lw <= Nr) {
      if (UTswapCode (Fbo) == DS_SWAP)
	VRswapBytes (p, Val, Lw, Nval);
      else
	memcpy (Val, p, Nval * Lw);
      Found = 1;
    }
  }

  return Found;
}
