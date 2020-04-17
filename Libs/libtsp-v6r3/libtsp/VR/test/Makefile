#
# Makefile libtsp/VR/test
#

SHELL = /bin/sh

dist_dir = ../../..

dist_libdir = $(dist_dir)/lib
dist_includedir = $(dist_dir)/include

COPTS = -O
CFLAGS = $(COPTS) -I$(dist_includedir)
LDFLAGS = -lm

LIB = $(dist_libdir)/libtsp.a

BINDIR = bin

TEST_SCRIPT = ./tVRroutines
LOG_FILE = tVR.log
REF_FILE = tVR.ref
FILTER = 

.PHONY: all test mostlyclean clean

BINS = \
	$(BINDIR)/tVRswapBytes \
	$(BINDIR)/tVRx2y \
	$(BINDIR)/tVRxShift \
	#

all: $(BINS)
$(BINDIR)/tVRswapBytes: tVRswapBytes.c $(LIB)
	@-test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $< $(LIB) $(LDFLAGS) -o $@

$(BINDIR)/tVRx2y: tVRx2y.c $(LIB)
	@-test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $< $(LIB) $(LDFLAGS) -o $@

$(BINDIR)/tVRxShift: tVRxShift.c $(LIB)
	@-test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $< $(LIB) $(LDFLAGS) -o $@

test:	$(BINS)
	@echo "$(TEST_SCRIPT) $(FILTER) > $(LOG_FILE)"
	@PATH=$(BINDIR):$$PATH \
		$(TEST_SCRIPT) $(FILTER) > $(LOG_FILE)
	diff $(LOG_FILE) $(REF_FILE)

mostlyclean:
	rm -f *.o
	rm -f $(LOG_FILE)
clean:	mostlyclean
	rm -f $(BINDIR)/*
	rmdir $(BINDIR)
