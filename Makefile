CC=gcc
CFLAGS=`pkg-config --cflags --libs gtk+-3.0`

TARGET = output/thinpi/thinpi-*
ODIR = output/thinpi


all: manager config cli tpsudo

req:
	sudo apt-get install gtk3.0 gtk+-3.0-dev freerdp2-x11 gcc make 

manager: src/thinpi/managerv2.c src/thinpi/rdp.c src/thinpi/helpers.c src/thinpi/addserver.c
	$(CC) -w src/thinpi/managerv2.c src/thinpi/rdp.c src/thinpi/helpers.c $(CFLAGS)  -o $(ODIR)/thinpi-manager

config:
	$(CC) -w src/thinpi/addserver.c src/thinpi/helpers.c $(CFLAGS) -o $(ODIR)/thinpi-config

cli: src/thinpi/cli/cli.c
	$(CC) -w src/thinpi/cli/cli.c -o output/usr/bin/thinpi-cli
	@echo "[THINPI] - CLI Tool Built"

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

