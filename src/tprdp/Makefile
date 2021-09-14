prefix      = /usr/local
exec_prefix = ${prefix}
bindir      = ${exec_prefix}/bin
mandir      = ${datarootdir}/man
datarootdir = ${prefix}/share
datadir     = ${datarootdir}

VERSION     = 1.0.0-beta
KEYMAP_PATH = $(datadir)/tprdp/keymaps/

CC          = gcc
INSTALL     = /usr/bin/install -c
CFLAGS      = -g -O2 -w -Wall -Wextra      -I/usr/include/p11-kit-1   -D_REENTRANT  -DPACKAGE_NAME=\"tprdp\" -DPACKAGE_TARNAME=\"tprdp\" -DPACKAGE_VERSION=\"1.0.0-beta\" -DPACKAGE_STRING=\"tprdp\ 1.0.0-beta\" -DPACKAGE_BUGREPORT=\"\" -DPACKAGE_URL=\"\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DL_ENDIAN=1 -DHAVE_SYS_SELECT_H=1 -DHAVE_LOCALE_H=1 -DHAVE_LANGINFO_H=1 -DHAVE_SYSEXITS_H=1 -DHAVE_XRANDR=1 -DHAVE_XCURSOR=1 -DEGD_SOCKET=\"/var/run/egd-pool\" -DWITH_RDPSND=1 -DRDPSND_OSS=1 -DRDPSND_ALSA=1 -DRDPSND_PULSE=1 -DHAVE_DIRENT_H=1 -DHAVE_DIRFD=1 -DHAVE_DECL_DIRFD=1 -DHAVE_ICONV_H=1 -DHAVE_ICONV=1 -DICONV_CONST= -DHAVE_SYS_VFS_H=1 -DHAVE_SYS_STATVFS_H=1 -DHAVE_SYS_STATFS_H=1 -DHAVE_SYS_PARAM_H=1 -DHAVE_SYS_MOUNT_H=1 -DSTAT_STATVFS=1 -DHAVE_STRUCT_STATVFS_F_NAMEMAX=1 -DHAVE_STRUCT_STATFS_F_NAMELEN=1 -DHAVE_MNTENT_H=1 -DHAVE_SETMNTENT=1 -DKEYMAP_PATH=\"$(KEYMAP_PATH)\"
LDFLAGS     =  -lgmp  -lXrandr -lXcursor -ltasn1 -lnettle -lhogweed -lgnutls  -lasound -lpulse   
STRIP       = strip

TARGETS     = tprdp
SOUNDOBJ    =  ./sound/rdpsnd.o ./sound/rdpsnd_dsp.o ./sound/rdpsnd_oss.o ./sound/rdpsnd_alsa.o ./sound/rdpsnd_pulse.o
SCARDOBJ    = 
CREDSSPOBJ  = 

RDPOBJ   = Protocol/tcp.o Protocol/asn.o Protocol/iso.o Protocol/mcs.o Protocol/secure.o Protocol/license.o rdp/rdp.o Protocol/orders.o Protocol/bitmap.o cache/cache.o rdp/rdp5.o redirects/channels.o rdp/rdpdr.o redirects/serial.o redirects/printer.o redirects/disk.o redirects/parallel.o cache/printercache.o Protocol/mppc.o Protocol/pstcache.o Protocol/lspci.o Protocol/seamless.o Protocol/ssl.o utilities/utils.o include/stream.o Protocol/dvc.o rdp/rdpedisp.o
X11OBJ   = tprdp.o utilities/xwin.o utilities/xkeymap.o Protocol/ewmhints.o utilities/xclip.o redirects/cliprdr.o Protocol/ctrl.o

.PHONY: all
all: $(TARGETS)

tprdp: $(X11OBJ) $(SOUNDOBJ) $(RDPOBJ) $(SCARDOBJ) $(CREDSSPOBJ)
	$(CC) $(CFLAGS) -o tprdp $(X11OBJ) $(SOUNDOBJ) $(RDPOBJ) $(SCARDOBJ) $(CREDSSPOBJ) $(LDFLAGS) -lX11

.PHONY: install
install: installbin installkeymaps installman

.PHONY: installbin
installbin: tprdp
	mkdir -p $(DESTDIR)$(bindir)
	$(INSTALL) tprdp $(DESTDIR)$(bindir)
	$(STRIP) $(DESTDIR)$(bindir)/tprdp
	chmod 755 $(DESTDIR)$(bindir)/tprdp

.PHONY: installman
installman: doc/tprdp.1
	mkdir -p $(DESTDIR)$(mandir)/man1
	cp doc/tprdp.1 $(DESTDIR)$(mandir)/man1
	chmod 644 $(DESTDIR)$(mandir)/man1/tprdp.1

.PHONY: installkeymaps
installkeymaps:
	mkdir -p $(DESTDIR)$(KEYMAP_PATH)
# Prevent copying the CVS directory
	cp keymaps/?? keymaps/??-?? keymaps/??-??-???? $(DESTDIR)$(KEYMAP_PATH)
	cp keymaps/common $(DESTDIR)$(KEYMAP_PATH)
	cp keymaps/modifiers $(DESTDIR)$(KEYMAP_PATH)
	chmod 644 $(DESTDIR)$(KEYMAP_PATH)/*

.PHONY: clean
clean:
	rm -f *.o *~ tprdp redirects/*.o cache/*.o utilities/*.o X11/*.o include/*.o Protocol/*.o rdp/*.o sound/*.o

.PHONY: distclean
distclean: clean
	rm -rf autom4te.cache config.log config.status Makefile tprdp-$(VERSION).tar.gz

.PHONY: dist
dist: tprdp-$(VERSION).tar.gz

tprdp-$(VERSION).tar.gz: Makefile configure
	mkdir -p /tmp/tprdp-make-dist-dir
	ln -sf `pwd` /tmp/tprdp-make-dist-dir/tprdp-$(VERSION) 
	cd /tmp/tprdp-make-dist-dir; \
	tar czvf tprdp-$(VERSION)/tprdp-$(VERSION).tar.gz \
	tprdp-$(VERSION)/configure \
	tprdp-$(VERSION)/configure.ac \
	tprdp-$(VERSION)/config.sub \
	tprdp-$(VERSION)/config.guess \
	tprdp-$(VERSION)/bootstrap \
	tprdp-$(VERSION)/install-sh \
	tprdp-$(VERSION)/Makefile.in \
	tprdp-$(VERSION)/*.c \
	tprdp-$(VERSION)/*.h \
	tprdp-$(VERSION)/cache/*.c \
	tprdp-$(VERSION)/include/*.c \
	tprdp-$(VERSION)/include/*.h \
	tprdp-$(VERSION)/Protocol/*.c \
	tprdp-$(VERSION)/Protocol/*.h \
	tprdp-$(VERSION)/rdp/*.c \
	tprdp-$(VERSION)/redirects/*.c \
	tprdp-$(VERSION)/redirects/*.h \
	tprdp-$(VERSION)/sound/*.c \
	tprdp-$(VERSION)/sound/*.h \
	tprdp-$(VERSION)/utilities/*.c \
	tprdp-$(VERSION)/utilities/*.h \
	tprdp-$(VERSION)/keymaps/?? \
	tprdp-$(VERSION)/keymaps/??-* \
	tprdp-$(VERSION)/keymaps/common \
	tprdp-$(VERSION)/keymaps/modifiers \
	tprdp-$(VERSION)/keymaps/convert-map
	rm -rf /tmp/tprdp-make-dist-dir

.PHONY: dist-noversion
dist-noversion: tprdp.tar.gz

tprdp.tar.gz: tprdp-$(VERSION).tar.gz
	mkdir -p /tmp/tprdp-make-dist-dir
	tar zxvf $< -C /tmp/tprdp-make-dist-dir
	mv /tmp/tprdp-make-dist-dir/tprdp-$(VERSION) /tmp/tprdp-make-dist-dir/tprdp
	ls /tmp/tprdp-make-dist-dir/tprdp
	tar zcvf $@ -C /tmp/tprdp-make-dist-dir tprdp
	rm -rf /tmp/tprdp-make-dist-dir

Makefile: Makefile.in configure
	./config.status

configure: configure.ac
	./bootstrap

.SUFFIXES:
.SUFFIXES: .c .o

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: doc/AUTHORS
doc/AUTHORS:
	./genauthors *.c
