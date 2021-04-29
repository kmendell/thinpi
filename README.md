# Thinpi

ThinPi is a open source Thin Client for the raspberry pi 4

## Installation

 # Option 1:
 Use the standalone binary and install to any debian based distro

 # Option 2:
 Use Docker to compile the full OS image and flash it onto the USB device

 # Option 3: 
 Download the latest image and flash it onto your SD Card using the Raspberry Pi Imager


## Building - Only for Developers

```bash
git clone https://git.kmprojects.us/kmendell/thinpi.git

cd thinpi

make all

```

New Folders

Packages: Location of the standalone thinpios filesystem .deb packages

boot: The additional files needed to be added to the boot drive for the custom boot logo to appear, be careful with cmdline.txt, only copy the last few commands right after rootwait.

scripts: image building scripts for ease of build purposes.

**This way does not give you the custom desktop or boot logo as the image will** 

## ThinPi Usage

Upon first boot of the Pi you will be greeted with a custom rasbian desktop.

Use the Start menu in the top left to select your option under the Thin Pi Category

Too add your server Opened the Thin Pi Configurator : Start > Thin Pi > Thin Pi Configuration and enter the IP address and Display names in the correct boxes and click add!

Removing Servers this way does not quite work yet, but we are working on that as well

The Main user of the OS is "thinpi" with password of tppassword.

**Note: we are working on building a integrated "sudo" or admin system ourselves**

Currently there is no key combos coded in that allow you to "disconnect" from the server once connected. However Ctl+Alt+Enter will get you out of fullscreen mode of the RDP connection



## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License
[GNU](https://choosealicense.com/licenses/gpl-3.0/)
