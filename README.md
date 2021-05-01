
<div align="center">
<img height="50%" width="50%" src="assets/logo/logo-white@2x.png"></img>
</div>

# Thinpi

ThinPi is a open source Thin Client for the raspberry pi 4

**Run the following commands if your are having issues cloning this repo**
```bash
git config --global http.sslverify false

export GIT_SSL_NO_VERIFY=true
```

## Installation

Download the latest image from the downloads pages

Flash the Image to your SD Card using etcher or Raspberry Pi Imager

Boot up the Pi and continue with the installation


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
