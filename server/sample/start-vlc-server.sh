#!/bin/bash
. /etc/vlc-server.rc
vlc-server -r "$MEDIA_DIR" -p $PORT -- --no-video -A alsa --alsa-audio-device $ALSA_DEV 
