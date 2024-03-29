# nc-vlc-client

Version 0.1i, November 2023

## What is this?

This is a simple, ncurses-based console interface to the `vlc-server`
media player server, which is in turn an interface to the VLC media player.

The client defaults to communicating with a `vlc-server` instance on
the same host, and port 30000, although these properties can be
changed using the `--host` and `--port` switches (but see 'local mode'
below).

As of version 0.1f, the client can list artists and genres, and get a list
of albums that match the artist or genre. However, the interface
is still entirely album-based. The 'play something' menu command
will select a random album to play.

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
remotes, which have a USB receiver. The only problem with these
cheap devices is that the button are a bit bouncy, so sometimes
extra button-presses get generated.

If there is full keyboard attached, the letter keys select the
starting point in the current display. In the 'ablums' page, for
example, hitting 'C' will move the display to the first album whose
name begins with 'C' (or 'c').

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

## Screen navigation

Use the cursor keys (or 4-way pad on a remote) to select menu items,
and to return to a previous menu. If the a page is displayed for
15 seconds with no activity, control returns to the main menu.
Other than in kiosk mode (see below), if there is no activity in the
main meny for 15 seconds, the program exits.

## Kiosk mode

When run with the `-k` switch, `nc-vlc-client` will not exit, either 
by navigating back from the main menu, or by inactivity timeout.
In addition, it will keep trying to connect to the `vlc-server` server
process indefintely. There is no timeout -- the logic is that, in 
kiosk mode, there simply is nothing to be done if the server is
not contactable.

During the initial connection process in kiosk mode, `nc-vlc-client`
does not display error messages if they are 'connection refused'. This
is taken to be normal, because the client will likely start up more 
quickly than the server.

## Standby info display

After a period with no user-interface activity (currently 15 seconds) the menu
will be replaced by a 'standby' information display.  It shows file meta-data
(artist, album...), and position in the playlist. Pressing any key will restore
the menu. Menu-related keys are not processed immediately, because it makes no
sense to do so when the menu is not on screen. However,
play/pause/next/previous, etc., are processed before the menu display is
redrawn. 

One reason for implementing the standby information display was to do something
to limit screen burn-in. However, some elements of the standby display are
unchanging, so its utility for the purpose is suspect.

If screen burn-in is a real risk, it's still possible to implement screen
blanking at the operating system level. 

## Colour

`nc-vlc-client` has a colour mode, which can be enabled using the
`-c` switch if the terminal supports colour. Colour is used sparingly,
even when it is used at all, because the small screens on the embedded
systems for which this application is intended often do not render
colour well. That's also the reason why it isn't enabled by default.

## Local mode

If the command-line does not include a `--host` argument, this 
interface and the `vlc-server` server must be running on the same
host. In that case, `nc-vlc-client` is running in 'local' mode.
At present, the only difference this makes is that, when displaying
the system information page, the local IP number is displayed in the
URL for attaching a web browser. It would make no sense to display
`localhost` here. 

Additional 'local' operations may be added in future.

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

0.2a January 2024
- Added 'streams' view, with capability to play an Internet radio stream
  from the media database.

0.1k December 2023
- Made title configurable on command line 

0.1j December 2023
- Added 'full scan' menu command 

0.1i December 2023
- Added preliminary colour support 
- Added server storage reporting 

0.1h November 2023
- Added 'play random' key binding 

0.1g November 2023
- Added 'local' mode
- Fixed a nasty memory leak

0.1f November 2023
- Added genre list and artist list
- Added standby info display

0.1e November 2023
- Various bug fixes, mostly involving escaping special characters 


0.1d November 2023
- Added 'system info' menu item 


