CC=gcc
CFLAGS= --libs gtk+-3.0

TARGET = output/thinpi-manager
TARGETBETA = output/thinpi-beta

all: release

req:
	sudo apt-get install gtk3.0 gtk+-3.0-dev freerdp2-x11

release: src/thinpi/managerv2.c src/thinpi/rdp.c src/thinpi/helpers.c src/thinpi/addserver.c
	gcc -w `pkg-config --cflags --libs gtk+-3.0` src/thinpi/managerv2.c src/thinpi/rdp.c src/thinpi/helpers.c -o output/thinpi-manager
	echo "[THINPI] - Connection Manager built at system/thinpi-manager"
	gcc -w `pkg-config --cflags --libs gtk+-3.0` src/thinpi/addserver.c src/thinpi/helpers.c -o output/config-manager
	echo "[THINPI] - Configuration Manager built at system/config-manager"
	
	
beta: src/thinpi/managerv2.c src/thinpi/rdp.c src/thinpi/helpers.c src/thinpi/addserver.c
	gcc  `pkg-config --cflags --libs gtk+-3.0` src/thinpi/managerv2.c src/thinpi/rdp.c src/thinpi/helpers.c -o output/beta
	echo "[THINPI] - Connection Manager built at system/beta"
	gcc  `pkg-config --cflags --libs gtk+-3.0` src/thinpi/addserver.c src/thinpi/helpers.c -o output/beta-config
	echo "[THINPI] - Configuration Manager built at system/beta-config"
	
git:
	git add . && git commit -m "v2 update" && git push origin master -f
	
clean:
	$(RM) $(TARGET) $(TARGETBETA)

