# nc-vlc-client

Version 0.1d, November 2023

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
keyboard. USB numeric keypads and remote controls are widely 
available and dirt cheap, 
and can easily be dismantled and integrated into a custom hardware
design. I particularly favour the Microsoft Media Centre 
remotes, that have a USB receiver. The only problem with this 
cheap devices is that the button are a bit bouncy, so sometimes
extra button-presses get generated.



## Key bindings

The whole menu structure is designed to be navigated using a 
Microsoft Media Centre remote control with USB receiver. So the key
bindings are a little odd, for keyboard users. In particularly,
to select a menu item, use the cursor-right key. To navigate to
the previous item, use cursor-left. On the remote, all navigation
can be done using the four-way navigation pad, which generates
cursor-movement keystrokes.

The interface also responds to page-up and page-down keys, which
have corresponding buttons on the remote. Play/pause/stop on the remote
work, as doe previous/next. Use the 'help' menu to see the
console key bindings. The correspondong keys on the remote are not
very easy to document, but I hope they are reasonably self-explanatory. 

`nc-vlc-client` will run in a terminal emulator, but also in the
kernel console, so it will work in a system with no graphical
desktop or X support. But Unicode support is a little tricky
in this set-up (see below).

## Limitations

Apart from the obvious lack of functionality at present, there are some
striking limitations.

`nc-vlc-client` will probably only work with UTF-8 terminal configurations.
The data fields in the `vlc-server` media database are assumed to be
UTF-8, and there is no conversion in the program. 

All the text and error messages are in English only.

Where tests are case-insensitive, the notion of 'case' is one that
applies to the ASCII character set only.

The program does not change its layout dynamically, if the terminal size
changes at run time.

The `ncurses` library is notoriously fussy about Unicode support. 
`vlc-server` uses UTF-8 throughout, and the client will receive names
of albums, files, etc., in UTF-8 format. The platform's `ncurses` library
needs to have proper Unicode support, as must the terminal the client
runs in. Graphical terminal emulators are usually OK, but a plain
Linux text console might be problematic. The terminal not only must
support UTF-8 wide characters, but it must have the necessary fonts to
render these characters.  There really isn't much that `nc-vlc-client` can
do to correct deficiencies in the console.

## Revision history

0.1d November 2023
- Added 'system info' menu item 


