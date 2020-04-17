#
# Makefile for libtsp
#   make [COPTS=-O]
#
#   make test
#
#   make mostlyclean   - remove object files, test logs
#   make clean         - remove object files, test logs, test binaries, library
#
# $Id: Makefile 1.9 2003/05/21 libtsp-v6r3 $

SHELL=/bin/sh

INSTALL = install -c
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL) -m 664
INSTALL_DIR = $(INSTALL) -d

prefix=/usr/local
exec_prefix=$(prefix)
bindir=$(exec_prefix)/bin
libdir=$(exec_prefix)/lib
includedir=$(prefix)/include
mandir=$(prefix)/man

.PHONY:	all libtsp test install intstall_bin install_data mostlyclean clean
all:	libtsp

libtsp:
	-@test -d lib || mkdir lib
	cd libtsp; $(MAKE)

test:
	cd libtsp; $(MAKE) test

# Install
install_bin: all
	@-test -d $(libdir) ||	$(INSTALL_DIR) $(libdir)
	$(INSTALL_DATA) lib/* $(libdir)

install: install_bin
	@-test -d $(includedir) || $(INSTALL_DIR) $(includedir)
	$(INSTALL_DATA) include/*.h $(includedir)
	@-test -d $(includedir)/libtsp || $(INSTALL_DIR) $(includedir)/libtsp
	$(INSTALL_DATA) include/libtsp/* $(includedir)/libtsp
	@-test -d $(mandir)/man3 || $(INSTALL_DIR) $(mandir)/man3
	$(INSTALL_DATA) man/??/* $(mandir)/man3

# Install extras
install_data:
	@-test -d $(datadir)/libtsp/html || \
		$(INSTALL_DIR) $(datadir)/libtsp/html
	@-test -d $(datadir)/libtsp/html/AF || \
		$(INSTALL_DIR) $(datadir)/libtsp/html/AF
	$(INSTALL_DATA) html/AF/* $(datadir)/libtsp/html/AF
	@-test -d $(datadir)/libtsp/html/FI || \
		$(INSTALL_DIR) $(datadir)/libtsp/html/FI
	$(INSTALL_DATA) html/FI/* $(datadir)/libtsp/html/FI
	@-test -d $(datadir)/libtsp/html/FL || \
		$(INSTALL_DIR) $(datadir)/libtsp/html/FL
	$(INSTALL_DATA) html/FL/* $(datadir)/libtsp/html/FL
	@-test -d $(datadir)/libtsp/html/FN || \
		$(INSTALL_DIR) $(datadir)/libtsp/html/FN
	$(INSTALL_DATA) html/FN/* $(datadir)/libtsp/html/FN
	@-test -d $(datadir)/libtsp/html/MA || \
		$(INSTALL_DIR) $(datadir)/libtsp/html/MA
	$(INSTALL_DATA) html/MA/* $(datadir)/libtsp/html/MA
	@-test -d $(datadir)/libtsp/html/MS || \
		$(INSTALL_DIR) $(datadir)/libtsp/html/MS
	$(INSTALL_DATA) html/MS/* $(datadir)/libtsp/html/MS
	@-test -d $(datadir)/libtsp/html/SP || \
		$(INSTALL_DIR) $(datadir)/libtsp/html/SP
	$(INSTALL_DATA) html/SP/* $(datadir)/libtsp/html/SP
	@-test -d $(datadir)/libtsp/html/ST || \
		$(INSTALL_DIR) $(datadir)/libtsp/html/ST
	$(INSTALL_DATA) html/ST/* $(datadir)/libtsp/html/ST
	@-test -d $(datadir)/libtsp/html/UT || \
		$(INSTALL_DIR) $(datadir)/libtsp/html/UT
	$(INSTALL_DATA) html/UT/* $(datadir)/libtsp/html/UT
	@-test -d $(datadir)/libtsp/html/VR || \
		$(INSTALL_DIR) $(datadir)/libtsp/html/VR
	$(INSTALL_DATA) html/VR/* $(datadir)/libtsp/html/VR

# Clean-up
mostlyclean:
	cd libtsp; $(MAKE) mostlyclean
clean:
	cd libtsp; $(MAKE) clean
	cd lib; rm -f *
