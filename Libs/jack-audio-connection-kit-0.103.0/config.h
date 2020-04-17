/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Directory for plugins */
#define ADDON_DIR "/usr/local/lib/jack"

/* Enable debugging messages */
/* #undef DEBUG_ENABLED */

/* Default tmp directory */
#define DEFAULT_TMP_DIR "/tmp"

/* Enable buffer resizing */
/* #undef DO_BUFFER_RESIZE */

/* check realtime preemption */
/* #undef DO_PREEMPTION_CHECKING */

/* Ensure that memory locking succeeds */
/* #undef ENSURE_MLOCK */

/* "Is there Altivec Support ?" */
/* #undef HAVE_ALTIVEC_LINUX */

/* Define to 1 if you have the `atexit' function. */
#define HAVE_ATEXIT 1

/* "Whether or not clock_gettime can be found in system libraries" */
/* #undef HAVE_CLOCK_GETTIME */

/* Define to 1 if you have the <CoreAudio/CoreAudio.h> header file. */
#define HAVE_COREAUDIO_COREAUDIO_H 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <getopt.h> header file. */
#define HAVE_GETOPT_H 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the `dl' library (-ldl). */
/* #undef HAVE_LIBDL */

/* Define to 1 if you have the `inet' library (-linet). */
/* #undef HAVE_LIBINET */

/* Define to 1 if you have the `m' library (-lm). */
#define HAVE_LIBM 1

/* Define to 1 if you have the `nsl' library (-lnsl). */
/* #undef HAVE_LIBNSL */

/* Define to 1 if you have the `pthread' library (-lpthread). */
/* #undef HAVE_LIBPTHREAD */

/* Define to 1 if you have the `rt' library (-lrt). */
/* #undef HAVE_LIBRT */

/* Define to 1 if you have the `socket' library (-lsocket). */
/* #undef HAVE_LIBSOCKET */

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `on_exit' function. */
/* #undef HAVE_ON_EXIT */

/* Define to 1 if you have the <portaudio.h> header file. */
/* #undef HAVE_PORTAUDIO_H */

/* Define to 1 if you have the `posix_memalign' function. */
/* #undef HAVE_POSIX_MEMALIGN */

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* CPP has a broken varargs implementation */
#define JACK_CPP_VARARGS_BROKEN 1

/* Default JACK driver */
#define JACK_DEFAULT_DRIVER "coreaudio"

/* JACK shared memory type */
#define JACK_SHM_TYPE "POSIX"

/* Guaranteed size of realtime stack */
#define JACK_THREAD_STACK_TOUCH 10000

/* Use MACH threads where possible */
#define JACK_USE_MACH_THREADS 1

/* Include old transport interfaces */
#define OLD_TRANSPORT 

/* Name of package */
#define PACKAGE "jack-audio-connection-kit"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME ""

/* Define to the full name and version of this package. */
#define PACKAGE_STRING ""

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME ""

/* Define to the version of this package. */
#define PACKAGE_VERSION ""

/* "Are we running a ppc CPU?" */
/* #undef POWERPC */

/* Protocol version */
#define PROTOCOL_VERSION "16"

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define to 1 if Altivec assembly is available. */
/* #undef USE_ALTIVEC */

/* Use pthread barrier functions */
/* #undef USE_BARRIER */

/* Enable POSIX 1.e capabilities support */
/* #undef USE_CAPABILITIES */

/* Define to 1 to use dynamic SIMD selection. */
/* #undef USE_DYNSIMD */

/* Use POSIX memory locking */
/* #undef USE_MLOCK */

/* Define to 1 if MMX assembly is available. */
#define USE_MMX 1

/* Use POSIX shared memory interface */
#define USE_POSIX_SHM 1

/* Define to 1 if SSE assembly is available. */
#define USE_SSE 1

/* Version number of package */
#define VERSION "0.103.0"

/* Enable JACK timestamp API */
/* #undef WITH_TIMESTAMPS */

/* Define to 1 if your processor stores words with the most significant byte
   first (like Motorola and SPARC, unlike Intel and VAX). */
/* #undef WORDS_BIGENDIAN */

/* "Nope it's intel" */
#define x86 1
