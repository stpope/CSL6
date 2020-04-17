/*
  Test for C preprocessor symbols

  $Id: tcpp.c 1.9 2001/10/05 libtsp-v6r3 $

  The symbols defined by gcc can be determined as follows.
  % touch x.c
  % gcc -dM -E x.c

  Dec Ultrix 4.2
    cc 2.1 compiler
      __ultrix = 1
      ultrix = 1
      __mips = 1
      mips = 1
      __MIPSEL = 1
      MIPSEL = 1
      __unix = 1
      unix = 1
    gcc 2.7.2 compiler
      __ultrix__ = 1
      __ultrix = 1
      ultrix = 1
      __mips__ = 1
      __mips = 1
      mips = 1
      __MIPSEL__ = 1
      __MIPSEL = 1
      MIPSEL = 1
      __unix__ = 1
      __unix = 1
      unix = 1
      __STDC__ = 1
      __GNUC__ = 2

  SunOS 4.1.4
    cc compiler
      sun = 1
      sparc = 1
      unix = 1
    gcc 2.7.2.1
      __sun__ = 1
      __sun = 1
      sun = 1
      __sparc__ = 1
      __sparc = 1
      sparc = 1
      __unix__ = 1
      __unix = 1
      unix = 1
      __unix = 1
      __STDC__ = 1
      __GNUC__ = 2
    CC 2.0.1 compiler
      sun = 1
      sparc = 1
      unix = 1
      __STDC__ = 0
      cplusplus = 1

  SunOS 5.5
    gcc 2.7.2
      sun = 1
      __sun__ = 1
      __sun = 1
      sun = 1
      __sparc__ = 1
      __sparc = 1
      sparc = 1
      __unix__ = 1
      __unix = 1
      unix = 1
      __svr4__ = 1
      __SVR4 = 1
      __STDC__ = 1
      _GNUC__ = 2
    cc (SC4.0) -Xc
      __sun = 1
      __sparc = 1
      __unix = 1
      __SVR4 = 1

  gcc (CYGWIN 1.3)
    gcc 2.95.3-5
      __i386__ 1
      _X86_ 1
      __i386 1
      __i686 1
      __GNUC_MINOR__ 95
      pentiumpro 1
     __CYGWIN__ 1
     __pentiumpro 1
     i386 1
     i686 1
     __pentiumpro__ 1
     __unix 1
     __unix__ 1
     __i686__ 1
    __GNUC__ 2
    __CYGWIN32__ 1
    unix 1

  MSVC 6
    _WIN32

Compiler identifications:
  symbol: __STDC__ = 1

  HPUX 9.01
    gcc? (Not a full list, taken from notes)
      hpux = 1
      unix = 1

  SGI 5.2
    gcc (not a full list, taken from notes)
      __mips__ = 1
      __mips = 1
      mips =1
      __sgi__ = 1
      __sgi   = 1
      __MIPSEB__ = 1
      __MIPSEB = 1
      __unix = 1
      unix = 1
  MS-DOS onPC-486
    gcc (not a full list, taken from notes)
      __MSDOS__ = 1
      __MSDOS = 1
      MSDOS = 1
      __unix__ = 1
      __unix = 1
      unix = 1
      __GNUCC__ = 1
*/

#include <stdio.h>

#define PRINT(sym, val)	printf ("  symbol: %s = %d\n", sym, val)

int
main (int argc, const char *argv[])

{
  printf ("C preprocessor symbols\n");
  printf ("\n");
  printf ("System identifications\n");

#ifdef __sun__
  PRINT ("__sun__", __sun__);
#endif
#ifdef __sun
  PRINT ("__sun", __sun);
#endif
#ifdef sun
  PRINT ("sun", sun);
#endif

#ifdef __sparc__
  PRINT ("__sparc__", __sparc__);
#endif
#ifdef __sparc
  PRINT ("__sparc", __sparc);
#endif
#ifdef sparc
  PRINT ("sparc", sparc);
#endif

#ifdef __ultrix__
  PRINT ("__ultrix__", __ultrix__);
#endif
#ifdef __ultrix
  PRINT ("__ultrix", __ultrix);
#endif
#ifdef ultrix
  PRINT ("ultrix", ultrix);
#endif

#ifdef __mips__
  PRINT ("__mips__", __mips__);
#endif
#ifdef __mips
  PRINT ("__mips", __mips);
#endif
#ifdef mips
  PRINT ("mips", mips);
#endif

#ifdef __MIPSEL__
  PRINT ("__MIPSEL__", __MIPSEL__);
#endif
#ifdef __MIPSEL
  PRINT ("__MIPSEL", __MIPSEL);
#endif
#ifdef MIPSEL
  PRINT ("MIPSEL", MIPSEL);
#endif

#ifdef __sgi__
  PRINT ("__sgi__", __sgi__);
#endif
#ifdef __sgi
  PRINT ("__sgi", __sgi);
#endif
#ifdef sgi
  PRINT ("sgi", sgi);
#endif

#ifdef __MIPSEB__
  PRINT ("__MIPSEB__", __MIPSEB__);
#endif
#ifdef __MIPSEB
  PRINT ("__MIPSEB", __MIPSEB);
#endif
#ifdef MIPSEB
  PRINT ("MIPSEB", MIPSEB);
#endif

#ifdef hpux
  PRINT ("hpux", hpux);
#endif

#ifdef __WINNT__
  PRINT ("__WINNT__", __WINNT__);
#endif
#ifdef _WIN32
  PRINT ("_WIN32", _WIN32);
#endif
#ifdef __CYGWIN__
  PRINT ("__CYGWIN__", __CYGWIN__);
#endif
#ifdef __CYGWIN32__
  PRINT ("__CYGWIN32__", __CYGWIN32__);
#endif

#ifdef __MSDOS__
  PRINT ("__MSDOS__", __MSDOS__);
#endif
#ifdef __MSDOS
  PRINT ("__MSDOS", _MSDOS);
#endif
#ifdef MSDOS
  PRINT ("MSDOS", MSDOS);
#endif

#ifdef __unix__
  PRINT ("__unix__", __unix__);
#endif
#ifdef __unix
  PRINT ("__unix", __unix);
#endif
#ifdef unix
  PRINT ("unix", unix);
#endif

#ifdef __svr4__
  PRINT ("__svr4__", __svr4__);
#endif
#ifdef __SVR4
  PRINT ("__SVR4", __SVR4);
#endif

  printf ("\n");
  printf ("Compiler identifications:\n");

#ifdef __STDC__
  PRINT ("__STDC__", __STDC__);
#endif

#ifdef __GNUC__
  PRINT ("__GNUC__", __GNUC__);
#endif
#ifdef __GNUCC__
  PRINT ("__GNUCC__", __GNUCC__);
#endif

#ifdef __cplusplus
  PRINT ("__cplusplus", __cplusplus);
#endif

  return 0;
}
