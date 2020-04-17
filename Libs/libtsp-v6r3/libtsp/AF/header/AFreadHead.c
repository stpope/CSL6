/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFreadHead (FILE *fp, void *Buf, int Size, int Nv, int Swapb)

Purpose:
  Read and optionally swap audio file header values

Description:
  This routine reads data from an audio file header.  The information to be
  read is considered to be organized as Nv elements each of Size bytes.
  The information (Nv * Size bytes) is read into memory in file byte order.
  Then, optionally each of the Nv elements is byte swapped.

  If the output buffer is a NULL pointer, this routine skips Nv elements
  each of Size bytes.

  If end-of-file is encountered or an error occurs, this routine issues a
  longjmp to the AFR_JMPENV environment set up by the calling routine.

Parameters:
  <-  int AFreadHead
      Number of bytes read (normally equal to Nv * Size)
  <-  void *Buf
      Pointer to a buffer of size Nv * Size bytes or a NULL pointer
   -> int Size
      Size of each element in bytes
   -> int Nv
      Number of elements to be read
   -> int Swapb
      Byte swap flag.  This parameter is not used for Size = 1.  If the bytes
      are to be swapped, Size must be 2, 4 or 8 bytes.
      DS_EB     - File data is in big-endian byte order.  The data will be
                  swapped if the current host uses little-endian byte order.
      DS_EL     - File data is in little-endian byte order data.  The data will
                  be swapped if the current host uses big-endian byte order.
      DS_NATIVE - File data is in native byte order
      DS_SWAP   - File data is byte-swapped

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.21 $  $Date: 2003/05/09 01:21:35 $

-------------------------------------------------------------------------*/

#include <setjmp.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFmsg.h>
#include <libtsp/AFpar.h>

#define BSIZE(x)	((int)(sizeof (x)))
#define MINV(a, b)	(((a) < (b)) ? (a) : (b))
#define FREAD(buf,size,nv,fp)	(int) fread ((char *) buf, (size_t) size, \
					     (size_t) nv, fp)

#define NBUF	256

/* setjmp / longjmp environment */
jmp_buf AFR_JMPENV;		/* Defining point */


int
AFreadHead (FILE *fp, void *Buf, int Size, int Nv, int Swapb)

{
  double Lbuf[NBUF];
  int Nvr, n, Nreq, status;

  if (Buf == NULL) {

    /* Skip data */
    n = 0;
    if (FLseekable (fp)) {
      status = fseek (fp, Size * Nv, SEEK_CUR);
      if (status) {
	UTwarn ("AFreadHead: %s", AFM_FilePosErr);
	longjmp (AFR_JMPENV, 1);
      }
      n = Nv;
    }
    else {
      while (n < Nv) {
	Nreq = MINV (Nv - n, BSIZE (Lbuf) / Size);
	Nvr = FREAD (Lbuf, Size, Nreq, fp);
	n += Nvr;
	if (Nvr < Nreq)
	  break;
      }
    }
  }

  else {
    
    /* Read the data in file byte order */
    n = FREAD (Buf, Size, Nv, fp);

    /* Swap the data if necessary */
    if (Size != 1 && UTswapCode (Swapb) == DS_SWAP)
      VRswapBytes (Buf, Buf, Size, n);
  }

  /* Error messages */
  if (n < Nv) {
    if (ferror (fp)) {
      UTsysMsg ("AFreadHead - %s", AFM_ReadErr);
      longjmp (AFR_JMPENV, 2);
    }
    else {
      UTwarn ("AFreadHead - %s", AFM_UEoF);
      longjmp (AFR_JMPENV, 1);
    }
  }

  return (Size * n);
}
