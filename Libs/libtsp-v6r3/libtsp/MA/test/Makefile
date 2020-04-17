#
# Makefile libtsp/MA/test
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

TEST_SCRIPT = ./tMAroutines
LOG_FILE = tMA.log
REF_FILE = tMA.ref
FILTER = 

.PHONY: all test mostlyclean clean

BINS = \
	$(BINDIR)/tMAfChSolve \
	$(BINDIR)/tMAfPrint \
	$(BINDIR)/tMAfTpSolve \
	$(BINDIR)/tMAfxxQuad \
	#

all: $(BINS)
$(BINDIR)/tMAfChSolve: tMAfChSolve.c $(LIB)
	@-test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $< $(LIB) $(LDFLAGS) -o $@

$(BINDIR)/tMAfPrint: tMAfPrint.c $(LIB)
	@-test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $< $(LIB) $(LDFLAGS) -o $@

$(BINDIR)/tMAfTpSolve: tMAfTpSolve.c $(LIB)
	@-test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $< $(LIB) $(LDFLAGS) -o $@

$(BINDIR)/tMAfxxQuad: tMAfxxQuad.c $(LIB)
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
