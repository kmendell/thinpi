CC=gcc
CFLAGS=`pkg-config --cflags --libs gtk+-3.0` -lX11

TARGET = output/thinpi/thinpi-*
ODIR = output/thinpi
RED=\e[38;5;199m
BLUE=\e[92m
GRN=\e[90m


all: clean manager config tprdp

manager: src/thinpi/manager.c src/thinpi/rdp.c src/thinpi/helpers.c src/include/ini.c src/thinpi/ini.c src/thinpi/settings.c
	@echo "${RED}TPBUILD[*] - Building connect-manager ..."
	@$(CC) -w src/thinpi/manager.c src/thinpi/rdp.c src/thinpi/settings.c src/thinpi/helpers.c src/include/ini.c src/thinpi/ini.c $(CFLAGS)  -o $(ODIR)/thinpi-manager
	@$(CC) -w src/thinpi/tptofu.c src/thinpi/helpers.c src/include/ini.c src/thinpi/ini.c $(CFLAGS) -o $(ODIR)/thinpi-tofu
	@echo "${GRN}TPBUILD[*] - Copying Files ..."
	cp src/Interface/connect-manager.glade output/thinpi/Interface/connect-manager.glade
	cp src/Interface/tofu.glade output/thinpi/Interface/tofu.glade
	cp src/Interface/settings.glade output/thinpi/Interface/settings.glade
	@echo "${BLUE}TPBUILD[*] - connect-manager Build Succeded"

config:
	@echo "${RED}TPBUILD[*] - Building config-manager ..."
	@$(CC) -w src/thinpi/configmanager.c src/thinpi/helpers.c src/include/ini.c src/include/minIni.c src/thinpi/ini.c $(CFLAGS) -o $(ODIR)/thinpi-config
	@echo "${GRN}TPBUILD[*] - Copying Files ..."
	cp src/Interface/configmanager.glade output/thinpi/Interface/configmanager.glade
	@echo "${BLUE}TPBUILD[*] - config-manager Build Succeded"

tprdp:
	@echo "${RED}TPBUILD[*] - Building tprdp ..."
	@cd src/tprdp; \
	autoreconf -i; \
	./configure --disable-credssp --disable-smartcard >/dev/null; \
	make --no-print-directory -s
	@echo "${GRN}TPBUILD[*] - Copying Files ..."
	@cp src/tprdp/tprdp  output/usr/bin/tprdp
	@echo "${BLUE}TPBUILD[*] - tprdp Build Succeded"

extras: 
	@echo "${RED}TPBUILD[*] - Building thinpi-cli ..."
	@$(CC) -w src/thinpi/cli/tpcli.c -o output/usr/bin/tpcli
	@echo "${BLUE}TPBUILD[*] - CLI Tool Built"

install: 
	@echo "${RED}TPBUILD[*] - Installing ThinPi to your OS ..."
	@echo "${GRN}TPBUILD[*] - Copying Files ..."
	@echo "${GRN}TPBUILD[*] - Copying Files to /usr/bin..."
	@cp -r output/usr/bin/* /usr/bin/
	@echo "${GRN}TPBUILD[*] - Copying Files to /thinpi..."
	@cp -r output/thinpi/* /thinpi
	@echo "${GRN}TPBUILD[*] - Changing /thinpi Permissions ..."
	@chmod -R 0777 /thinpi
	@chown -R kmendell /thinpi
	@sudo chown root:root /usr/bin/thinpi-cli
	@sudo chmod 0777 /usr/bin/thinpi-cli
	@echo "${BLUE}TPBUILD[*] - ThinPi has been Installed!"

clean:
	@echo "${RED}TPBUILD[*] - Cleaning Old Build Files"
	@echo "${GRN}TPBUILD[*] - Cleaning Files ..."
	@sleep 1
	@$(RM) $(TARGET)
	@cd src/tprdp; \
	rm -Rf *.o
	@cd src/Interface; \
	rm -Rf *.glade~
	@cd output/thinpi/Interface; \
	rm -Rf *.glade~
	@echo "${BLUE}TPBUILD[*] - Cleaning Complete"

