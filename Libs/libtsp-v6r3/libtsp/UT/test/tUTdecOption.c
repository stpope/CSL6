/*
   Test UTdecOption

   $Id: tUTdecOption.c 1.6 1999/06/04 libtsp-v6r3 $
*/

#include <libtsp/nucleus.h>


int
main (int argc, const char *argv[])

{
  const char *OptArg;
  int ier;

  ier = UTdecOption (argv[1], argv[2], &OptArg);
  if (OptArg != NULL)
    printf ("String: %-8s Option: %-8s stat: %-4d Arg: %s\n",
	    argv[1], argv[2], ier, OptArg);
  else
    printf ("String: %-8s Option: %-8s stat: %-4d Arg: (null)\n",
	    argv[1], argv[2], ier);

  return 0;
}
