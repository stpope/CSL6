/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFrdTextAFsp (FILE *fp, int Size, const char Ident[],
                    struct AF_infoX *InfoX, int Align)
  void AFaddAFspRec (const char Ident[], const char line[], int Size,
                     struct AF_infoX *InfoX)

Purpose:
  Read text and append as a record in an AFsp information structure
  Append a line of text as a record in an AFsp information structure

Description:
  AFaddAFspRec
  If the input text is not empty, a record is added to the AFsp information
  structure.  This record consists of an identification string followed by the
  text. Additional editing of the text string is performed:
    - Carriage return / newline combinations are replaced by a newline
    - Nulls are replaced by newlines
    - Carriage returns are replaced by newlines

  AFrdTextAFsp reads a string (Size bytes) and if not already at a multiple of
  Align, skips ahead in the file to the next multiple of the Align value. If
  the string has the AFsp identifier as the first 4 characters, the string is
  assumed to be preformatted and the identifier Ident is not used. If the
  string does not begin with the AFsp identifier, the string is pass to
  AFaddAFspRec for processig.

Parameters:
  <-  int AFrdTextAFsp
      Number of characters read (including any skipped characters)
   -> FILE *fp
      File pointer to the audio file
   -> int Size
      Number of characters in the text string
   -> const char Ident[]
      Identification string to be prefixed to the text
  <-> struct AF_infoX *InfoX
      AFsp information structure
   -> int Align
      Alignment requirement.  If Size is not a multiple of Align, file data
      up to the next multiple of Align is skipped, leaving the file positioned
      at that point.

   -> const char Ident[]
      Identification string to be prefixed to the text
   -> const char line[]
      Input text line
   -> int Size
      Number of characters in the text string
  <-> struct AF_infoX *InfoX
      AFsp information structure   

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.10 $  $Date: 2003/11/04 12:45:14 $

-------------------------------------------------------------------------*/

#include <ctype.h>
#include <string.h>

#include <libtsp.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFpar.h>
#include <libtsp/AFmsg.h>

#define ICEILV(n,m)	(((n) + ((m) - 1)) / (m))	/* int n,m >= 0 */
#define RNDUPV(n,m)	((m) * ICEILV (n, m))		/* Round up */

#define SSIZE(str)	(sizeof (str) - 1)		/* ignore null */

#define NBUFF 512

static int
AF_trimTail (const char Si[], int nc, char So[]);
static int
AF_fixCR (const char Si[], int nc, char So[]);
static void
AF_addRec (char str[], int N, struct AF_infoX *InfoX);


int
AFrdTextAFsp (FILE *fp, int Size, const char Ident[], struct AF_infoX *InfoX,
	      int Align)

{
  int offs;
  int Nu;
  char buff[NBUFF];
  char *t, *u;

  /* Allocate a buffer for long strings */
  if (Size + 1 > NBUFF)
    t = (char *) UTmalloc (Size + 1);
  else
    t = buff;

  /* Read the text */
  offs = RHEAD_SN (fp, t, Size);

  /* Move ahead in the file to a multiple of Align */
  offs += RSKIP (fp, RNDUPV (Size, Align) - offs);

  /* Check for AFsp records */
  if (Size > SSIZE (FM_AFSP) && memcmp (t, FM_AFSP, SSIZE (FM_AFSP)) == 0) {
    u = t + SSIZE (FM_AFSP);		/* Past the AFsp identifier */

    /* Clean up the end of the string */
    Nu = AF_trimTail (u, Size - SSIZE (FM_AFSP), u);

    /* Add the string to the info structure */
    if (Nu > 1)
      AF_addRec (u, Nu, InfoX);
  }

  else {

    /* Add the text to the info structure */
    AFaddAFspRec (Ident, t, Size, InfoX);

  }

  /* Release the buffer */
  if (Size + 1 > NBUFF)
    UTfree (t);

  return offs;
}

/* Place a line into the info string */


void
AFaddAFspRec (const char Ident[], const char line[], int Size,
	      struct AF_infoX *InfoX)

{
  int Ni, Nt;
  char buff[NBUFF];
  char *t;

  Ni = strlen (Ident);

  /* Allocate a buffer for long strings */
  if (Size + Ni + 1 > NBUFF)
    t = (char *) UTmalloc (Size + Ni + 1);
  else
    t = buff;

  /* Sanitize the string */
  memcpy (t, Ident, Ni);
  Nt = AF_trimTail (line, Size, &t[Ni]);    /* Nt includes trailing null */
  Nt = AF_fixCR (t, Nt+Ni-1, t);

  /* Add the string to the info structure (if it is longer than Ident) */
  if (Nt > Ni + 1)
    AF_addRec (t, Nt, InfoX);

  /* Release the buffer */
  if (Size + Ni + 1 > NBUFF)
    UTfree (t);

  return;
}

/* Trim trailing white space, null characters and '\377' characters.
   (The latter occur as padding in some old audio files.)
   - replace \r\n combinations with \n (occur in some WAVE files)
   - replace \r with \n
   - replace \0 with \n
   A trailing null character is added.

   The number of characters, including the trailing null, is returned.
*/


static int
AF_fixCR (const char Si[], int nc, char So[])

{
  int i, j;

  for (i = 0, j = 0; i < nc; ++i, ++j) {
    if (Si[i] == '\r' && Si[i+1] == '\n') {
      ++i;
      So[j] = Si[i];
    }
    else if (Si[i] == '\0' || Si[i] == '\r')
      So[j] = '\n';
    else
      So[j] = Si[i];
  }
  nc = j;

  So[nc] = '\0';
  nc = nc + 1;

  return nc;
}

/* Trim tailing white space, nulls and '\377' characters, leaving a single
   trailing null. The input and output string pointers may be the same. */

static int
AF_trimTail (const char Si[], int nc, char So[])

{
  int i;

  for (i = nc-1; i >= 0; --i) {
    if (! (isspace ((int)(Si[i])) || Si[i] == '\0' || Si[i] == '\377'))
      break;
  }
  nc = i + 1;

  if (Si != So)
    memcpy (So, Si, nc);

  So[nc] = '\0';
  nc = nc + 1;

  return nc;
}

static void
AF_addRec (char str[], int N, struct AF_infoX *InfoX)

{
  int Np;

  Np = InfoX->N;
  if (N > InfoX->Nmax - Np) {
    UTwarn ("AFrdTextAFsp - %s", AFM_TruncInfo);
    N = InfoX->Nmax - Np;
  }

  memcpy (&InfoX->Info[Np], str, N);
  InfoX->N = InfoX->N + N;
}
