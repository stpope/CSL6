/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFupdWVhead (AFILE *AFp)

Purpose:
  Update header information in a RIFF WAVE file

Description:
  This routine updates the data length fields of a RIFF WAVE file.  The file
  is assumed to have been opened with routine AFopnWrite.  This routine also
  writes information chunks at the end of the file.

Parameters:
  <-  int AFupdWVhead
      Error code, zero for no error
   -> AFILE *AFp
      Audio file pointer for an audio file opened by AFopnWrite

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.34 $  $Date: 2003/11/03 18:48:31 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <setjmp.h>
#include <string.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFdataio.h>
#include <libtsp/AFheader.h>
#define AF_DATA_LENGTHS
#include <libtsp/AFpar.h>
#include <libtsp/WVpar.h>

#define ICEILV(n,m)	(((n) + ((m) - 1)) / (m))	/* int n,m >= 0 */
#define NELEM(array)	((int) ((sizeof array) / (sizeof array[0])))
#define RNDUPV(n,m)	((m) * ICEILV (n, m))		/* Round up */

#define MCOPY(src,dest)		memcpy ((void *) (dest), \
					(const void *) (src), sizeof (dest))
#define WRPAD(fp,size,align) \
     AFwriteHead (fp, NULL, 1, (int) (RNDUPV(size, align) - (size)), \
		  DS_NATIVE);
#define SAME_CSTR(str,ref) 	(memcmp (str, ref, sizeof (str)) == 0)


#define ALIGN		2	/* Chunks padded out to a multiple of ALIGN */

/* setjmp / longjmp environment */
extern jmp_buf AFW_JMPENV;

static const char *
AF_findRec (const char *Id[], const struct AF_info *InfoS);
static void
AF_setAFSP (struct WV_AFSP *afsp, const struct AF_info *InfoS);
static void
AF_setDISP (struct WV_DISP *disp, const struct AF_info *InfoS);
static void
AF_setLIST_INFO (struct WV_LIST_INFO *info, const struct AF_info *InfoS);
static int
AF_wrAFSP (FILE *fp, const struct WV_AFSP *afsp);
static int
AF_wrDISP (FILE *fp, const struct WV_DISP *disp);
static int
AF_wrLIST_INFO (FILE *fp, const struct WV_LIST_INFO *info);


int
AFupdWVhead (AFILE *AFp)

{
  long int Nbytes, Ldata;
  uint4_t val;
  struct WV_AFSP afsp;
  struct WV_DISP disp;
  struct WV_LIST_INFO info;

/* Set the long jump environment; on error return a 1 */
  if (setjmp (AFW_JMPENV))
    return 1;		/* Return from a header write error */

/* This routine assumes that the header has the following layout
     offset  contents
        4     "RIFF" chunk
       12      "fmt" chunk
      ...     other data
      D-20    "fact" chunk (if present)
      D-16     Chunk size (4)
      D-12     Number of samples
      D-8     "data" chunk
      D-4       Chunk size
        D       Audio data

   Header size:
               RIFF  fmt   fact   Data   Total
               WAVE             preamble
   PCM          12    24     -     8       44
   non-PCM      12    26    12     8       58
   PCM-Ext      12    48     -     8       68
   non-PCM-Ext  12    48    12     8       80
*/

/* Add a padding byte to the sound data; this padding byte is not included
   in the data chunk cksize field, but is included in the RIFF chunk
   cksize field
*/
  Ldata = AF_DL[AFp->Format] * AFp->Nsamp;
  Nbytes = AFp->Start + Ldata;
  Nbytes += WRPAD (AFp->fp, Nbytes, ALIGN);	/* Write pad byte */

/* Generate the information records */
  AF_setAFSP (&afsp, &AFp->InfoS);
  AF_setDISP (&disp, &AFp->InfoS);
  AF_setLIST_INFO (&info, &AFp->InfoS);

/* Write the information records (at the end of the file) */
  if (afsp.cksize > 4) {
    AF_wrAFSP (AFp->fp, &afsp);
    Nbytes += 8 + RNDUPV (afsp.cksize, ALIGN);
  }
  if (disp.cksize > 4) {
    AF_wrDISP (AFp->fp, &disp);
    Nbytes += 8 + RNDUPV (disp.cksize, ALIGN);
  }
  if (info.cksize > 4) {
    Nbytes += 8 + RNDUPV (info.cksize, ALIGN);
    AF_wrLIST_INFO (AFp->fp, &info);
  }

/* Update the "RIFF" chunk cksize field (at the beginnning of the file) */
  val = (uint4_t) (Nbytes - 8);
  if (AFseek (AFp->fp, 4L, NULL))	/* Back at the beginning of the file */
    return 1;
  WHEAD_V (AFp->fp, val, DS_EL);

/* Update the "fact" chunk SampleLength field */
  assert (AFp->Start == 44 || AFp->Start == 58 ||
	  AFp->Start == 68 || AFp->Start == 80);
  if (AFp->Start == 58 || AFp->Start == 80) {
    if (AFseek (AFp->fp, AFp->Start - 12, NULL))
      return 1;
    val = (uint4_t) (AFp->Nsamp / AFp->Nchan);
    WHEAD_V (AFp->fp, val, DS_EL);
  }

/* Update the "data" chunk cksize field */
  if (AFseek (AFp->fp, AFp->Start - 4, NULL))
    return 1;
  val = (uint4_t) Ldata;
  WHEAD_V (AFp->fp, val, DS_EL); /* Number of data bytes */

  return 0;
}

/* Fill in the afsp chunk */


static void
AF_setAFSP (struct WV_AFSP *afsp, const struct AF_info *InfoS)

{
  MCOPY ("afsp", afsp->ckid);
  if (InfoS->N > 0)
    afsp->cksize = (uint4_t) (4 + InfoS->N);
  else
    afsp->cksize = (uint4_t) 4;
  MCOPY (FM_AFSP, afsp->AFspID);
  afsp->text = InfoS->Info;

  return;
}

/* Write the afsp chunk */


static int
AF_wrAFSP (FILE *fp, const struct WV_AFSP *afsp)

{
  int offs;

  offs = WHEAD_S (fp, afsp->ckid);
  offs += WHEAD_V (fp, afsp->cksize, DS_EL);
  offs += WHEAD_S (fp, afsp->AFspID);
  offs += WHEAD_SN (fp, afsp->text, afsp->cksize - 4);

  assert (8 + (int) afsp->cksize == offs);
  offs += WRPAD (fp, offs, ALIGN);

  return offs;
}

/* Fill in the DISP/text chunk */


static void
AF_setDISP (struct WV_DISP *disp, const struct AF_info *InfoS)

{
  const char *title;
  static const char *TitleID[] = {"title:", "display_name:", "name:",
				  "user_comment:", NULL};

  MCOPY ("DISP", disp->ckid);
  title = AF_findRec (TitleID, InfoS);
  if (title == NULL)
    disp->cksize = (uint4_t) 4;
  else
    disp->cksize = (uint4_t) (4 + strlen (title) + 1);
  disp->type = (uint4_t) (CF_TEXT);
  disp->text = title;

  return;
}

/* Write the DISP/text chunk */


static int
AF_wrDISP (FILE *fp, const struct WV_DISP *disp)

{
  int offs;

  offs = WHEAD_S (fp, disp->ckid);
  offs += WHEAD_V (fp, disp->cksize, DS_EL);
  offs += WHEAD_V (fp, disp->type, DS_EL);
  offs += WHEAD_SN (fp, disp->text, disp->cksize - 4);

  assert (8 + (int) disp->cksize == offs);
  offs += WRPAD (fp, offs, ALIGN);

  return offs;
}

/* Fill in the LIST/INFO chunk */


static void
AF_setLIST_INFO (struct WV_LIST_INFO *info, const struct AF_info *InfoS)

{
  int i, k, Nk, size;
  const char *text;
  static const char *IID[] = {
    "IARL", "archival_location:", NULL,
    "IART", "artist:", NULL,
    "ICMS", "commissioned:", NULL,
    "ICRD", "creation_date:", "date:", "recording_date:", NULL,
    "ICMT", "comments:", NULL,
    "ICOP", "copyright:", NULL,
    "IENG", "engineer:", NULL,
    "IGNR", "genre:", NULL,
    "IKEY", "keywords:", NULL,
    "IMED", "medium:", NULL,
    "INAM", "name:", NULL,
    "IPRD", "product:", NULL,
    "ISBJ", "subject:", NULL,
    "ISFT", "software:", "program:", NULL,
    "ISRC", "source:", NULL,
    "ISRF", "source_form:", NULL,
    "ITCH", "technician:", NULL,
    "ICMT", "user:", NULL	/* user: in comments field */
  };
  
  MCOPY ("LIST", info->ckid);
  MCOPY ("INFO", info->listid);

  Nk = (int) NELEM (IID);
  size = 4;
  k = 0;
  i = 0;
  while (k < Nk && i < WV_N_LIST_INFO) {

    /* Look for a record with the given keywords */
    text = AF_findRec (&IID[k+1], InfoS);	/* Keywords */
    if (text != NULL) {
      MCOPY (IID[k], info->listitem[i].ckid);	/* Item ID */
      info->listitem[i].cksize = strlen (text) + 1;
      info->listitem[i].text = text;
      size += 8 + RNDUPV (info->listitem[i].cksize, ALIGN);
      ++i;
    }

    /* Skip over the item ID and keywords */
    for (; IID[k] != NULL; ++k)
      ;
    ++k;
  }

  /* INFO chunk size */
  info->cksize = size;
  info->N = i;

  return;
}

/* Write the LIST/INFO chunk */


static int
AF_wrLIST_INFO (FILE *fp, const struct WV_LIST_INFO *info)

{
  int offs, i;

  offs = WHEAD_S (fp, info->ckid);
  offs += WHEAD_V (fp, info->cksize, DS_EL);
  offs += WHEAD_S (fp, info->listid);
  for (i = 0; i < info->N; ++i) {
    offs += WHEAD_S (fp, info->listitem[i].ckid);
    offs += WHEAD_V (fp, info->listitem[i].cksize, DS_EL);
    offs += WHEAD_SN (fp, info->listitem[i].text, info->listitem[i].cksize);
    offs += WRPAD (fp, info->listitem[i].cksize, ALIGN);
  }
  assert (8 + (int) info->cksize == offs);

  return offs;
}

/* Search for named records in an AFsp information structure */


static const char *
AF_findRec (const char *Id[], const struct AF_info *InfoS)

{
  int i;
  const char *p;

  p = NULL;
  for (i = 0; Id[i] != NULL; ++i) {
    p = AFgetInfoRec (Id[i], InfoS);
    if (p != NULL)
      break;
  }

  if (p != NULL)
    p = STtrimIws (p);

  return p;
}
