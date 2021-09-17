CC=gcc
CFLAGS=`pkg-config --cflags --libs gtk+-3.0` -lX11

TARGET = output/thinpi/thinpi-*
ODIR = output/thinpi


all: manager config tprdp

manager: src/thinpi/manager.c src/thinpi/rdp.c src/thinpi/helpers.c src/include/ini.c src/thinpi/ini.c src/thinpi/settings.c
	@echo "[THINPI] - Building connect-manager ..."
	$(CC) -w src/thinpi/manager.c src/thinpi/rdp.c src/thinpi/settings.c src/thinpi/helpers.c src/include/ini.c src/thinpi/ini.c $(CFLAGS)  -o $(ODIR)/thinpi-manager
	$(CC) -w src/thinpi/tptofu.c src/thinpi/helpers.c src/include/ini.c src/thinpi/ini.c $(CFLAGS) -o $(ODIR)/thinpi-tofu
	cp src/Interface/connect-manager.glade output/thinpi/Interface/connect-manager.glade
	cp src/Interface/tofu.glade output/thinpi/Interface/tofu.glade
	cp src/Interface/settings.glade output/thinpi/Interface/settings.glade

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

install: 
	# cp -r output/usr/bin/* /usr/bin/
	cp -r output/thinpi/* /thinpi
	# chmod -R 0777 /thinpi
	# chown -R kmendell /thinpi
	# sudo chown root:root /usr/bin/thinpi-cli
	# sudo chmod 0777 /usr/bin/thinpi-cli

clean:
	$(RM) $(TARGET)
	cd src/tprdp; \
	rm -Rf *.o
	cd src/Interface; \
	rm -Rf *.glade~
	cd output/thinpi/Interface; \
	rm -Rf *.glade~

