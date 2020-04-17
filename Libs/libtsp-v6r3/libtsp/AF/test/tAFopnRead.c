/*
   Test AFopnRead

   $Id: tAFopnRead.c 1.1 2003/05/09 libtsp-v6r3 $
*/

#include <libtsp.h>


int
main (int argc, const char *argv[])

{
  AFILE *AFp;
  int status;
  long int Nsamp, Nchan;
  double Sfreq;
  char Fname[FILENAME_MAX];

/* Parameters for raw files */
  status = AFsetNHpar ("$AF_NOHEADER");
  if (status)
    UThalt ("tAFopnRead: Error return from AFsetNHpar");
  status = AFsetFileType ("$AF_FILETYPE");
  if (status)
    UThalt ("tAFopnRead: Error return from AFsetFileType");

/* Default search path */
  FLpathList (argv[1], "$AUDIOPATH", Fname);

/* Open the file */
  AFp = AFopnRead (Fname, &Nsamp, &Nchan, &Sfreq, stdout);

/* Close the file */
  AFclose (AFp);

  return 0;
}
