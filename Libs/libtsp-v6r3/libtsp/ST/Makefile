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
	$(LIB)(STcatMax.o) \
	$(LIB)(STcopyMax.o) \
	$(LIB)(STcopyNMax.o) \
	$(LIB)(STdec1double.o) \
	$(LIB)(STdec1float.o) \
	$(LIB)(STdec1int.o) \
	$(LIB)(STdec1long.o) \
	$(LIB)(STdecDfrac.o) \
	$(LIB)(STdecIfrac.o) \
	$(LIB)(STdecIrange.o) \
	$(LIB)(STdecLrange.o) \
	$(LIB)(STdecNdouble.o) \
	$(LIB)(STdecNfloat.o) \
	$(LIB)(STdecNint.o) \
	$(LIB)(STdecNlong.o) \
	$(LIB)(STfindToken.o) \
	$(LIB)(STkeyMatch.o) \
	$(LIB)(STkeyXpar.o) \
	$(LIB)(STstrLC.o) \
	$(LIB)(STtrim.o) \
	$(LIB)(STunQuote.o) \
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
