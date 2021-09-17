##Setup ThinPi FS
RED='\033[0;31m'
BLUE='\033[1;34m'

echo ""
echo -e "${RED}Creating /thinpi ..."
echo ""
sudo mkdir -p /thinpi
echo -e "${RED}Changing /thinpi permissions ..."
echo ""
useradd -d /home/thinpi thinpi
sudo chmod 0777 /thinpi
echo -e "${RED}Current User: ${BLUE}thinpi"
echo -e "${RED}Changing Ownership of /thinpi to ${BLUE}thinpi"
echo ""
sudo chown thinpi:thinpi /thinpi
##Get the latest master source code
git clone --recursive https://github.com/kmendell/thinpi.git
##Change to the ThinPi Directory
cd thinpi
##Run the Dpendencies Install and FS Setup Command
echo -e "${RED}Installing ThinPi Build Dependencies..."
sudo apt-get install -y gtk+-3.0-dev gcc make shc build-essential libtasn1-6-dev nettle-dev gnutls-dev git-core debhelper cdbs dpkg-dev autotools-dev pkg-config xmlto libssl-dev docbook-xsl xsltproc libxkbfile-dev libx11-dev libwayland-dev libxrandr-dev libxi-dev libxrender-dev libxext-dev libxinerama-dev libxfixes-dev libxcursor-dev libxv-dev libxdamage-dev libxtst-dev libcups2-dev libpcsclite-dev libasound2-dev libpulse-dev libjpeg-dev libgsm1-dev libusb-1.0-0-dev libudev-dev libdbus-glib-1-dev uuid-dev libxml2-dev libfaad-dev apache2
##Make the project
make all
##Install ThinPi to the Linux Container
sudo make install
