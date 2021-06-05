#!/bin/bash
echo "ThinPi Automated Installer"
echo "Getting the Source Code"
sleep 2
git clone --recursive https://github.com/kmendell/thinpi.git
cd thinpi
echo "Installing Dependencies and Setting up the FS"
sleep 2
sudo scripts/tpdepends
echo "Compiling Project"
make all
echo "Installing ThinPi"
sudo make Install
echo "Thin Pi Installation Complete!"
