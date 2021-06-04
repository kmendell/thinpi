
<h1 align="center">
  <img src="https://github.com/kmendell/thinpi/raw/master/assets/logo/icon-colors.png" width="150px"/><br/>
  ThinPi
</h1>
<p align="center">Open Source Thin Client for the Raspberry Pi built in <b>C</b>

<p align="center">
  <a href="https://github.com/kmendell/thinpi/releases" target="_blank"><img src="https://img.shields.io/badge/download-v0.2.0-FC427B?style=for-the-badge&logo=none" alt="thinpi version" /></a>

## ⚡️ Quick start

Run the Following Command in The Terminal to Download, Compile, and Install all Dependencies and ThinPi Files

```bash
sudo bash -c "$(curl -fsSL https://thinpi.kmprojects.us/install.sh)"
```

ThinPi is also able to run on WSL

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
That's all you need to get ThinPi Installed! 🎉

### 🐳 Docker-way to quick start

```bash
docker run -it -v /local/path:/thinpi kmendell/tpdocker:build
```

> 🔔 ***/local/path*** is where the contents of the build will go on the host machine 

## ⚙️ Commands & Options - TODO

### `tpcli`
  
`tpcli -u` Update ThinPi Project<br>
`tpcli -ua` Update APT Packages<br>
`tpcli -d` Remove all ThinPi Configuration<br>
`tpcli -i packagename` Install APT Package<br>
`tpcli -r packagename` Remove APT Package<br>
`tpcli --reboot` Reboot System<br>
`tpcli --adduser username` Add new Linux User<br>
`tpcli --deluser username` Remove Linux User<br>
`tpcli --man-start` Start Connection Manager<br>
`tpcli --config-start` Start Config Utility<br>

## 📝 ThinPi Web Console

Certain Aspects of ThinPi can be Controlled via a Browser

Navigate to 0.0.0.0/dashboard to view the web based dashboard

> 🔔 0.0.0.0 should be replaced with the local ip of the device

## ⚠️ License

`ThinPi` is free and open-source software licensed under the [Apache 2.0 License](https://github.com/kmendell/thinpi/blob/master/LICENSE), and distributed under [Creative Commons](https://creativecommons.org/licenses/by-sa/4.0/) license (CC BY-SA 4.0 International).
