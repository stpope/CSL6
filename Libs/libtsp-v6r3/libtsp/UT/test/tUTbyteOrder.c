/*
   Test UTbyteOrder

   $Id: tUTbyteOrder.c 1.8 1999/06/04 libtsp-v6r3 $
*/

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/UTpar.h>


int
main (int argc, const char *argv[])

{
  int BO, i;
  static const char *DataSwap[] = {
    "big-endian",
    "little-endian",
    "native",
    "swap"
  };

  BO = UTbyteOrder();
  printf ("Host: %s\n", DataSwap[BO]);

  for (i = 0; i < 4; ++i) {
    BO = UTbyteCode (i);
    printf ("  UTbyteCode (%s) = %s\n", DataSwap[i], DataSwap[BO]);
    BO = UTswapCode (i);
    printf ("  UTswapCode (%s) = %s\n", DataSwap[i], DataSwap[BO]);
  }

  return 0;
}
