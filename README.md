<div align="center">
<img src="https://github.com/kmendell/thinpi/raw/master/assets/logo/icon-colors.png" align="center" width="150" alt="Project icon">

# ThinPi

<p align="center">Open Source Thin Client for the Raspberry Pi built in <b>C</b>

<p align="center"><a href="https://gitlab.kmprojects.us/thinpi/core/-/releases" target="_blank"><img src="https://img.shields.io/badge/version-v0.3.0(DEV)-hotpink?style=for-the-badge&logo=none" alt="cli version" /></a>&nbsp;<a href="https://github.com/kmendell/thinpi/" target="_blank"><img src="https://img.shields.io/badge/C-C17+-00ADD8?style=for-the-badge&logo=C" alt="go version" /></a><a href="https://github.com/kmendell/thinpi/" target="_blank">&nbsp;<img src="https://img.shields.io/badge/license-apache_2.0-red?style=for-the-badge&logo=none" alt="license" /></p></a>




> 🔔 For a Full Changelog click [Here](https://gitlab.kmprojects.us/thinpi/core/-/blob/master/github/CHANGELOG.md)

## Demo

<img src="assets/demo.png" align="center" width="1000" alt="Project icon">
</div>
## ⚡️ Quick start

Download the thinpi_start.sh from this repo and Run the Following Commands in The Terminal to Download, Compile, and Install all Dependencies and ThinPi Files

```bash
chmod +x ./thinpi_start.sh
sudo ./thinpi_start.sh
```

That's all you need to get ThinPi Installed! 🎉

## 📝 ThinPi INI Config

An Example config has been included in output/thinpi/config/thinpi.ini
Under [thinpi_proto] the numcon varible should be set to how many connections you have starting at 1 NOT 0. See Example below.

```ini
[thinpi_proto]
numcon=1

[connection0]
name = ThinPi Server
ip = 10.4.100.2
usb = 1
printers = 0
drives = 1
res = 1920x1080
domain = thinpi.io
```

### 🐳 Docker-way to quick start

> 🔔 Docker Building Support is Currently on hold while we work on the new update

## ⚠️ License

`ThinPi` is free and open-source software licensed under the [Apache 2.0 License](https://github.com/kmendell/thinpi/blob/master/LICENSE), and distributed under [Creative Commons](https://creativecommons.org/licenses/by-sa/4.0/) license (CC BY-SA 4.0 International).
