#
# Makefile for libtsp/XX
#

SHELL = /bin/sh

dist_dir = ../..

dist_libdir = $(dist_dir)/lib
dist_includedir = $(dist_dir)/include

COPTS = -O
CFLAGS = $(COPTS) -I$(dist_includedir)

LIB = $(dist_libdir)/libtsp.a

.PHONY: all nucleus_routines test mostlyclean clean

all:	$(LIB) nucleus_routines

.PRECIOUS: $(LIB)
$(LIB): \
	$(LIB)(FLbackup.o) \
	$(LIB)(FLbaseName.o) \
	$(LIB)(FLdReadData.o) \
	$(LIB)(FLdWriteData.o) \
	$(LIB)(FLdate.o) \
	$(LIB)(FLdefName.o) \
	$(LIB)(FLdirName.o) \
	$(LIB)(FLexpHome.o) \
	$(LIB)(FLfReadData.o) \
	$(LIB)(FLfWriteData.o) \
	$(LIB)(FLextName.o) \
	$(LIB)(FLfReadData.o) \
	$(LIB)(FLfWriteData.o) \
	$(LIB)(FLfileDate.o) \
	$(LIB)(FLfullName.o) \
	$(LIB)(FLgetLine.o) \
	$(LIB)(FLgetRec.o) \
	$(LIB)(FLhomeDir.o) \
	$(LIB)(FLpathList.o) \
	$(LIB)(FLpreName.o) \
	$(LIB)(FLprompt.o) \
	$(LIB)(FLreadLine.o) \
	#
	ranlib $(LIB)

nucleus_routines:
	@(test -d nucleus && (cd nucleus; $(MAKE))) || true
	@(test -d dataio && (cd dataio; $(MAKE))) || true
	@(test -d header && (cd header; $(MAKE))) || true

test:
	@(test -d test && (cd test; $(MAKE) test)) || true

mostlyclean:
	rm -f *.o
	@(test -d nucleus && (cd nucleus; $(MAKE) mostlyclean)) || true
	@(test -d dataio && (cd dataio; $(MAKE) mostlyclean)) || true
	@(test -d header && (cd header; $(MAKE) mostlyclean)) || true
	@(test -d test && (cd test; $(MAKE) mostlyclean)) || true
clean:
	rm -f *.o
	@(test -d nucleus && (cd nucleus; $(MAKE) clean)) || true
	@(test -d dataio && (cd dataio; $(MAKE) clean)) || true
	@(test -d header && (cd header; $(MAKE) clean)) || true
	@(test -d test && (cd test; $(MAKE) clean)) || true
