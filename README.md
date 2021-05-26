<div align="center">
<br><img height="50%" width="50%" src="assets/logo/logo-colors@2x.png"></img>
</div>

# Thinpi

ThinPi is a open source Thin Client for the Raspberry Pi 4

Developed and Maintained By: Kyle Mendell

This project has been being developed on a RPi4 so it is fully stabled on all of those devices, It has not been tested on any other Pi's, That being said it is running a armhf verison not a arm64 based OS

## Building Using Docker

**ThinPi Now Uses Docker as a Build Context**

```bash

docker pull kmendell/tpdocker:alpha

docker run -it -v /local/path:/thinpi kmendell/tpdocker:alpha

```

/local/path is where you want the output of the ThinPi FS to save too 

Once built Navigate to that local path and copy all files and folders to the root of the SD Card of the Raspberry Pi

**This Docker Context will EVENTUALLY build the complete .img - Currently it does not**

If you encounter any errors please open a issue and email getthinpi@gmail.com

## Contributing

Email: getthinpi@gmail.com to join the team and discuss certain changes or improvements
