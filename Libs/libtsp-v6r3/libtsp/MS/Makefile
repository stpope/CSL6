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
	$(LIB)(MSdConvCof.o) \
	$(LIB)(MSdDeconvCof.o) \
	$(LIB)(MSdIntLin.o) \
	$(LIB)(MSdIntMC.o) \
	$(LIB)(MSdLocate.o) \
	$(LIB)(MSdNint.o) \
	$(LIB)(MSdPolyInt.o) \
	$(LIB)(MSfConvCof.o) \
	$(LIB)(MSfGaussRand.o) \
	$(LIB)(MSfIntLin.o) \
	$(LIB)(MSfIntMC.o) \
	$(LIB)(MSfUnifRand.o) \
	$(LIB)(MSiCeil.o) \
	$(LIB)(MSiFloor.o) \
	$(LIB)(MSiPower.o) \
	$(LIB)(MSlCeil.o) \
	$(LIB)(MSlFloor.o) \
	$(LIB)(MSratio.o) \
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
