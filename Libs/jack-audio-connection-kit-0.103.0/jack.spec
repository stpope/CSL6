# $Id: jack.spec.in 976 2006-06-18 15:30:29Z rncbc $
# set to 0 to build rpms without capabilities support
%define enable_capabilities 0
# strip binaries 
%define strip_jackd 1
# set to 1 to enable alternate jack temporary
# directory mounted as tmpfs
%define enable_tmpdir 1
%if %{enable_tmpdir}
%define jack_tmpdir /dev/shm
%endif
# use oss
%define oss 1
# use portaudio
%define portaudio 0
# use freebob
%define freebob 1
# use preemption check
%define preemption_check 0


Summary:	The Jack Audio Connection Kit
Name:		jack-audio-connection-kit
Version:	0.103.0
Release:	4
License:	GPL
Group:		System Environment/Daemons
Source0:	%{name}-%{version}.tar.gz
URL:		http://jackaudio.org
BuildRoot:	%{_tmppath}/%{name}-%{version}-root-%(id -u -n)

BuildRequires: automake >= 1.6 libsndfile-devel >= 1.0.0
BuildRequires: pkgconfig
BuildRequires: doxygen

%if %{portaudio}
BuildRequires: portaudio >= 18.1
%endif
%if %{freebob}
BuildRequires: libfreebob >= 1.0.0
%endif


%description
JACK is a low-latency audio server, written primarily for the Linux
operating system. It can connect a number of different applications to
an audio device, as well as allowing them to share audio between
themselves. Its clients can run in their own processes (ie. as a
normal application), or can they can run within a JACK server (ie. a
"plugin").

JACK is different from other audio server efforts in that it has been
designed from the ground up to be suitable for professional audio
work. This means that it focuses on two key areas: synchronous
execution of all clients, and low latency operation.

%package devel
Summary:	Header files for Jack 
Group:		Development/Libraries
Requires:	%{name} = %{version}

%description devel
Header files for the Jack Audio Connection Kit.

%package example-clients
Summary:	Example clients that use Jack 
Group:		Applications/Multimedia
Requires:	%{name} = %{version}

%description example-clients
Small example clients that use the Jack Audio Connection Kit.

%prep

%setup -q

%build
%configure \
%if %{enable_capabilities}
	--enable-capabilities \
%if %{strip_jackd}
	--enable-stripped-jackd \
%endif
%endif
%if %{enable_tmpdir}
	--with-default-tmpdir=%{jack_tmpdir} \
%endif
%if ! %{oss}
	--disable-oss \
%endif
%if ! %{portaudio}
	--disable-portaudio \
%endif
%if ! %{freebob}
	--disable-freebob \
%endif
%if %{preemption_check}
	--enable-preemption-check \
%endif
	--enable-optimize
make


%install
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT

%if %{enable_capabilities}
# make jackstart suid root
chmod 04755 $RPM_BUILD_ROOT%{_bindir}/jackstart
%endif

# remove extra install of the documentation
rm -rf $RPM_BUILD_ROOT%{_datadir}/%{name}/*

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%clean
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT

%files 
%defattr(-,root,root)
%doc AUTHORS TODO COPYING*
%if %{enable_capabilities}
%attr(4755, root, root) %{_bindir}/jackstart
%endif
%{_bindir}/jackd
%{_bindir}/jack_load
%{_bindir}/jack_unload
%{_bindir}/jack_bufsize
%{_bindir}/jack_freewheel
%{_bindir}/jack_transport
%{_libdir}/libjack.so
%{_libdir}/libjack.so.0
%{_libdir}/libjack.so.0.0.*
%{_libdir}/jack/inprocess.so
%{_libdir}/jack/intime.so
%{_libdir}/jack/jack_*.so
%{_mandir}/man1/*

%files devel
%defattr(-,root,root)
%doc doc/reference
%{_libdir}/libjack.la
%{_libdir}/jack/inprocess.la
%{_libdir}/jack/intime.la
%{_libdir}/jack/jack_*.la
%{_includedir}/jack/jack.h
%{_includedir}/jack/ringbuffer.h
%{_includedir}/jack/timestamps.h
%{_includedir}/jack/transport.h
%{_includedir}/jack/types.h
%{_includedir}/jack/thread.h
%{_includedir}/jack/intclient.h
%{_includedir}/jack/statistics.h
%{_includedir}/jack/midiport.h
%{_libdir}/pkgconfig/jack.pc

%files example-clients
%defattr(-,root,root)
%{_bindir}/jackrec
%{_bindir}/jack_connect
%{_bindir}/jack_disconnect
%{_bindir}/jack_impulse_grabber
%{_bindir}/jack_lsp
%{_bindir}/jack_metro
%{_bindir}/jack_monitor_client
%{_bindir}/jack_showtime
%{_bindir}/jack_simple_client
%{_bindir}/jack_midiseq
%{_bindir}/jack_midisine


%changelog

* Sun Jun 18 2006 Rui Nuno Capela <rncbc@rncbc.org> - 0.102.15-4
- enable freebob backend drivers whenever available 

* Sun May  7 2006 Rui Nuno Capela <rncbc@rncbc.org> - 0.102.1-3
- new JACK MIDI files on devel and example-clients
- set default tmpdir to /dev/shm

* Thu Oct 30 2005 Rui Nuno Capela <rncbc@rncbc.org> - 0.100.7-2
- omitted enable-capabilities, stripped-binaries and preemption
  check from default.

* Thu Jan  3 2005 Rui Nuno Capela <rncbc@rncbc.org> - 0.99.44-1
- add jack/statistics.h to devel package

* Thu Dec 28 2004 Rui Nuno Capela <rncbc@rncbc.org> - 0.99.41-1
- merge integration of usx2y (rawusb) driver into alsa backend
- enable preemption check feature of 2.6 RT kernels
- add jack/intclient.h to devel package
- add jack/thread.h to devel package
- moved doc/reference to devel package
- set default tmpdir to /var/lib/jack/tmp
- include oss backend driver if available

* Sat May 22 2004 Pete Bessman <ninjadroid@gazuga.net> - 0.98.1-1
- changes to accomodate jack_oss and RPM's fascist build policy

* Mon Nov 13  2003 Lawrie Abbott <lawrieabbott@iinet.net.au>      
- update based on Planet CCRMA 0.80.0 release

* Thu May 23 2002 Fernando Lopez-Lezcano <nando@ccrma.stanford.edu>
- added configuration variable to build with/without capabilities

* Tue May 21 2002 Fernando Lopez-Lezcano <nando@ccrma.stanford.edu>
- split the examples into a different package so that the base
  package does not depend on, for example, fltk. 
- disable stripping of binaries

* Mon May 13 2002 Fernando Lopez-Lezcano <nando@ccrma.stanford.edu>
- do not compress documentation, added doxygen docs directory
- changed defattr directives
- added libdir/jack*, libdir/*.a and libdir/*.so.* to files
- moved all so's to libs, jack will not start without jack_alsa.so
- merged base and libs packages

* Sat May  4 2002 Christian Fredrik Kalager Schaller <uraeus@linuxrising.org> 
- initial release of jack sound server package for GStreamer