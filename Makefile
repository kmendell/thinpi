CC=gcc
CFLAGS=`pkg-config --cflags --libs gtk+-3.0`

TARGET = output/thinpi/thinpi-*
ODIR = output/thinpi


all: manager cli tprdp tpupdate http

manager: src/thinpi/manager.c src/thinpi/rdp.c src/thinpi/helpers.c src/include/ini.c
	@echo "[THINPI] - Building connect-manager ..."
	$(CC) -w src/thinpi/manager.c src/thinpi/rdp.c src/thinpi/helpers.c src/include/ini.c $(CFLAGS)  -o $(ODIR)/thinpi-manager
	$(CC) -w src/thinpi/tptofu.c src/thinpi/helpers.c src/include/ini.c $(CFLAGS) -o $(ODIR)/thinpi-tofu
	cp src/Interface/connect-manager.glade output/thinpi/Interface/connect-manager.glade
	cp src/Interface/tofu.glade output/thinpi/Interface/tofu.glade

config:
	@echo "[THINPI] - Building config-manager ..."
	$(CC) -w src/thinpi/tpconfig.c src/thinpi/helpers.c src/include/ini.c $(CFLAGS) -o $(ODIR)/thinpi-config
	cp src/Interface/addserver.glade output/thinpi/Interface/addserver.glade

cli: src/thinpi/cli/tpcli.c
	@echo "[THINPI] - Building thinpi-cli ..."
	$(CC) -w src/thinpi/cli/tpcli.c -o output/usr/bin/tpcli
	@echo "[THINPI] - CLI Tool Built"

tprdp:
	@echo "[THINPI] - Building tprdp ..."
	cd src/tprdp; \
	autoreconf -i; \
	./configure --disable-credssp --disable-smartcard; \
	make --no-print-directory
	cp src/tprdp/tprdp  output/usr/bin/tprdp

tpupdate:
	shc -f src/thinpi/tpupdate/tpupdate
	cp src/thinpi/tpupdate/tpupdate.x output/usr/bin/tpupdate

http:
	@echo "[THINPI] - Building http data..."
	gcc -w src/thinpi-http/reboot.c -o src/thinpi-http/build/reboot.cgi
	gcc -w src/thinpi-http/shutdown.c -o src/thinpi-http/build/shutdown.cgi
	gcc -w src/thinpi-http/default.c -o src/thinpi-http/build/default.cgi
	cp -r src/thinpi-http/build/* output/var/www/html/
	cp src/thinpi-http/index.cgi output/var/www/html/index.cgi

dashboard:
	@echo "[THINPI] - Installing HTTP Dashboard ..."
	cp -r src/thinpi-http/dashboard/* /var/www/html/dashboard/

install: 
	cp -r output/usr/bin/* /usr/bin/
	cp -r output/thinpi/* /thinpi
	cp -r output/var/www/html/* /var/www/html/
	chmod -R 0777 /thinpi
	chown -R root /thinpi
	sudo chown root:root /usr/bin/thinpi-cli
	sudo chmod 0777 /usr/bin/thinpi-cli

docker:
	cp -r output/* /thinpi/
	cp -r src/thinpi-http/dashboard/ /thinpi/var/www/html/
	
git:
	git add . -f && git commit -m "Commit From Make Script (AUTOMATED)" && git push origin master -f
	
uninstall:
	rm -Rf /thinpi

clean:
	$(RM) $(TARGET)
	cd src/tprdp; \
	rm *.o

