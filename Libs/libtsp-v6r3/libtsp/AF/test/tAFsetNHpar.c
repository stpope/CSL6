/*
   Test AFsetNHpar

   $Id: tAFsetNHpar.c 1.9 2003/04/27 libtsp-v6r3 $
*/

#include <libtsp.h>
#include <libtsp/AFpar.h>


int
main (int argc, const char *argv[])

{
  int ErrCode;
  struct AF_opt *AFopt;

/* Parameters for raw files */
  ErrCode = AFsetNHpar (argv[1]);
  printf ("\"%s\"\n", argv[1]);

/* Print the parameters for headerless files*/
  AFopt = AFoptions ();
  printf (" ErrCode = %d, Format = %d, Start = %ld, Sfreq = %g\n",
	  ErrCode, AFopt->NHpar.Format, AFopt->NHpar.Start,
	  AFopt->NHpar.Sfreq);
  printf ("   Swapb = %d, Nchan = %ld, ScaleF = %g\n",
	  AFopt->NHpar.Swapb, AFopt->NHpar.Nchan, AFopt->NHpar.ScaleF);

  return 0;
}
