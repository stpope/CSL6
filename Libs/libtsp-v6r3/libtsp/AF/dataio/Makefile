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
	$(LIB)(AFdRdAlaw.o) \
	$(LIB)(AFdRdFx.o) \
	$(LIB)(AFdRdIx.o) \
	$(LIB)(AFdRdMulaw.o) \
	$(LIB)(AFdRdTA.o) \
	$(LIB)(AFdRdU1.o) \
	$(LIB)(AFdWrAlaw.o) \
	$(LIB)(AFdWrFx.o) \
	$(LIB)(AFdWrIx.o) \
	$(LIB)(AFdWrMulaw.o) \
	$(LIB)(AFdWrTA.o) \
	$(LIB)(AFdWrU1.o) \
	$(LIB)(AFfRdAlaw.o) \
	$(LIB)(AFfRdFx.o) \
	$(LIB)(AFfRdIx.o) \
	$(LIB)(AFfRdMulaw.o) \
	$(LIB)(AFfRdTA.o) \
	$(LIB)(AFfRdU1.o) \
	$(LIB)(AFfWrAlaw.o) \
	$(LIB)(AFfWrFx.o) \
	$(LIB)(AFfWrIx.o) \
	$(LIB)(AFfWrMulaw.o) \
	$(LIB)(AFfWrTA.o) \
	$(LIB)(AFfWrU1.o) \
	$(LIB)(AFgetLine.o) \
	$(LIB)(AFposition.o) \
	$(LIB)(AFseek.o) \
	$(LIB)(AFtell.o) \
	#
	ranlib $(LIB)

mostlyclean:
	rm -f *.o
clean:	mostlyclean
