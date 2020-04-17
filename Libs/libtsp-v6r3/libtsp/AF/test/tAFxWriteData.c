/*
   Test AFxWriteData

   $Id: tAFxWriteData.c 1.3 2003/11/03 libtsp-v6r3 $
*/

#include <libtsp.h>
#include <libtsp/AFpar.h>

#define N	12


int
main (int argc, const char *argv[])

{
  AFILE *AFp;
  int Ftype, Dformat;
  long int Nsamp, Nchan;
  double Sfreq;
  double xi[N+2];
  struct AF_opt *AFopt;
  static double xo[N] = {30., 40., 60., -10., -40.,
			 80., 60.25, -90.25, 32000., -32000., 33000., -33000.};

  sscanf (argv[2], "%d", &Ftype);
  sscanf (argv[3], "%d", &Dformat);
  sscanf (argv[4], "%ld", &Nchan);
  sscanf (argv[5], "%lg", &Sfreq);
  AFopt = AFoptions ();
  if (argc > 6)
    sscanf (argv[6], "%d", &(AFopt->NbS));
  if (argc > 7)
    AFsetSpeaker (argv[7]);

  VRdScale (1./32768., xo, xo, N);	/* scale to [-1, +1) */

  AFsetInfo ("\ntitle: Test file");
  AFp = AFopnWrite (argv[1], Ftype, Dformat, Nchan, Sfreq, stdout);
  AFdWriteData (AFp, xo, N) ;
  AFclose (AFp);

  /* Set the parameters for the open for read */
  AFopt->NHpar.Format = Dformat;
  AFopt->NHpar.Swapb = Ftype / FTW_SUBTYPE_MOD;

  AFp = AFopnRead (argv[1], &Nsamp, &Nchan, &Sfreq, stdout);
  /* Read several times to test random access */
  /* File samples:  0 1 2 3 4 5 6 7      (shown for N = 8)
     (1) samples -1 0 1 2 3 4 5 6 7 8 9  === -1 to N+1, total N+2 samples
     (2) samples        2 3 4 5 6 7      === 2 to N-1, total N-2 samples
     (3) samples          3 4            === 3 to 4, total 2 samples
     (4) samples                6 7      === 6 to 7, total 2 samples
  */

  AFdReadData (AFp, -1L, xi, N+2);
  AFdReadData (AFp, 2L, &xi[3], N-2);
  AFdReadData (AFp, 3L, &xi[4], 2);
  AFdReadData (AFp, 6L, &xi[7], 2);
  AFclose (AFp);

  VRdScale (32768., xi, xi, N+2);
  VRdPrint (stdout, "Readback data:", xi, N+2);

  return 0;
}
