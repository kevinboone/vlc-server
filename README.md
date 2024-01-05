# vlc-server

*This is a work in progress!*
*(And it's likely to remain that way for some time)*

`vlc-server` is a simple HTTP server that wraps the VLC media player, and
provides an HTTP interface for playback of audio files and URLs.  There is a
rudimentary web interface that can be used with a browser, and also a REST-like
API that can be used by other kinds of client. The web interface, and the REST
API, allow audio files to be viewed and selected using metadata (tags), or
just filesystem layout. 

The bundle includes a simple C library for controlling the the server using the
REST API, and command-line and ncurses utilities that uses this library.

The purpose of `vlc-server` is allow media playback on one Linux computer to be
controlled remotely from some other computer. The "controlling" computer can be
anything that can issue HTTP requests.

`vlc-server` is intended to be a replacement for `xine-server`:

https://github.com/kevinboone/xine-server

`vc-server` supports audio metadata (tags), and the web interface allows 
audio files to be displayed and searched by artist, album, genre, etc.
To provide this support, the server scans the media directory and builds
up an sqlite3 database.

The database can also store URLs of, for example, Internet radio stations.
See the seperate README.md in the `radio_support` directory for ideas
how this might be used.

`vlc-server` notionally supports video.  However, since the purpose of the
utility is to allow for remote control, there seems to be little purpose in
using it to play video. The server can optionally be provided with a list of
filename patterns that it will allow to play (e.g., `*.flac,*.mp3`).

I have tested this software on Fedora and Ubuntu, as well as Raspberry Pi.
The Pi, and systems like it, are the main target platform for this software.

## Project status

Done:

- Basic audio playback through the server essentially works
- The web interface is not particularly elegant, but it works. Audio
  files can be listed and browsed, with cover art if available
- Media scanning and the construction of the media database essentially
  works, but is rather crude. In particular, the scanner ignores any
  audio file that is already in the database, even though its metadata
  may have changed. A full scan can be forced by running the scanner
  on the command line
- There is an API library that can be used by clients. This exercises
  the server's REST API
- There is a basic command-line client
- There is a basic ncurses-based console client. It is limited to 
  selecting music by album
- The is only very early support for Internet radio streams. There is
  provision in the database for them, and they can be listed in the
  various clients. However, there is at present no way to add new
  streams, except to add them administratively to the database.  (see the
  scripts and documentation in the `radio_support/` directory)

Still to do:

- There are many ways in which the web user interface can be tidied up
- The ncurses client is very basic. Pretty much everything remains to be done.
- It would be nice to have a graphical client for a built-in display on
  embedded systems. A web browser is not a practical interface on such
  a display, and the ncurses interface is rather basic.
- The way that Internet radio is handled needs to be overhauled completely.

## In this repository

This repository has six main directories.

`server/` contains the HTTP server implementation. This has a dependency
on `libvlc`, which is part of the VLC media player.

`api/` contains a C library that encapsulates the REST API, providing
simple functions to play files, query the playlist, etc. It is 
intended to simplify the development of other clients. The API depends
on `libcurl` for its HTTP protocol support.

`client/` contains a command-line client that uses the API library for
communication with a `vlc-server` server instance.

`common/` contains general-purpose functions that are used by all 
the other components, such as string and list manipulation.

`nc-curses-client/` contains the ncurses-based console client.

`radio_support/` documentation on how to use `vlc-server` with Internet
radio stations, and sample scripts for maintaining a station list.

## Building

The programs in this repository are intended to be compiled using `gcc`, on
Linux or a similar platform. There are certain dependencies, in addition to the
compiler, which are documented in `docs/PREREQUISITES.md`.  All of these are in
the usual repositories for mainstream Linux distributions, including for
Raspberry Pi. In a custom Linux installation, some work may be required, to
assemble all the relevant dependencies.  `libvlc`, in particular, has
_hundreds_ of dependencies.

With the prerequisites in place, building should be as simple as
running:

    $ make 
    $ sudo make install

## More information

See the README files in the individual directories for more information.


