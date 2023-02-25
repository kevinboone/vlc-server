# nc-vlc-client

Version 0.2b, February 2023

## What is this?

This is a simple, ncurses-based console interface to the `vlc-server`
media player server, which is in turn an interface to the VLC media player.

The client defaults to communicating with a `vlc-server` instance on
the same host, and port 30000, although these properties can be
changed using the `--host` and `--port` switches. 

At present, the client displays a list of albums, or the current
playlist. Selecting an album from the list plays it; selecting a 
track from the playlist moves playback to that point. There are
key bindings for pausing and resuming playback, moving between 
items in the playlist, etc.

`nc-vlc-client` has a long way to go before it can be considered a
fully-featured, console-based music player. It's also important to
understand that it's only an interface to `vlc-server` -- not a music
player in its own right. To use `nc-vlc-client`, `vlc-sever` must first
be set up and running. 

## Design goals

The main design goal of `nc-vlc-client` is that all functions are
accessible via menus using a four-way keypad. Other keys do have a 
function but, at a pinch, only four keys are needed. 

The reason for this design is so `nv-vlc-client` can be used as 
a player interface in an embedded Linux system, that may have no
keyboard. USB numeric keypads are widely available and dirt cheap, 
and can easily be dismantled and integrated into a custom hardware
design.  

