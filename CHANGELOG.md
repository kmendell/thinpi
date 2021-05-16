# Changelog

All notable changes to this project will be documented in this file.

## 0.2.0 - In Progress
- [Optimized] Created thinpi.h for importing into other programs or files
- [Optimized] Restructured the fs of the projects
- [Optimized] Code cleanup (still on going)
- [UI] UI layout change added more top margin
- [Optimized] RDP compression has been reduced 
- [Optimized] RDP gfx is now using RDP 8.1 thin-client graphics
- [Build] Added makefile instead of build scripts
- [Feature] Added add server ui to add servers to config files (only adding works as of now)
- [GIT] Updated Git Repo Structure and moved to selfhosted gitlab
- [CLI] CLI Tool has been started in this build
- [SUDO] tpsudo progress has begun
- [RDP] Added FreeRDP fork called tprdp (stripped down version to fit our needs)
- [UPDATE] Added version.tlp file this will check with our server to see if the OS it self needs an update using tpupdate command
- [Optimized] By Default ThinPi will use wayland over X11 to fix the tearing issue X11 causes 
- [Devices] USB, Home Drives, and Audio all now get Passed Through 
- [Feature] Device pass-through support - Done
- [Build] Implement custom FreeRDP build (tprdp) - Done
- [RDP] Bumped up to FreeRDP v2.3.2 -- Pulled -- Canceled
- [RDP] Switched to a custom tpdp implementation (rdesktop)
- [RDP] If logging into a Domain Based Server put -DOMAINNAME in the config file after the name of the server
- [CONFIG] Adding and Removing Servers now works completely
