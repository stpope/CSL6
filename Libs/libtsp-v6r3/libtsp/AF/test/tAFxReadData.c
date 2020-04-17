/*
   Test AFxReadData

   $Id: tAFxReadData.c 1.1 2003/05/09 libtsp-v6r3 $
*/

#include <libtsp.h>

#define MINV(a, b)	(((a) < (b)) ? (a) : (b))

#define NBUF	2560


int
main (int argc, const char *argv[])

{
  AFILE *AFp;
  int status;
  long int Nsamp, Nchan, ioffs, Nrem;
  int Nv, Nr;
  double Sfreq;
  char Fname[FILENAME_MAX];
  float Xf[NBUF];
  double Xd[NBUF];


/* Parameters for raw files */
  status = AFsetNHpar ("$AF_NOHEADER");
  if (status)
    UThalt ("tAFreadData: Error return from AFsetNHpar");
  status = AFsetFileType ("$AF_FILETYPE");
  if (status)
    UThalt ("tAFreadData: Error return from AFsetFileType");

/* Default search path */
  FLpathList (argv[1], "$AUDIOPATH", Fname);

/* Open the file */
  AFp = AFopnRead (Fname, &Nsamp, &Nchan, &Sfreq, stdout);

/* Read the data */
  ioffs = 0L;
  Nrem = Nsamp;
  while (Nrem > 0L) {
    Nv = (int) MINV (Nrem, NBUF);
    if (argv[2][1] == 'F')
      Nr = AFfReadData (AFp, ioffs, Xf, Nv);
    else
      Nr = AFdReadData (AFp, ioffs, Xd, Nv);
    ioffs += Nv;
    Nrem -= Nv;
  }
  printf ("tAFxReadData: %s: Total samples: %ld\n", argv[2], ioffs);

/* Close the file */
  AFclose (AFp);

  return 0;
}
