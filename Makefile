CC=gcc
CFLAGS=`pkg-config --cflags --libs gtk+-3.0`

TARGET = output/thinpi/thinpi-*
ODIR = output/thinpi


all: manager tprdp extras

manager: src/thinpi/manager.c src/thinpi/rdp.c src/thinpi/helpers.c src/include/ini.c src/thinpi/ini.c
	@echo "[THINPI] - Building connect-manager ..."
	$(CC) -w src/thinpi/manager.c src/thinpi/rdp.c src/thinpi/helpers.c src/include/ini.c src/thinpi/ini.c $(CFLAGS)  -o $(ODIR)/thinpi-manager
	$(CC) -w src/thinpi/tptofu.c src/thinpi/helpers.c src/include/ini.c src/thinpi/ini.c $(CFLAGS) -o $(ODIR)/thinpi-tofu
	cp src/Interface/connect-manager.glade output/thinpi/Interface/connect-manager.glade
	cp src/Interface/tofu.glade output/thinpi/Interface/tofu.glade

config:
	@echo "[THINPI] - Building config-manager ..."
	$(CC) -w src/thinpi/configmanager.c src/thinpi/helpers.c src/include/ini.c src/include/minIni.c src/thinpi/ini.c $(CFLAGS) -o $(ODIR)/thinpi-config
	cp src/Interface/configmanager.glade output/thinpi/Interface/configmanager.glade

tprdp:
	@echo "[THINPI] - Building tprdp ..."
	cd src/tprdp; \
	autoreconf -i; \
	./configure --disable-credssp --disable-smartcard; \
	make --no-print-directory
	cp src/tprdp/tprdp  output/usr/bin/tprdp

extras: 
	@echo "[THINPI] - Building thinpi-cli ..."
	$(CC) -w src/thinpi/cli/tpcli.c -o output/usr/bin/tpcli
	@echo "[THINPI] - CLI Tool Built"
	@echo "[THINPI] - Installing Base HTTP Data ..."
	gcc -w src/thinpi-http/reboot.c -o src/thinpi-http/build/reboot.cgi
	gcc -w src/thinpi-http/shutdown.c -o src/thinpi-http/build/shutdown.cgi
	gcc -w src/thinpi-http/default.c -o src/thinpi-http/build/default.cgi
	cp -r src/thinpi-http/build/* output/var/www/html/
	cp src/thinpi-http/index.cgi output/var/www/html/index.cgi
	@echo "[THINPI] - Installed HTTP Base Data"
	@echo "[THINPI] - Installing HTTP Dashboard ..."
	cp -r src/thinpi-http/dashboard/* /var/www/html/dashboard/
	shc -f src/thinpi/tpupdate/tpupdate
	cp src/thinpi/tpupdate/tpupdate.x output/usr/bin/tpupdate
	@echo "[THINPI] - Installed HTTP Dashboard"

install: 
	cp -r output/usr/bin/* /usr/bin/
	cp -r output/thinpi/* /thinpi
	cp -r output/var/www/html/* /var/www/html/
	chmod -R 0777 /thinpi
	chown -R root /thinpi
	sudo chown root:root /usr/bin/thinpi-cli
	sudo chmod 0777 /usr/bin/thinpi-cli

clean:
	$(RM) $(TARGET)
	cd src/tprdp; \
	rm -Rf *.o
	cd src/Interface; \
	rm -Rf *.glade~

