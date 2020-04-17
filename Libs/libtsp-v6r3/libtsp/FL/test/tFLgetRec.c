/*
   Test FLgetRec

   tFLgetRec <filename> <comment_chars> <cont_chars> <echo>
   (filename can be "stdin")

   $Id: tFLgetRec.c 1.3 1999/06/04 libtsp-v6r3 $
*/

#include <string.h>

#include <libtsp.h>


int
main (int argc, const char *argv[])

{
  FILE *fp;
  char *p;
  int echo;

  if (strcmp (argv[1], "stdin") == 0)
    fp = stdin;
  else
    fp = fopen (argv[1], "r");
  sscanf (argv[4], "%d", &echo); 

  p = "";
  while (p != NULL) {
    p = FLgetRec (fp, argv[2], argv[3], echo);
    if (p == NULL)
      printf ("FLgetRec: end-of-file\n");
    else
      printf ("FLgetRec: %.40s\n", p);
  }

  return 0;
}
