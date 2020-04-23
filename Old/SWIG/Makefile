#
# Makefile for CSL SWIG Interfaces
#

.SUFFIXES: .cxx
CC = gcc
CXX = gcc
CFLAGS = -g -O2 -fno-common -dynamic

CSL_ROOT = /users/stp/Code/CSL/CSL

CPPFLAGS = -I$(CSL_ROOT)/Kernel -I$(CSL_ROOT)/Sources -I$(CSL_ROOT)/Utilities -I$(CSL_ROOT)/Processors -I$(CSL_ROOT)/IO \
	-I/usr/local/include -I/Users/stp/Code/Smalltalk/SWIG-Smalltalk/Lib/Smalltalk -I.

DLL_SRCS  = CSL_SWIG_wrap.cxx  # SirenCSL.cxx
DLL_OBJS  = $(DLL_SRCS:.cxx=.o)
DLL_OBJS := $(DLL_OBJS:.c=.o)
# DLL_OBJS := $(DLL_OBJS:.i=.c)

DLL_LDFLAGS = -dynamic -bundle -flat_namespace -bind_at_load \
	-bundle_loader /Users/stp/Code/Smalltalk/visual_Mac86.app/Contents/MacOS/visual \
	-undefined suppress -lgcc  -L/usr/local/lib -lCSL

DLL_NAME = CSL_SWIG_wrap.so

SWIG_FLAGS = -c -smalltalk -c++ -module csl

$(DLL_NAME): $(DLL_OBJS)
	gcc -bind_at_load -o $(DLL_NAME) $(DLL_OBJS) $(DLL_LDFLAGS) $(DLL_LDLIBS)

.cxx.o:
	$(CXX) -c $(DLL_CFLAGS) $(CPPFLAGS) $(CFLAGS) -o $@ $<

.c.o:
	$(CC) -c $(DLL_CFLAGS) $(CPPFLAGS) $(CFLAGS) -o $@ $<

swig:
	swig $(SWIG_FLAGS) $(CPPFLAGS) CSL_SWIG.i

clean:
	-rm -f $(OBJS) $(DLL_NAME) $(DLL_EXP_DEF) example_wrap.c SwigType*.ssi ExampleVariables.ssi ExampleVariablesNI.ssi

CSL_SWIG_wrap.cxx: CSL_SWIG.i

CSL_SWIG_wrap.o: CSL_SWIG_wrap.cxx


