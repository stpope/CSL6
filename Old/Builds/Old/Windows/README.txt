This is old -- see ../CSL_JUCE for builds using introjucer

CSL 5.0g README file for MS-Windows

A VisualStudio project is included that is known to work with Windows XP.
It assumes the JUCE (version 1.50 with patched sound file IO) and DirectX9 SDK are installed in C:\Program Files\...

stp - 0910

#############################

CSL 4.0 README file for windows.

Note: CSL4 is not yet fully ported to Windows; there is a new VisualStudio project included, but it fails to compile based on missing includes and libraries (pthreads, stdlib -- these should be rewritten to use JUCE threads) and some C usage that GCC accepts and MSVC++ doesn't (e.g. declarations of the form float f_array[variable-size]). We include the partially working VS project, as well as Ryan Avery's project files for CSL 4.2 (pre-JUCE), in the hope that someone will get this running.

#############################

Appended by Ryan Avery (ravery@umail.ucsb.edu) 10/19/2005:

Newest project file is for VisualCNet. Use of other project files may require manually adding project folders:
- IO, Instruments, Processors, Sources, Kernel, Utilities, Spatializers

In your project settings, you must have WIN32 in your preprocessor definitions (should be there by default in MS Visual compilers, but it would be good to check to make sure).

The instructions below for building in MSVC++ will work, but I recommend a different strategy to manage libraries, includes, and DLL files:

1. Put all of your .LIB files in a single folder on your hard drive, and reference that in your "Show Directories For: Library Files" list under Tools->Options.

2. Put all of your .H files in the same directory, again adding that to the "Show Directories For: Include Files" list.

3. Put all of your .DLL files in the C:\WINDOWS\SYSTEM or \SYSTEM32 directory

(Make sure you read the directions below - they are still helpful to get you up and running)


Building in MSVC++:
#############################

Firstly, you will need the latest libsndfile, Portaudio, pthreads, and fftw source as you will need to compile each package into a library and have the headers accessible to your MSVC++ Project.

It would be advisable to do the following to make the libraries accessible in MSVC++:
00) Put the .lib files and source code for these libraries under a new directory called "C:\_LIBRARIES".
01) In MSVC++ go to: Tools->Options.
02) Click on the Directories tab.
03a) For MSVC++ .NET only: Click "the Projects" folder and then "VC++ Directories" click "Library files".
03b) Under "Show Directories For:" click "Library files".
04) Add C:\LIBRARIES to the list.
05) Under "Show Directories For:" click "Include files".
06) Add the following three directories to the list (if your source directories have slightly different names that's fine):
		C:\_LIBRARIES\PORTAUDIO\PA_COMMON
		C:\_LIBRARIES\PTHREADS
		C:\_LIBRARIES\fftw-3.0.1\api
		C:\_LIBRARIES\LIBSNDFILE-1.0.4\SRC
		C:\_LIBRARIES\NEWRAN03

07) Make a copy of the pthreadsVC.dll and libsndfile.dll into the C:\_LIBRARIES directory.
08) Add the directory C:\_LIBRARIES to your PATH environment variable so that the dlls will load automatically. Otherwise, MSVC++ will give you an error at runtime.


The libraries should exist by default in your MSVC++ installation:
user32.lib wsock32.lib dsound.lib winmm.lib 
You will need to have these libraries in C:\LIBRARIES:
msvc_portaudio_ds.lib msvc_FFTW3.0.1_single.lib msvc_pthread.lib msvc_libsndfile.lib msvc_newran3.lib
