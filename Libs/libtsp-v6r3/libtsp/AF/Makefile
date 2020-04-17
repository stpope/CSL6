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
	$(LIB)(AFclose.o) \
	$(LIB)(AFdReadData.o) \
	$(LIB)(AFdWriteData.o) \
	$(LIB)(AFfReadData.o) \
	$(LIB)(AFfWriteData.o) \
	$(LIB)(AFopnRead.o) \
	$(LIB)(AFopnWrite.o) \
	$(LIB)(AFsetFileType.o) \
	$(LIB)(AFsetInfo.o) \
	$(LIB)(AFsetNHpar.o) \
	$(LIB)(AFsetSpeaker.o) \
	$(LIB)(AFopenRead.o) \
	$(LIB)(AFopenWrite.o) \
	$(LIB)(AFreadData.o) \
	$(LIB)(AFwriteData.o) \
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
