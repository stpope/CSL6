/*
   Test STkeyXpar

   $Id: tSTkeyXpar.c 1.4 1999/06/04 libtsp-v6r3 $
*/

#include <libtsp.h>

static const char *Keytable [] = {
  "fir*st",
  "sec*ond",
  "third**",
  NULL
};


int
main (int argc, const char *argv[])

{
  int n;
  char Par[1024];

  n = STkeyXpar (argv[1], "=", "\"\"", Keytable, Par);

  if (n >= 0)
    printf ("tSTkeyXpar: %s = %s\n", Keytable[n], Par);
  else
    printf ("tSTkeyXpar: no match\n");

  return 0;
}
