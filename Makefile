CC=gcc
CFLAGS=`pkg-config --cflags --libs gtk+-3.0`

TARGET = output/thinpi/thinpi-*
ODIR = output/thinpi

all: manager config cli tpsudo

req:
	sudo apt-get install gtk3.0 gtk+-3.0-dev freerdp2-x11 gcc make 

manger: src/thinpi/managerv2.c src/thinpi/rdp.c src/thinpi/helpers.c src/thinpi/addserver.c
	$(CC) -w src/thinpi/managerv2.c src/thinpi/rdp.c src/thinpi/helpers.c $(CFLAGS)  -o $(ODIR)/thinpi-manager
	echo "[THINPI] - Connection Manager built"

config:
	$(CC) -w src/thinpi/addserver.c src/thinpi/helpers.c $(CFLAGS) -o $(ODIR)/thinpi-config
	echo "[THINPI] - Configuration Manager built"

cli:

	echo "THINPI] - CLI Tool Built"

tpsudo:

	echo "THINPI] - tpsudo Built"
	
git:
	git add . && git commit -m "v2 update" && git push origin master -f
	
clean:
	$(RM) $(TARGET)

