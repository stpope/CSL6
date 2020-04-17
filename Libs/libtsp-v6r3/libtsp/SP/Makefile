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
	$(LIB)(SPautoc.o) \
	$(LIB)(SPcepXpc.o) \
	$(LIB)(SPcorBWexp.o) \
	$(LIB)(SPcorFilt.o) \
	$(LIB)(SPcorFmse.o) \
	$(LIB)(SPcorPmse.o) \
	$(LIB)(SPcorXpc.o) \
	$(LIB)(SPcovCXpc.o) \
	$(LIB)(SPcovFilt.o) \
	$(LIB)(SPcovFmse.o) \
	$(LIB)(SPcovLXpc.o) \
	$(LIB)(SPcovMXpc.o) \
	$(LIB)(SPcovPmse.o) \
	$(LIB)(SPcovXpc.o) \
	$(LIB)(SPcovar.o) \
	$(LIB)(SPdCFFT.o) \
	$(LIB)(SPdQuantL.o) \
	$(LIB)(SPdRFFT.o) \
	$(LIB)(SPecXpc.o) \
	$(LIB)(SPfCFFT.o) \
	$(LIB)(SPfDCT.o) \
	$(LIB)(SPfQuantL.o) \
	$(LIB)(SPfQuantU.o) \
	$(LIB)(SPfRFFT.o) \
	$(LIB)(SPlpcISdist.o) \
	$(LIB)(SPlpcLSdist.o) \
	$(LIB)(SPlsfXpc.o) \
	$(LIB)(SPpcBWexp.o) \
	$(LIB)(SPpcXcep.o) \
	$(LIB)(SPpcXcor.o) \
	$(LIB)(SPpcXec.o) \
	$(LIB)(SPpcXlsf.o) \
	$(LIB)(SPpcXrc.o) \
	$(LIB)(SPrcXpc.o) \
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
