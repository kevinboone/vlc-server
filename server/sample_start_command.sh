#!/bin/bash
vlc-server -f -r /home/kevin/mediafiles/audio_music/ -- --no-video -A alsa --alsa-audio-device front:CARD=PCH,DEV=0

