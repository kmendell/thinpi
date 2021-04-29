CC=gcc
CFLAGS= --libs gtk+-3.0

TARGET = system/thinpi-manager
TARGETBETA = system/thinpi-beta

all: release

req:
	sudo apt-get install gtk3.0 gtk+-3.0-dev freerdp2-x11

release: src/managerv2.c src/rdp.c src/helpers.c src/addserver.c
	gcc -w `pkg-config --cflags --libs gtk+-3.0` src/managerv2.c src/rdp.c src/helpers.c -o system/thinpi-manager
	echo "[THINPI] - Connection Manager built at system/thinpi-manager"
	gcc -w `pkg-config --cflags --libs gtk+-3.0` src/addserver.c src/helpers.c -o system/config-manager
	echo "[THINPI] - Configuration Manager built at system/config-manager"
	
	
beta: src/managerv2.c src/rdp.c src/helpers.c src/addserver.c
	gcc  `pkg-config --cflags --libs gtk+-3.0` src/managerv2.c src/rdp.c src/helpers.c -o system/beta
	echo "[THINPI] - Connection Manager built at system/beta"
	gcc  `pkg-config --cflags --libs gtk+-3.0` src/addserver.c src/helpers.c -o system/beta-config
	echo "[THINPI] - Configuration Manager built at system/beta-config"
	
git:
	git add . && git commit -m "v2 update" && git push origin master -f
	
clean:
	$(RM) $(TARGET) $(TARGETBETA)

