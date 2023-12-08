# vlc-server-client

Version 0.1f, December 2023

## What is this

This is a simple command-line client that can exercise all the functionality
of `vlc-server`. Most the the communications logic, and handling the
REST protocol, is embedded in a stand-alone library whose source is
in the `api/' directory. The actual HTTP operations are carried out by
`libcurl`. 

The client defaults to communicating with a `vlc-server` instance on
the same host, and port 30000, although these properties can be
changed using the `--host` and `--port` switches. 

The basic usage of the client is:

    $ vlc-server-client {options} {command} [arguments]

To get a full list of commands, run

    $ vlc-server-client --help

After installation, you should be able to run `man vlc-server-client`
for more information.

The `add-url` and `play` commands take one or more URLs or files as 
arguments. The difference between these commands is that `add-url` just
adds to the playlist, whilst `play` replaces the playlist with new files.

Both `add-url` and `play` can take a full URL (e.g., an `http://` URL) or
a local file or directory. If a local directory is specified, then
all files in that directory will be added. The server should probably
have been configured so tha non-media files are not accepted into
the playlist -- but this is not under the control of the client.

## Revision history

0.1g December 2023
- added 'fullscan' command

0.1f December 2023
- added 'storage' command

0.1e November 2023
- added 'play-random-XXX' commands

0.1d November 2023
- added 'version' command



