
<h1 align="center">
  <img src="https://github.com/kmendell/thinpi/raw/master/assets/logo/icon-colors.png" width="150px"/><br/>
  ThinPi
</h1>
<p align="center">Open Source Thin Client for the Raspberry Pi built in <b>C</b>

<p align="center"><a href="https://github.com/create-go-app/cli/releases" target="_blank"><img src="https://img.shields.io/badge/version-v0.2.0-blue?style=for-the-badge&logo=none" alt="cli version" /></a>&nbsp;<a href="https://pkg.go.dev/github.com/create-go-app/cli/v2?tab=doc" target="_blank"><img src="https://img.shields.io/badge/C-C17+-00ADD8?style=for-the-badge&logo=C" alt="go version" /></a><a href="" target="_blank">&nbsp;<img src="https://img.shields.io/badge/license-apache_2.0-red?style=for-the-badge&logo=none" alt="license" /></p>

## ⚡️ Quick start

Run the Following Commands in The Terminal to Download, Compile, and Install all Dependencies and ThinPi Files

```bash
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

##Run Either the Configuration Manager or the Connection Manager
/thinpi/thinpi-config
or
/thinpi/thinpi-manager
```
or
  
Download the thinpi_start.sh from this repo
  
```bash
chmod +x ./thinpi_start.sh
sudo ./thinpi_start.sh
```
  
That's all you need to get ThinPi Installed! 🎉

### 🐳 Docker-way to quick start

> 🔔 Docker Building Support is Currently on hold while we work on the new update

## ⚙️ Commands & Options - TODO

### `tpcli`
-u	: Updates the ThinPi Filesystem to the Latest Code<br>
-ua  : Updates the host system using APT<br>
-d	: Deletes all ThinPi Configuration Files<br>
-i pkg	: Installs a package from APT pkg is the package<br>
-r pkg	: Installs a package from APT pkg is the package<br>
--reboot<br>
--adduser user<br>
--deluser user<br>
--man-start	: Starts ThinPi Connection Manager<br>
--config-start  : Starts ThinPi Configuration Manager<br>

## 📝 ThinPi Web Console

Certain Aspects of ThinPi can be Controlled via a Browser

Navigate to 0.0.0.0/dashboard to view the web based dashboard

> 🔔 0.0.0.0 should be replaced with the local ip of the device

## ⚠️ License

`ThinPi` is free and open-source software licensed under the [Apache 2.0 License](https://github.com/kmendell/thinpi/blob/master/LICENSE), and distributed under [Creative Commons](https://creativecommons.org/licenses/by-sa/4.0/) license (CC BY-SA 4.0 International).
