# libvlc-server-client

Version 0.2b, February 2023

## What is this

This is a simple command-line client that can exercise all the functionality
of `libvlc-server`. Most the the communications logic, and handling the
REST protocol, is embedded in a stand-alone library whose source is
in the `api/' directory. The actual HTTP operations are carried out by
`libcurl`. 

The client defaults to communicating with a `libvlc-server` instance on
the same host, and port 30000, although these properties can be
changed using the `--host` and `--port` switches. 

The basic usage of the client is:

    $ libvlc-server-client {options} {command} [arguments]

To get a full list of commands, run

    $ libvlc-server-client --help

The `add-url` and `play` commands take one or more URLs or files as 
arguments. The difference between these commands is that `add-url` just
adds to the playlist, whilst `play` replaces the playlist with new files.

Both `add-url` and `play` can take a full URL (e.g., an `http://` URL) or
a local file or directory. If a local directory is specified, then
all files in that directory will be added. The server should probably
have been configured so tha non-media files are not accepted into
the playlist -- but this is not under the control of the client.

