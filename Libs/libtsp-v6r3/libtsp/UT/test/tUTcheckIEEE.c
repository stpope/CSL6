/*
   Test UTcheckIEEE

   $Id: tUTcheckIEEE.c 1.5 1999/06/04 libtsp-v6r3 $
*/

#include <libtsp/nucleus.h>


int main (int argc, char *argv[])

{
  if (UTcheckIEEE () == 1)
    printf ("UTcheckIEEE: IEEE float representation\n");
  else
    printf ("UTcheckIEEE: non-IEEE float representation\n");

  return 0;
}
