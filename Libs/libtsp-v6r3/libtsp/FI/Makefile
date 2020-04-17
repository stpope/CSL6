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
	$(LIB)(FIdBiquad.o) \
	$(LIB)(FIdConvSI.o) \
	$(LIB)(FIdConvol.o) \
	$(LIB)(FIdFiltAP.o) \
	$(LIB)(FIdFiltIIR.o) \
	$(LIB)(FIdKaiserLPF.o) \
	$(LIB)(FIdReadFilt.o) \
	$(LIB)(FIdWinHamm.o) \
	$(LIB)(FIdWinKaiser.o) \
	$(LIB)(FIfBiquad.o) \
	$(LIB)(FIfConvSI.o) \
	$(LIB)(FIfConvol.o) \
	$(LIB)(FIfDeem.o) \
	$(LIB)(FIfFiltAP.o) \
	$(LIB)(FIfFiltIIR.o) \
	$(LIB)(FIfKaiserLPF.o) \
	$(LIB)(FIfPreem.o) \
	$(LIB)(FIfReadFilt.o) \
	$(LIB)(FIfWinHCos.o) \
	$(LIB)(FIfWinHamm.o) \
	$(LIB)(FIfWinKaiser.o) \
	$(LIB)(FIfWinRCos.o) \
	$(LIB)(FIgdelFIR.o) \
	$(LIB)(FIgdelIIR.o) \
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
