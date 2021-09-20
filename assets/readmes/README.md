   ![GitHub all releases](https://img.shields.io/github/downloads/kmendell/thinpi/total?label=Downloads)
![GitHub repo size](https://img.shields.io/github/repo-size/kmendell/thinpi?label=Repo%20Size)
![GitHub issues](https://img.shields.io/github/issues/kmendell/thinpi)
![GitHub tag (latest by date)](https://img.shields.io/github/v/tag/kmendell/thinpi?label=Version)
![GitHub forks](https://img.shields.io/github/forks/kmendell/thinpi?style=social)
![GitHub contributors](https://img.shields.io/github/contributors/kmendell/thinpi)
![Website](https://img.shields.io/website?down_color=red&down_message=offline&up_color=green&up_message=online&url=https%3A%2F%2Fthinpi.kmprojects.us)
![GitHub branch checks state](https://img.shields.io/github/checks-status/kmendell/thinpi/master)
<div align="center">
<br><img height="50%" width="50%" src="assets/logo/logo-colors@2x.png"></img>
</div>

# Thinpi

ThinPi is a open source Thin Client for the Raspberry Pi 4

ThinPi Can Connect to any Machine that has a RDP Server Running 

Currently RDP is only supported in the future pottentially will add citrix based support and so on

Developed and Maintained By: Kyle Mendell

ThinPi is a Debian Based arm32v7 based OS by default, Build for arm64 will be available soon



## Building

**If you build manually and dont use docker, you will have to run the scripts in the scripts folder to ensure dependencies are installed**

**ThinPi Now Uses Docker as a Build Context**

```bash

docker pull kmendell/tpdocker:build

docker run -it -v /local/path:/thinpi kmendell/tpdocker:build

```

/local/path is where you want the output of the ThinPi FS to save too 

Once built Navigate to that local path and copy all files and folders to the root of the SD Card of the Raspberry Pi

### Docker tag options:

```
:build - Build the latest stable source code of ThinPi

:testing - Build the absolute latest (nightly) builds of ThinPi

:latest - Same thing as build

:alpha - Build the not stable but not nightly builds of ThinPi
```

**This Docker Context will EVENTUALLY build the complete .img - Currently it does not**

If you encounter any errors please open a issue and email getthinpi@gmail.com

## Running with WSL

Download WSL for Windows 10 https://aka.ms/wslinstall

Download Ubuntu 20.04 LTS from the MS Store

Download GWSL from the MS Store

Enable Display/Audio Auto Exporting Under GWSL Distro Tools

Run this command under the linux instance 

```bash
git clone --recursive https://github.com/kmendell/thinpi.git

cd thinpi

sudo scripts/tpdepends

make all

sudo make install 

/thinpi/thinpi-config

or

/thinpi/thinpi-manager
```

***This also can be the way to build manually***
## Github Branch Tags

Each Version of ThinPi will get its own branch or "tag"

Any features in a development/testing phase will first be pushed to master then once stable will be pushed to the correct version tag

## Contributing

Email: getthinpi@gmail.com to join the team and discuss certain changes or improvements
