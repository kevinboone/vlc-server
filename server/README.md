# vlc-server

`vlc-server` is a simple HTTP server that wraps the VLC media player,
and provides a REST-like interface for clients, as well as a 
rudimentary web interface for use by browsers. Its purpose is to
allow music playback on one Linux computer to be controlled remotely
from some other computer. The "controlling" computer can be anything
that can issue HTTP requests. The intended application is multi-room
audio.

The server was designed to be run on a Raspberry Pi, although it should
build on other Linux-like systems, so long as they have access to the
VLC library.

The REST interface is documented in REST-API.md.

Note that the server is essentially asynchronous -- the client sends
requests that may, or may not, be sucessfully actioned. For example,
the client can add a file to the playlist that VLC cannot play. The
client will never find out about this, because the server cannot check
it until it's time to play it. Of course, there will be an error message
in the server log, but the client will have gone away by then.

The exception to this principle is playback of local files. If an item
is added to the playlist that appears to be a local file, the server
will check that it actually exists, and issue an immediate error
response if it does not. Optionally, the server will also check against
a list of file patterns, so that only particular file types will
be accepted. Again, though, this can only be done on local files --
the server does not know what kind of data a network stream contains,
until it is time to play it.

## Usage

    $ vlc-server [options] -- [VLC options]

## Command-line options

-a, --pattern={patterns}

Sets the list of filename patterns that the server is prepared to play.
The list should be separated by commas, with no spaces. If no pattern
option is given, then all file types are considered to be playable.

For example: `--pattern='*.flac,*.m4a,*.mp3'`

Care should be taken when specifying this argument on the command
line or in a script: the shell may try to expand the patterns 
itself into real filenames.

Note that this option only works for local filenames. A URL,
even a `file://` URL, will always be played, if possible. 

-f, --foreground

Do not go into the background. Log messages are written to the console.
Otherwise the server will detach from the console, and write log message
to the system log. 

-l, --log-level=N

Sets the log verbosity between 0 (catastrophic errors only) and 4
(trace function calls). Log messages are written to the system log
by default; when foreground (`-f`) operation is selected, they are
written to the console.

--list-devices

Lists the known output devices for the default output module
(usually ALSA).

-p, --port=number

Set the HTTP server port; default 30000.

-v, --version

Show the server version.

## Notes

### Setting the output device

vlc-server does not have a built-in way to set where the audio (or video)
output is directed. The libvlc API calls for this don't seem to work --
at least, not in the version's I've tested. However, you can still 
configure this -- and many other things -- by passing VLC arguments
direcly. For example, to set a specific ALSA output channel:

    $ vlc-server ... -- --alsa-audio-device "hdmi:CARD=NVidia,DEV=5" 

For convenience, you can list te output devices on the default
audio plug-in (usually ALSA) using

    $ vlc-server --list-devices 

Note that the "--" on the command line separates the `vlc-server` 
arguments from the VLC arguments.


