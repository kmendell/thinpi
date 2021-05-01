CC=gcc
CFLAGS=`pkg-config --cflags --libs gtk+-3.0`

TARGET = output/thinpi/thinpi-*
ODIR = output/thinpi
CDATE=`date '+%Y_%m_%d__%H_%M'`;

all: manager config cli tpsudo

req:
	sudo apt-get install gtk3.0 gtk+-3.0-dev freerdp2-x11 gcc make 

manager: src/thinpi/managerv2.c src/thinpi/rdp.c src/thinpi/helpers.c src/thinpi/addserver.c
	$(CC) -w src/thinpi/managerv2.c src/thinpi/rdp.c src/thinpi/helpers.c $(CFLAGS)  -o $(ODIR)/thinpi-manager

config:
	$(CC) -w src/thinpi/addserver.c src/thinpi/helpers.c $(CFLAGS) -o $(ODIR)/thinpi-config

cli:

	echo " [THINPI] - CLI Tool Built "

tpsudo:

	echo "[THINPI] - tpsudo Built"

install: cleaninstall
	sudo mkdir /thinpi
	sudo chown $(USER):$(USER) /thinpi
	cp -r output/thinpi/* /thinpi
	
git:
	git add . && git commit -m "v2 Update $(CDATE)" && git push origin master -f
	
cleaninstall:
	rm -Rf /thinpi

clean:
	$(RM) $(TARGET)

