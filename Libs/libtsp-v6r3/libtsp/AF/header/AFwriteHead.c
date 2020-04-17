/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFwriteHead (FILE *fp, const void *Buf, int Size, int Nv, int Swapb)

Purpose:
  Write (with optionally swap) audio file header values

Description:
  This routine writes data to an audio file header.  The information to be
  written is considered to be organized as Nv elements each of Size bytes.
  Optionally each of the Nv elements is byte swapped.  The data (Nv * Size
  bytes) is then written to the file.  If input buffer is buffer is a NULL
  pointer, this routine writes zero-valued bytes to the file.

  If an error occurs, this routine issues a longjmp to the AFW_JMPENV
  environment set up by the calling routine.

Parameters:
  <-  int AFwriteHead
      Number of bytes written
   -> FILE *fp
      File pointer associated with the file
   -> const void *Buf
      Pointer to a buffer of size Nelem * size or a NULL pointer
   -> int Size
      Size of each element in bytes
   -> int Nv
      Number of elements to be written
   -> int Swapb
      Byte swap flag.  This parameter is not used for size = 1.  If the bytes
      are to be swapped, size must be 2, 4 or 8 bytes.
      DS_EB     - File data is in big-endian byte order.  The data will be
      	          swapped if the current host uses little-endian byte order.
      DS_EL     - File data is in little-endian byte order data.  The data will
                  be swapped if the current host uses big-endian
		  byte order.
      DS_NATIVE - File data is in native byte order
      DS_SWAP   - File data is byte-swapped

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.25 $  $Date: 2003/05/09 01:21:36 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFpar.h>
#include <libtsp/AFmsg.h>
#include <libtsp/UTtypes.h>

#define BSIZE(x)	((int)(sizeof (x)))
#define MINV(a, b)	(((a) < (b)) ? (a) : (b))

#define FWRITE(buf,size,nv,fp)	(int) fwrite ((const char *) buf, \
					      (size_t) size, (size_t) nv, fp)

#define NBUF	256	/* Number of doubles */
#define NPAD	16	/* Number of bytes */

/* setjmp / longjmp environment */
jmp_buf AFW_JMPENV;		/* Defining point */


int
AFwriteHead (FILE * fp, const void *Buf, int Size, int Nv, int Swapb)

{
  int n, Nreq, Nw;
  const char *p;
  double Lbuf[NBUF];
  static const char Pad[NPAD] = "";		/* Initialized to zero */

  n = 0;
  if (Buf == NULL) {

    /* Write zeros to the file */
    Nv = Nv * Size;	/* Change to units of bytes */
    Size = 1;
    while (n < Nv) {
      Nreq = MINV (Nv - n, NPAD);
      Nw = FWRITE (Pad, 1, Nreq, fp);
      n += Nw;
      if (Nw < Nreq)
	break;
    }
  }

  else if (Size != 1 && UTswapCode (Swapb) == DS_SWAP) {

    /* Swapped data passes through a temporary buffer */
    p = (const char *) Buf;
    while (n < Nv) {

      Nreq = MINV (Nv - n, BSIZE (Lbuf) / Size);
      VRswapBytes (p, Lbuf, Size, Nreq);
      Nw = FWRITE (Lbuf, Size, Nreq, fp);
      p += Nw * Size;
      n += Nw;
      if (Nw < Nreq)
	break;
    }
  }

  else

    /* Non-swapped data is written directly from the input buffer */
    n += FWRITE (Buf, Size, Nv, fp);

  if (n < Nv) {
    UTsysMsg ("AFwriteHead - %s", AFM_WriteErr);
    longjmp (AFW_JMPENV, 1);
  }

  return (Size * n);
}
