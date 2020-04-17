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
	$(LIB)(VRdAdd.o) \
	$(LIB)(VRdCopy.o) \
	$(LIB)(VRdCorSym.o) \
	$(LIB)(VRdDotProd.o) \
	$(LIB)(VRdLinInc.o) \
	$(LIB)(VRdMult.o) \
	$(LIB)(VRdPrint.o) \
	$(LIB)(VRdRFFTMSq.o) \
	$(LIB)(VRdScale.o) \
	$(LIB)(VRdSet.o) \
	$(LIB)(VRdShift.o) \
	$(LIB)(VRdSymPart.o) \
	$(LIB)(VRdZero.o) \
	$(LIB)(VRfAdd.o) \
	$(LIB)(VRfAmpldB.o) \
	$(LIB)(VRfCopy.o) \
	$(LIB)(VRfCorSym.o) \
	$(LIB)(VRfDiff.o) \
	$(LIB)(VRfDiffSq.o) \
	$(LIB)(VRfDotProd.o) \
	$(LIB)(VRfLog10.o) \
	$(LIB)(VRfMax.o) \
	$(LIB)(VRfMin.o) \
	$(LIB)(VRfMult.o) \
	$(LIB)(VRfPow10.o) \
	$(LIB)(VRfPrint.o) \
	$(LIB)(VRfRev.o) \
	$(LIB)(VRfScale.o) \
	$(LIB)(VRfSet.o) \
	$(LIB)(VRfShift.o) \
	$(LIB)(VRfSum.o) \
	$(LIB)(VRfZero.o) \
	$(LIB)(VRiPrint.o) \
	$(LIB)(VRiZero.o) \
	$(LIB)(VRlPrint.o) \
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
