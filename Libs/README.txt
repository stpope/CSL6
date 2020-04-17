These files are the pre-compiled libraries and include files for the support frameworks required by CSL.

To use this, copy the files in the folder "local_your_platform" to /usr/local" i.e.,
	sudo cp -rp ./local_include/* /usr/local/include
	sudo cp -rp ./local_lib_MacIntel/* /usr/local/lib

You may need to run ranlib on the libraries after this; try
	cd /usr/local/lib
	ranlib *.a

For MS-Windows, copy the binaries to your DLL folder.

For more info, see the CSL documentation.

stp -- August, 2004 - May, 2007 - April, 2009
