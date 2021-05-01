CC=gcc
CFLAGS=`pkg-config --cflags --libs gtk+-3.0`

TARGET = output/thinpi/thinpi-*
ODIR = output/thinpi


all: manager config cli tpsudo tprdp tpupdate

req:
	@echo "[THINPI] - Installing requirements ..."
	sudo apt-get install gtk3.0 gtk+-3.0-dev freerdp2-x11 gcc make cmake shc dialog
	sudo apt-get install ninja-build build-essential git-core debhelper cdbs dpkg-dev autotools-dev cmake pkg-config xmlto libssl-dev docbook-xsl xsltproc libxkbfile-dev libx11-dev libwayland-dev libxrandr-dev libxi-dev libxrender-dev libxext-dev libxinerama-dev libxfixes-dev libxcursor-dev libxv-dev libxdamage-dev libxtst-dev libcups2-dev libpcsclite-dev libasound2-dev libpulse-dev libjpeg-dev libgsm1-dev libusb-1.0-0-dev libudev-dev libdbus-glib-1-dev uuid-dev libxml2-dev libfaad-dev libfaac-dev

manager: src/thinpi/managerv2.c src/thinpi/rdp.c src/thinpi/helpers.c src/thinpi/addserver.c
	@echo "[THINPI] - Building connect-manager ..."
	$(CC) -w src/thinpi/managerv2.c src/thinpi/rdp.c src/thinpi/helpers.c $(CFLAGS)  -o $(ODIR)/thinpi-manager

config:
	@echo "[THINPI] - Building config-manager ..."
	$(CC) -w src/thinpi/addserver.c src/thinpi/helpers.c $(CFLAGS) -o $(ODIR)/thinpi-config

cli: src/thinpi/cli/cli.c
	@echo "[THINPI] - Building thinpi-cli ..."
	$(CC) -w src/thinpi/cli/cli.c -o output/usr/bin/thinpi-cli
	@echo "[THINPI] - CLI Tool Built"

tprdp:
	@echo "[THINPI] - Building tprdp ..."
	cd src/freerdp; \
	make tprdp --no-print-directory
	cp src/freerdp/client/X11/tprdp output/usr/bin/tprdp
	cp src/freerdp/client/Wayland/wltprdp output/usr/bin/wltprdp

tpupdate:
	shc -f src/thinpi/tpupdate/tpupdate
	cp src/thinpi/tpupdate/tpupdate.x output/usr/bin/tpupdate

tpsudo:

	@echo "[THINPI] - tpsudo Built"

install: uninstall
	sudo mkdir /thinpi
	sudo chmod 0777 /thinpi
	cp -r output/thinpi/* /thinpi
	sudo chmod -R 0777 /thinpi
	
git:
	git add . && git commit -m "v2 Update" && git push origin master -f
	
uninstall:
	rm -Rf /thinpi

clean:
	$(RM) $(TARGET)

