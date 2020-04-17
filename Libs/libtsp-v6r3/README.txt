June 2006

libtsp library routines

The libtsp package is a library of routines for signal processing.  It also
includes a number of general purpose routines useful for program development.
Programs using this library for filtering, LPC analysis/synthesis and
resampling are available as part of the AFsp package.

The libtsp library routines are divided into a number of categories, with the
first two letters of the name keyed to the category.

AF - Audio file routines
  - Open and close audio files
  - Read and write audio files
FI - Filtering routines
  - Filter a signal (FIR and IIR filters)
  - Generate windows (Hamming, Kaiser and raised-cosine)
  - Design Kaiser-windowed lowpass filters
  - Read filter coefficient files
FL - File routines
  - Fill in the full path name for a file name
  - Return the creation date for a file
  - Prompt and read a line of text
  - Read and write numeric data
FN - Functions
  - Bessel function (0'th order modified)
  - Greatest common divisor
  - Series expansion of Chebyshev polynomials
  - Log base 2
MA - Matrix routines
  - Allocate space for a matrix
  - Print a matrix
  - Solve equations (Cholesky decomposition and Toeplitz matrix)
MS - Miscellaneous routines
  - Convolve coefficients of two arrays
  - Nearest integer, floor and ceiling functions
  - Interpolate a table of values (linear and cubic)
  - Rational approximation to a value
SP - Signal processing routines
  - Fast Fourier transform (complex and real)
  - Discrete cosine transform
  - Calculate covariance or autocorrelation from a block of data
  - Convert between linear prediction parameters (error filter coefficients,
    predictor coefficients, cepstral coefficients, line spectral frequencies,
    reflection coefficients, autocorrelation coefficients)
  - Solve for linear prediction coefficients (covariance and autocorrelation
    methods)
  - Calculate the mean-square filtering error
  - Distance measures (Itakura-Saito and RMS log LPC)
  - Quantize a value given a table of boundaries (binary search)
ST - String routines
  - Copy and concatenate strings
  - Decode numeric values from a string
  - Match a string to a table of keywords
UT - Utility routines
  - Decode command line options
  - Error message routines
VR - Vector routines
  - Print a vector of values
  - Vector functions (zero an array, convert to dB, dot product, minimum,
    maximum, scale, add, subtract, multiply)

The libtsp routines are written in C and have been tested on Unix and Windows
machines.

The routines are covered by copyright, see the file "Copying" for details of
the distribution conditions..

libtsp-v6r1.tar.gz
http://www.TSP.ECE.McGill.CA/MMSP/Documents/
or ftp://ftp.TSP.ECE.McGill.CA/TSP/libtsp

=============
Changes in v6r3
 - Use identity removed from audio file information
Changes in v6r2
 - More double precision versions
Changes in v6r1
 - Internal changes, AFsetInfo replaces AFsetHinfo 
Changes in v6r0a
 - Updated audio read/write routines
Changes in v5r6
 - Support for AIFF and WAVE_NOEX audio files
Changes in v5r5
 - Several routines now have float / double variants
Changes in v5r4
 - Plug a memory leak in audio open routines (Thanks to RS @ FhG)
 - More type casts for fussy compilers
Changes in v5r2
- updated audio header routines
Changes in v4r0
- several routines now have float and double versions
Changes in v3r1
- support for new data types for audio files, new FI routines, MS Visual C
  project
Changes in v3r0
- support for stdin/stdout for audio files
- more flexible error recovery for audio files
- support for header information fields
- minor fixes
Changes in V2R8
- support for Comdisco SPW Signal files
- support for 64-bit float data in audio files
Changes in V2R7a
- support for 8-bit data in audio files
Changes in V2R6
- support for text audio files
- Kaiser window routines
- more string decoding routines
Changes in V2R5
- HTML format documentation
Changes in V2R4b
- new audio file formats supported
- fix to UTgetHost for Solaris
- fix to AFwriteHead
Changes in V2R3
- changes for MS-DOS
Changes in V2R2
- changes for Sun CC compiler
Changes in V2R1
- first net release

=============
Peter Kabal
Dept. Electrical & Computer Engineering    McGill University
+1 514 398-7130   +1 514 398-4470 Fax
kabal@ECE.McGill.CA

$Id: README.txt 1.37 2006/06/06 libtsp-v6r3 $
