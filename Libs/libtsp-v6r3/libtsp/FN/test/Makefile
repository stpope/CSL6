#
# Makefile libtsp/FN/test
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

TEST_SCRIPT = ./tFNroutines
LOG_FILE = tFN.log
REF_FILE = tFN.ref
FILTER = 

.PHONY: all test mostlyclean clean

BINS = \
	$(BINDIR)/tFNgcd \
	$(BINDIR)/tFNiLog2 \
	$(BINDIR)/tFNxxx \
	#

all: $(BINS)
$(BINDIR)/tFNgcd: tFNgcd.c $(LIB)
	@-test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $< $(LIB) $(LDFLAGS) -o $@

$(BINDIR)/tFNiLog2: tFNiLog2.c $(LIB)
	@-test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $< $(LIB) $(LDFLAGS) -o $@

$(BINDIR)/tFNxxx: tFNxxx.c $(LIB)
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
