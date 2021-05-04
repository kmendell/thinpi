CC=gcc
CFLAGS=`pkg-config --cflags --libs gtk+-3.0`

TARGET = output/thinpi/thinpi-*
ODIR = output/thinpi


all: manager config cli tpsudo tprdp tpupdate

manager: src/thinpi/managerv2.c src/thinpi/rdp.c src/thinpi/helpers.c src/thinpi/addserver.c
	@echo "[THINPI] - Building connect-manager ..."
	$(CC) -w src/thinpi/managerv2.c src/thinpi/rdp.c src/thinpi/helpers.c $(CFLAGS)  -o $(ODIR)/thinpi-manager
	cp src/Interface/connectmanger.glade output/Interface/connectmanger.glade

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
	cmake .; \
	make tprdp --no-print-directory
	cp src/freerdp/client/X11/tprdp output/usr/bin/tprdp
	cp src/freerdp/client/Wayland/wltprdp output/usr/bin/wltprdp

tpupdate:
	shc -f src/thinpi/tpupdate/tpupdate
	cp src/thinpi/tpupdate/tpupdate.x output/usr/bin/tpupdate

tpsudo:

	@echo "[THINPI] - tpsudo Built"

install: uninstall
	cp -r output/thinpi/* /thinpi
	chmod -R 0777 /thinpi
	chown -R $USER /thinpi
	
git:
	git add . && git commit -m "v2 Update" && git push origin master -f
	
uninstall:
	rm -Rf /thinpi

clean:
	$(RM) $(TARGET)

