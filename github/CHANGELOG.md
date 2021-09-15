# Changelog

All notable changes to this project will be documented in this file.

## 0.3.0 
- [WSL] Compatible with WSL
- [GUI] Added TOFU Dialog when connecting to new SSL Server
- [CLI] Rewrote the CLI so it takes single arguments - ***WIP***
- [RDP] Rewrote tprdp in someplaces, restructured files, and renamed varibles for thinpi use
- [CONFIG] Removed text based server file and implemented .ini config files (removed stable config manager from master until a new gui has been built for ini files)
- [CONFIG] Adding Server through the new config manager now works, however only one server is able to be added currently
- [CONFIG] Added minIni source from github to help manag ini writing and removing servers

## 0.2.0 - RC
- [RDP] Switched to a custom tpdp implementation (rdesktop)
- [CONFIG] Adding and Removing Servers now works completely
- [CLI] CLI can now install and remove packages
- [BUILD] Updated the configure script : removed obsolete packages, added colors, runs thinpi setup scripts, removed output from commands
- [SYSTEM] Added/Updated Memory Management for thinpi core apps
- [SYSTEM] Fixed Memory Error When Launching ThinPi-Manager
- [BUILD] Configure Script renamed to scripts/tpdepends and scripts/tpsetup
- [BUILD] Automake removed from main project
- [RC] 0.2.0 RC

