#!/bin/bash
. /etc/vlc-server.rc
vlc-server-client -p $PORT shutdown
