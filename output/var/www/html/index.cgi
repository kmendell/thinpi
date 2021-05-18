#!/bin/bash
echo "Content-type: text/html"
echo ""
echo "<html><head><title>ThinPi - $(hostname)"
echo "</title></head><body><center>"

echo "<h1>System Information</h1>"
echo "<h3>Hostname: $(hostname)</h3>"
echo "<h3>Network IP Address: <br>$(hostname -I)</h3>"
echo "<h3>Linux Kernel Version: $(uname -r)"
echo "<h3>ThinPi OS Version: $(cat /thinpi/version.tpv)"
echo ""
echo "<h1>Functions</h1>"
echo "<a style='color: black; text-decoration: none;' href='reboot.cgi'>Reboot Device</a> || <a style='color: black; text-decoration: none;' href='default.cgi'>Delete ThinPi Config Files</a> || <a style='color: black; text-decoration: none;' href='shutdown.cgi'>Shutdown</a>"
echo ""
echo "<hr>"
echo "<h3>This info was generated at $(date -u)</h3>"
echo "</center></body></html>