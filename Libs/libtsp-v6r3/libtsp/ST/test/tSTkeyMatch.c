/*
   Test STkeyMatch

   $Id: tSTkeyMatch.c 1.5 1999/06/04 libtsp-v6r3 $
*/

#include <libtsp.h>

static const char *Keytable [] = {
  "destr*oy",
  "h",
  "what*isthis",
  "abc*de*f",
  "**well",
  NULL
};


int
main (int argc, const char *argv[])

{
  int n;

  if (argc == 1)
    n = STkeyMatch (NULL, Keytable);
  else
    n = STkeyMatch (argv[1], Keytable);

  if (n >= 0)
    printf ("String \"%s\" matches key %d, %s\n", argv[1], n, Keytable[n]);
  else if (argc == 1)
    printf ("String \"(null)\", no match\n");
  else
    printf ("String \"%s\", no match\n", argv[1]);

  return 0;
}
