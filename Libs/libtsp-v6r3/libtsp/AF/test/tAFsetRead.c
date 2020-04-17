/*
   Test AFsetRead

   $Id: tAFsetRead.c 1.13 2003/11/03 libtsp-v6r3 $
*/

#include <string.h>

#include <libtsp.h>
#include <libtsp/AFheader.h>
#define AF_DATA_LENGTHS
#include <libtsp/AFpar.h>


int
main (int argc, const char *argv[])

{
  AFILE *AFp;
  FILE *fp;
  long int Nbytes, Dstart, Ldata, Nsamp;
  int i, Fix, Format;
  const char *Fname;
  struct AF_read AFr;

/* Get the data */
  Fname = argv[1];
  sscanf (argv[2], "%ld", &Nbytes);
  sscanf (argv[3], "%d",  &Format);
  sscanf (argv[4], "%ld", &Dstart);
  sscanf (argv[5], "%ld", &Ldata);
  sscanf (argv[6], "%ld", &Nsamp);
  sscanf (argv[7], "%d",  &Fix);

/* Create a file */
  if (strcmp (Fname, "-") != 0) {
    fp = fopen (Fname, "wb");
    for (i = 0; i < Nbytes; ++i)
      fwrite ("\0", 1, 1, fp);
    fseek (fp, Dstart, 0);
  }
  else
    fp = stdin;

  AFr.Sfreq = 8000.0;
  AFr.DFormat.Format = Format;
  AFr.DFormat.Swapb = DS_EB;
  AFr.DFormat.ScaleF = 1.0;
  AFr.DFormat.NbS = 0;
  AFr.NData.Ldata = Ldata;
  AFr.NData.Nsamp = Nsamp;
  AFr.NData.Nchan = 2L;
  AFr.NData.SpkrConfig[0] = '\0';
  AFr.InfoX.Info = NULL;
  AFr.InfoX.N = 0;
  AFr.InfoX.Nmax = 0;

  AFp = AFsetRead (fp, FT_SPPACK, &AFr, Fix);

  if (AFp == NULL)
    printf ("tAFsetRead: Error return from AFsetRead\n");
  else
    printf ("AFsetRead: Ldata: %ld ==> %ld, Nsamp: %ld ==> %ld\n",
	    Ldata, AF_DL[AFp->Format] * AFp->Nsamp, Nsamp, AFp->Nsamp);

/* Close the file */
  fclose (fp);

  return 0;
}
