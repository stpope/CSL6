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
	$(LIB)(FLdReadTF.o) \
	$(LIB)(FLexist.o) \
	$(LIB)(FLfReadTF.o) \
	$(LIB)(FLfileSize.o) \
	$(LIB)(FLjoinNames.o) \
	$(LIB)(FLseekable.o) \
	$(LIB)(FLterm.o) \
	#
	ranlib $(LIB)

mostlyclean:
	rm -f *.o
clean:	mostlyclean
