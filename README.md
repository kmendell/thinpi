<div align="center">
<br><img height="50%" width="50%" src="assets/logo/logo-colors@2x.png"></img>
</div>

# Thinpi

ThinPi is a open source Thin Client for the Raspberry Pi 4

Developed and Maintained By: Kyle Mendell

This project has been being developed on a RPi4 so it is fully stabled on all of those devices, It has not been tested on any other Pi's, That being said it is running a armhf verison not a arm64 based OS

## Installation

Image Release Coming Soon

## Building - Project Only

```bash
git clone https://git.kmprojects.us/kmprojects/thinpi.git

cd thinpi

sudo ./configure 

make all

```

**The configure command must be ran as sudo - it handles all dependency install and directory creation**

This will compile all project files into the output directory in there repective directorys (output is /)

Note however this will not build a image to flash to an SD Card

If you want to install the files to the filesystem run 

```bash
sudo make install
```

This needs to be ran as sudo because it copys file into /usr/bin and /boot

## ThinPi Usage

Upon the first boot of the Pi you will be prompted by a custom LXDE Desktop

The start menu in the top left holds all functtions you will need to use thin pi

Under ThinPi > Connect Manager - This is where you will go to connect to the servers you have added in the config files

Under ThinPi > Config Manager - This is where youll go to add or remove servers to the config file that the Connect Manager will read from

There are also options for a Terminal and File Manager (In most cases these will only be used by the admin)

Custom Commands included in this distro are: Image Release Only

editmenu - This will open up the Start Menu Editor to edit the menu items shown to the user 

showconfig - This will open the file manager to the config directory if you need to maually edit or remove servers or devices from the config file

thinpi-cli - Admin Console for system commands **This needs to be ran as sudo**


## Contributing

Email: getthinpi@gmail.com to join the team and discuss ceertain changes or improvements

## License
[GNU](https://choosealicense.com/licenses/gpl-3.0/)
