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
	$(LIB)(UTbyteCode.o) \
	$(LIB)(UTbyteOrder.o) \
	$(LIB)(UTcheckIEEE.o) \
	$(LIB)(UTctime.o) \
	$(LIB)(UTdecOption.o) \
	$(LIB)(UTdIEEE80.o) \
	$(LIB)(UTeIEEE80.o) \
	$(LIB)(UTgetHost.o) \
	$(LIB)(UTgetUser.o) \
	$(LIB)(UTswapCode.o) \
	#
	ranlib $(LIB)

mostlyclean:
	rm -f *.o
clean:	mostlyclean
