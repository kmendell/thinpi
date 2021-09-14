#! /bin/bash

##ThinPi installer for any raspberry pi without using the .img file 
## IMPORTANT THIS SCRIPT REQUIRES SUDO

sudo useradd -m -p thinpipassword -s /bin/bash thinpi

## sudo mkdir -pv /etc/systemd/system/getty@tty1.service.d
## sudo echo '[Service] ExecStart= ExecStart=-/sbin/agetty --autologin thinpi --noclear %I 38400 linux'  >> /etc/systemd/system/getty@tty1.service.d/autologin.conf

##Get thinpi and install it with thinpi

cd /home/thinpi
git clone https://github.com/kmendell/thinpi.git
cd thinpi
sudo scripts/tpsetup
sudo make all
sudo make install

## Finish Setup
