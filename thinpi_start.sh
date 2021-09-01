##Get the latest master source code
git clone --recursive https://github.com/kmendell/thinpi.git
##Change to the ThinPi Directory
cd thinpi
##Run the Dpendencies Install and FS Setup Command
sudo scripts/tpdepends
##Make the project
make all
##Install ThinPi to the Linux Container
sudo make install
