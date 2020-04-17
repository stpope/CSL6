#
# Makefile libtsp/FI/test
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

TEST_SCRIPT = ./tFIroutines
LOG_FILE = tFI.log
REF_FILE = tFI.ref
FILTER = |./filter_log

.PHONY: all test mostlyclean clean

BINS = \
	$(BINDIR)/tFIgdelFIR \
	$(BINDIR)/tFIgdelIIR \
	$(BINDIR)/tFIxConvol \
	$(BINDIR)/tFIxKaiserLPF \
	$(BINDIR)/tFIxReadFilt \
	$(BINDIR)/tFIxWinXXX \
	#

all: $(BINS)
$(BINDIR)/tFIgdelFIR: tFIgdelFIR.c $(LIB)
	@-test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $< $(LIB) $(LDFLAGS) -o $@

$(BINDIR)/tFIgdelIIR: tFIgdelIIR.c $(LIB)
	@-test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $< $(LIB) $(LDFLAGS) -o $@

$(BINDIR)/tFIxConvol: tFIxConvol.c $(LIB)
	@-test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $< $(LIB) $(LDFLAGS) -o $@

$(BINDIR)/tFIxKaiserLPF: tFIxKaiserLPF.c $(LIB)
	@-test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $< $(LIB) $(LDFLAGS) -o $@

$(BINDIR)/tFIxReadFilt: tFIxReadFilt.c $(LIB)
	@-test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $< $(LIB) $(LDFLAGS) -o $@

$(BINDIR)/tFIxWinXXX: tFIxWinXXX.c $(LIB)
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
