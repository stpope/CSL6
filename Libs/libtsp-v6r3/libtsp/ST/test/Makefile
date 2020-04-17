#
# Makefile libtsp/ST/test
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

TEST_SCRIPT = ./tSTroutines
LOG_FILE = tST.log
REF_FILE = tST.ref
FILTER = 

.PHONY: all test mostlyclean clean

BINS = \
	$(BINDIR)/tsscanf \
	$(BINDIR)/tSTcatMax \
	$(BINDIR)/tSTcopyMax \
	$(BINDIR)/tSTcopyNMax \
	$(BINDIR)/tSTdecNxxx \
	$(BINDIR)/tSTdecPair \
	$(BINDIR)/tSTfindToken \
	$(BINDIR)/tSTkeyMatch \
	$(BINDIR)/tSTkeyXpar \
	$(BINDIR)/tSTstrDots \
	$(BINDIR)/tSTstrLC \
	$(BINDIR)/tSTstrstrNM \
	$(BINDIR)/tSTtrim \
	$(BINDIR)/tSTtrimNMax \
	$(BINDIR)/tSTunQuote \
	#

all: $(BINS)
$(BINDIR)/tsscanf: tsscanf.c $(LIB)
	@-test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $< $(LIB) $(LDFLAGS) -o $@

$(BINDIR)/tSTcatMax: tSTcatMax.c $(LIB)
	@-test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $< $(LIB) $(LDFLAGS) -o $@

$(BINDIR)/tSTcopyMax: tSTcopyMax.c $(LIB)
	@-test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $< $(LIB) $(LDFLAGS) -o $@

$(BINDIR)/tSTcopyNMax: tSTcopyNMax.c $(LIB)
	@-test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $< $(LIB) $(LDFLAGS) -o $@

$(BINDIR)/tSTdecNxxx: tSTdecNxxx.c $(LIB)
	@-test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $< $(LIB) $(LDFLAGS) -o $@

$(BINDIR)/tSTdecPair: tSTdecPair.c $(LIB)
	@-test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $< $(LIB) $(LDFLAGS) -o $@

$(BINDIR)/tSTfindToken: tSTfindToken.c $(LIB)
	@-test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $< $(LIB) $(LDFLAGS) -o $@

$(BINDIR)/tSTkeyMatch: tSTkeyMatch.c $(LIB)
	@-test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $< $(LIB) $(LDFLAGS) -o $@

$(BINDIR)/tSTkeyXpar: tSTkeyXpar.c $(LIB)
	@-test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $< $(LIB) $(LDFLAGS) -o $@

$(BINDIR)/tSTstrDots: tSTstrDots.c $(LIB)
	@-test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $< $(LIB) $(LDFLAGS) -o $@

$(BINDIR)/tSTstrLC: tSTstrLC.c $(LIB)
	@-test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $< $(LIB) $(LDFLAGS) -o $@

$(BINDIR)/tSTstrstrNM: tSTstrstrNM.c $(LIB)
	@-test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $< $(LIB) $(LDFLAGS) -o $@

$(BINDIR)/tSTtrim: tSTtrim.c $(LIB)
	@-test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $< $(LIB) $(LDFLAGS) -o $@

$(BINDIR)/tSTtrimNMax: tSTtrimNMax.c $(LIB)
	@-test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $< $(LIB) $(LDFLAGS) -o $@

$(BINDIR)/tSTunQuote: tSTunQuote.c $(LIB)
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
