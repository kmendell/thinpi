CC=gcc
CFLAGS=`pkg-config --cflags --libs gtk+-3.0`

TARGET = output/thinpi/thinpi-*
ODIR = output/thinpi


all: manager config cli tpsudo tprdp tpupdate

manager: src/thinpi/managerv2.c src/thinpi/rdp.c src/thinpi/helpers.c src/thinpi/addserver.c
	@echo "[THINPI] - Building connect-manager ..."
	$(CC) -w src/thinpi/managerv2.c src/thinpi/rdp.c src/thinpi/helpers.c $(CFLAGS)  -o $(ODIR)/thinpi-manager
	cp src/Interface/connect-manager.glade output/thinpi/Interface/connect-manager.glade

config:
	@echo "[THINPI] - Building config-manager ..."
	$(CC) -w src/thinpi/addserver.c src/thinpi/helpers.c $(CFLAGS) -o $(ODIR)/thinpi-config
	cp src/Interface/addserver.glade output/thinpi/Interface/addserver.glade

cli: src/thinpi/cli/cli.c
	@echo "[THINPI] - Building thinpi-cli ..."
	$(CC) -w src/thinpi/cli/cli.c -o output/usr/bin/thinpi-cli
	@echo "[THINPI] - CLI Tool Built"

tprdp:
	@echo "[THINPI] - Building tprdp ..."
	cd src/thinpi-rdp; \
	make  --no-print-directory
	cp src/thinpi-rdp/tprdp  output/usr/bin/tprdp

tpupdate:
	shc -f src/thinpi/tpupdate/tpupdate
	cp src/thinpi/tpupdate/tpupdate.x output/usr/bin/tpupdate

tpsudo:

	@echo "[THINPI] - tpsudo Built"

install: 
	cp output/usr/bin/tprdp /usr/bin/tprdp
	cp -r output/thinpi/* /thinpi
	chmod -R 0777 /thinpi
	chown -R pi /thinpi
	
git:
	git add . && git commit -m "v2 Update" && git push origin master -f
	
uninstall:
	rm -Rf /thinpi

clean:
	$(RM) $(TARGET)

