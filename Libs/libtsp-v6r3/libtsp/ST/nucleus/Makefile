#
# Makefile libtsp/XX/YY
#

SHELL = /bin/sh

dist_dir = ../../..

dist_libdir = $(dist_dir)/lib
dist_includedir = $(dist_dir)/include

COPTS = -O
CFLAGS = $(COPTS) -I$(dist_includedir)

LIB = $(dist_libdir)/libtsp.a

.PHONY: all mostlyclean clean

all:	$(LIB)

.PRECIOUS: $(LIB)
$(LIB): \
	$(LIB)(STdec1val.o) \
	$(LIB)(STdecNval.o) \
	$(LIB)(STdecPair.o) \
	$(LIB)(STstrDots.o) \
	$(LIB)(STstrstrNM.o) \
	$(LIB)(STtrimIws.o) \
	$(LIB)(STtrimNMax.o) \
	#
	ranlib $(LIB)

mostlyclean:
	rm -f *.o
clean:	mostlyclean
