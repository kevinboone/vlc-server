#!/bin/bash
# Install the sample scripts. DO NOT RUN THIS until you have read the
#   README.md file.
cp vlc-server.rc /etc
cp -p start-vlc-server.sh stop-vlc-server.sh /usr/bin
cp -p vlcserverd.openrc /etc/init.d/vlcserverd
