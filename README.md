# vlc-server

** This is a work in progress! **
** (And it's likely to remain that way for some time) **

`vlc-server` is a simple HTTP server that wraps the VLC media player, and
provides an HTTP interface. There is a rudimentary web interface that can be
used with a browser, and also a REST-like API that can be used by other kinds
of client. The web interface just displays local folders that contain audio,
and allows files or folders to be added to a playlist.

The purpose of `vlc-server` is allow media playback on one Linux computer to be
controlled remotely from some other computer. The "controlling" computer can be
anything that can issue HTTP requests.

`vlc-server` is intended, eventually, to be a replacement for `xine-server`:

https://github.com/kevinboone/xine-server

At present, `xine-server` is much more sophisticated, with full meta-data
indexing, Internet radio support, various console clients, etc. However,
the Xine player is -- so far as I can see -- currently unmaintained. It no longer
works on some platforms where it used to work fine. 
In the longer term, I intend to move all the functionality from `xine-server`
to `vlc-server`, but this is a colossal undertaking, and something that I
won't start until I know that VLC is basically workable when used in a 
server process.

`vlc-server` notionally supports video.  However, since the purpose of the
utility is to allow for remote control, there seems to be little purpose in
using it to play video. The server can optionally be provided with a list of
filename patterns that it will allow to play (e.g., `*.flac,*.mp3`).

The package also contains a simple command-line client for Linux, that can
exercise all the functionality of the server.

I have tested this software on Fedora and Ubuntu, as well as Raspberry Pi.

## In this repository

This repository has four main directories.

`server/` contains the actual server implementation. This has a dependency
on `libvlc`, which is part of the VLC media player.

`api/` contains a C library that encapsulates the REST API, providing
simple functions to play files, query the playlist, etc. It is 
intended to simplify the development of other clients. The API depends
on `libcurl` for its HTTP protocol support.

`client/` contains a command-line client that uses the API library for
communication with a `vlc-server` server instance.

`common/` contains general-purpose functions that are used by all 
the other components, such as string and list manipulation.

## Building

The programs in this repository are intended to be compiled using 
`gcc`, on Linux or a similar platform. There are certain dependencies,
in addition to the compiler, which are documented in `docs/PREREQUISITES.md`.
All of these are in the usual repositories for mainstream Linux
distributions, including for Raspberry Pi. In a custom Linux installation,
some work may be required, to assemble all the relevant dependencies.
`libvlc`, in particular, has _hundreds_ of dependencies.

With the prerequisites in place, building should be as simple as
running:

    $ make 
    $ sudo make install

## More information

See the README files in the individual directories for more information.


