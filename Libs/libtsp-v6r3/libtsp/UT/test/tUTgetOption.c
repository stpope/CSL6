/*
   Test UTgetOption

   $Id: tUTgetOption.c 1.6 1999/06/04 libtsp-v6r3 $
*/

#include <stdlib.h>	/* EXIT_SUCCESS */

#include <libtsp.h>

#define PROGRAM	"tUTgetOption"
#define VERSION	"V1R3 1995/01/07"

/* Option tables and usage message */
static const char *nullTable[] = { NULL };
static const char *OptTable[] = {
  "-d#", "--d*elay=",
  "-lim*it#", "--lim*it*=",
  "-a", "--a*arg",
  "-h", "--h*elp",
  "-v", "--v*ersion",
  "--",
  NULL
  };
static const char Usage[] = "\
Usage: %s [options] argA argB\n\
Options:\n\
  -d D1:D2, --delay=D1:D2  Specify a delay range.\n\
  -limit LIMITS, --limit=LIMITS  Limits.\n\
  -a, --aarg               Test option.\n\
  -h, --help               Print a list of options and exit.\n\
  -v, --version            Print the version number and exit.";


int
main (int argc, const char *argv[])

{
  int Index;
  const char *OptArg;
  const char **optt;

  int n;
  int ip;

/* Decode options */
  Index = 1;
  optt = OptTable;
  while (Index < argc) {
    ip = Index;
    n = UTgetOption (&Index, argc, argv, optt, &OptArg);
    switch (n) {
    case 0:
      /* Ordinary argument */
      printf ("Ordinary argument: %s\n", OptArg);
      break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
      if (OptArg != NULL)
	printf ("String: %-12s, Option %-3d: %-12s, value: %-6s\n",
		argv[ip], n, optt[n-1], OptArg);
      else
	printf ("String: %-12s, Option %-3d: %-12s \n", argv[ip],
		n, optt[n-1]);
      break;
    case 7:
    case 8:
      /* Usage message */
      UTwarn (Usage, PROGRAM);
      exit (EXIT_SUCCESS);
      break;
    case 9:
    case 10:
      /* Version */
      printf ("%s, %s\n", PROGRAM, VERSION);
      exit (EXIT_SUCCESS);
      break;
    case 11:
      /* Stop interpreting options */
      printf ("Stop option scan, arguments follow\n");
      optt = nullTable;
      break;
    default:
      /* Option error */
      UThalt (Usage, PROGRAM);
      break;
    }
  }
  return 0;
}
