/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  sysOS.h

Description:
  System and OS definitions.
  SY_OS: Operating System
    SY_OS_UNIX     - Pure Unix
    SY_OS_WINDOWS  - Pure Windows
    SY_OS_CYGWIN   - Unix under windows
  SY_FILENAME_SPEC: Filename specification
    SY_FNS_UNIX    - Unix style filenames
    SY_FNS_WINDOWS - Windows style filenames
  SY_POSIX: Availability of Posix routines
    0 - Not available
    1 - Available

Author / revision:
  P. Kabal  Copyright (C) 2001
  $Revision: 1.10 $  $Date: 2001/10/04 13:28:49 $

----------------------------------------------------------------------*/

#ifndef sysOS_h_
#define sysOS_h_

/* Operating System
   - pure Unix
     - file names: "/usr/xxx/yyy"
     - text files same as binary files
     - Posix routines available ("unistd.h" for example)
   - pure Windows
     - file names: "D:\xxx\yyy"
     - text files differ from binary files
     - some Posix routines available in various places
   - hybrid: Unix running under Windows (Cygwin)
     - file names: "//d/xxx/yyy"
     - text files differ from binary files
     - Posix routines available ("unistd.h" for example)
*/
#define SY_OS_UNIX	0
#define SY_OS_WINDOWS	1
#define SY_OS_CYGWIN	2

#define SY_FNS_UNIX	0
#define SY_FNS_WINDOWS	1

#if (defined(__CYGWIN32__))
#  define SY_OS			SY_OS_CYGWIN
#  define SY_FILENAME_SPEC	SY_FNS_UNIX
#  define SY_POSIX		1
#elif (defined(_WIN32))
#  define SY_OS			SY_OS_WINDOWS
#  define SY_FILENAME_SPEC	SY_FNS_WINDOWS
#  define SY_POSIX		0
#else
#  define SY_OS			SY_OS_UNIX
#  define SY_FILENAME_SPEC	SY_FNS_UNIX
#  define SY_POSIX		1
#endif

#endif /* sysOS_h_ */
