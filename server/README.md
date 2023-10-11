# vlc-server

Version 0.1c, February 2023

## What is this?

`vlc-server` is an HTTP server that wraps the VLC media player, and provides a
REST-like interface for clients, as well as a rudimentary web interface for use
by web browsers. Its purpose is to allow music playback on one Linux computer
to be controlled remotely from some other computer. The "controlling" computer
can be anything that can issue HTTP requests. The intended application is
multi-room audio.

The server was designed to be run on a Raspberry Pi, although it should build
on other Linux-like systems, so long as they have access to the VLC library.

The REST interface is documented in REST-API.md.

Note that the server is essentially asynchronous -- the client sends requests
that may, or may not, be sucessfully actioned. For example, the client can add
a file to the playlist that VLC cannot play. The client will never find out
about this, because the server cannot check it until it's time to play it. Of
course, there will be an error message in the server log, but the client will
have gone away by then.

The exception to this principle is playback of local files. If an item is added
to the playlist that appears to be a local file, the server will check that it
actually exists, and issue an immediate error response if it does not.
Optionally, the server will also check against a list of file patterns, so that
only particular file types will be accepted. Again, though, this can only be
done on local files -- the server does not know what kind of data a network
stream contains, until it is time to play it.

## Usage

    $ vlc-server [options] -- [VLC options]

## Command-line options

-a, --pattern={patterns}

Sets the list of filename patterns that the server is prepared to play.  The
list should be separated by commas, with no spaces. If no pattern option is
given, then all file types are considered to be playable.

For example: `--pattern='*.flac,*.m4a,*.mp3'`

Care should be taken when specifying this argument on the command line or in a
script: the shell may try to expand the patterns itself into real filenames.

Note that this option only works for local filenames. A URL, even a `file://`
URL, will always be played, if possible. 

If this option is not specified, the default is a list of common audio file
extensions.


-d, --database

Specify the media database file. If not give, the default is the file
`vlc-server.sqlite` in the media root directory. However, this file will not be
created if it does not exist -- the server will run without a media database,
albeit with limited functionality.

Please note that the media database must be writeable, in a writable directory.
The main content directory (the media root) need not be writeable at all; this
is why there is an option to specify the database directory separately.

-e, --entries

The maximum number of items that will be shown in the web interface, on any
given page. This limit applies to albums, tracks, etc., as well as files and
folders.  If there are more items than this, page navigation links will be
displayed.

The default is 30, which might be a little small. The reason for this limit is
self-protection -- all the content on a given page is retrieved in a single
request, with the response being built up in memory. 

-f, --foreground

Do not go into the background. Log messages are written to the console.
Otherwise the server will detach from the console, and write log message to the
system log. Note that this option also applies when `vlc-server` is being run
only to scan and update the media database -- this also would be a background
operation with `-f`. 

-l, --log-level=N

Sets the log verbosity between 0 (catastrophic errors only) and 4 (trace
function calls). Log messages are written to the system log by default; when
foreground (`-f`) operation is selected, they are written to the console.

--list-devices

Lists the known output devices for the default output module (usually ALSA).

-n, --name=string

The name of the server, which is displayed in various places on the web
interface. This setting is intended to distinguish different server instances
in the same installation. The default is the system hostname. 

-p, --port=number

Set the HTTP server port; default 30000.

-q, --quickscan

Start a 'quick' scan of the media root, to update the media database.  In a
quick scan, files are not inspected if they already have entries in the
database. In nearly all cases this is the kind of scan to use.

If the media database does not exist, a 'quick' scan will actually turn into a
'full' scan.

-r, --media-root={directory}

The top-level directory for local audio files. If audio files are in many
disparate directories, it is fine to create a new directory and populate
it with symbolic links to the real file locations. 

-s, --scan

Start a full scan of the media root, to update the media database.  In a full
scan, files are inspected whether they have entries in the database or not. The
only time such a scan is likely to be necessary is if files have been re-tagged
while keeping the same names.

Please note that, on a Raspberry Pi or similar, a full scan is likely to take a
very long time. Both the extraction of metadata, and the database update, are
time-consuming processes.

-v, --version

Show the server version.

## Media database

`vlc-server` maintains a database of metadata of audio files, so audio tracks
may be listed by album, genre, etc. The database is in sqlite3 format, and can
be viewed using sqlite3 utilities if necessary. `vlc-server` will work
without a media database, but it will try to construct one if it can. 
In practice, most functionality requires the database.

The database is constructed using an exhaustive examination of the metadata in
audio files. Currently, the metadata reader supports ID3V2, Vorbis, and MP4 tag
formats. This covers most of the tagging formats in current use, but probably
not all of them.

Once the database has been constructed for the first time, triggering a scan
will typically use 'quick' mode. This mode assumes that files may have been
added or removed, but the metadata of existing files will not have changed.
There is also a 'full' scan mode, which makes no such assumption -- and will be
very slow. At present, the full scan mode is only available by running the
scanner on the command line -- or, of course, by deleting the existing
database.

Constructing the database from scratch will be slow, particularly on
low-powered systems like Raspberry Pi. The Pi only scans about five media files
per second. The server can be used whilst the database is being updated,
but the database itself will not be available. This means that only
file/directory playback will be possible. Even on the Pi, a 'quick' scan
should not be particularly disruptive, unless a lot of media has been
added. 

It is recommended that the media database be constructed
administratively the first time, so its operation can be tracked. Do this using
the `-q` or `-s` switches, and foreground operation (`-f`).  For example:

    vlc-server -q -f -r /path/to/media/directory -d /path/to/database/file

If the server is started without a database, it will construct an empty
one, but it won't scan the filesystem until it is asked to -- either by
the web interface or using a command-line operation.

## Notes

### User privileges

`vlc-server` is designed to run as an unprivileged user. It needs no special
permissions -- a user account that can run VLC will be able to run this server.

Neither the media to be played, nor any directory in which it is contained,
need to be writeable. However, the media database needs to be writeable.  This
is why the `-d` switch is available, to set the media database outside the main
content directory. If this switch is not given, then at least the top-level
directory of the media content needs to be writeable.

Please note also that the media scanner, that updates the database, will try to
extract cover art files from audio files, and will write them into the same
directory as the audio files. These files will be called "cover.XXX", where the
XXX depends on the image type (PNG, JPG...). If the directories are not
writeable, then these cover art files will not be extracted. However, this
won't stop the scanner working.

### Setting the output device

vlc-server does not have a built-in way to set where the audio (or video)
output is directed. The `libvlc` API calls for this don't seem to work -- at
least, not in the versions I've tested. However, you can still configure this
-- and many other things -- by passing VLC arguments direcly. For example, to
set a specific ALSA output channel:

    $ vlc-server ... -- -A alsa --alsa-audio-device hdmi:CARD=NVidia,DEV=5 

Or, perhaps:

    $ vlc-server ... -- -A alsa --alsa-audio-device plughw:CARD=C10,DEV=0

For convenience, you can list the output devices on the default audio plug-in
(usually ALSA) using

    $ vlc-server --list-devices 

Note that the "--" on the command line separates the `vlc-server` arguments
from the VLC arguments.

This isn't the place to go into the details of ALSA configuration, much
less Pulse, which is a nightmare to configure to get optimal sound quality.
Some systems that use Pulse do not allow ALSA to be used directly at all,
so there's no point trying to set ALSA configuration. I tend to believe
that, if you can use ALSA directly, you probably should. ALSA will still
fiddle about this sample rates, etc., but at least you'll be able to
predict how it does it.

If you're using ALSA and aren't hugely fussed about subtle issues of
audio quality, you should probably use an ALSA device whose names
starts with "plughw". This will enable ALSA's conversion plugins. These
aren't _necessarily_ bad conversions. For example, my DACMagic USB DAC
only supports 24-bit samples. With 16-bit CD rips I get exactly the same 
output if I use the `plughw` interface and let ALSA fill in the missing
8 bits, as if I do this conversion manually when I rip the CD. 

Of course, using `plughw` enables _bad_ conversions as well. If the
audio hardware is set up to allow only 16-bit samples, and you play
a 24-bit file, ALSA will silently downsample.

In short, if you care about optimal sound quality, you really have to
understand how ALSA (or Pulse) works, and know the capabilities of
your audio hardware. 

Incidentally, I've had problems on systems that use PipeWire -- the
ALSA device cannot be opened, with a message like this: 

    ERROR The audio device "iec958:CARD=I20,DEV=0" could not be used:
    Device or resource busy.
    ERROR module not functional

Killing the `pipewire-pulse` process fixes this, at least in the short
term. It just gets restarted :/

### Volume control

The web interface exposes a volume control, as does the REST API. This volume
control affects only VLC's output level -- whether it applies a system-level
volume change depends on how VLC is set up.

### Memory leaks

I think `libvlc` is a little leaky. Tools like `valgrind` do show a fair amount
of memory that is not freed when the server quits. So long as this amount does
not increase over time -- and it seems not too -- I guess this is something we
have to live with. 

### Sorting search results

Metadata results page (artists, albums,...) are sorted in alphabetical order of
the field. That is, the 'artists' page is sorted by artist.  The except is the
tracks page, whether obtained by expanding an album or in some other way. This
page is sorted by track number first, and then by filename if there are no
track numbers. 

This is not ideal -- not everybody uses track tags and, even when people do,
the track names are often inconsistent. In this respect, `vlc-server` works in
the same way as every other media player I know, and it's not easy to implement
a better approach with the kind of intelligence provided by the `sqlite3`
database engine.

### Media organization 

`vlc-server` really depends on audio files being properly tagged. That is,
all the tracks in an album should have the same album tag; all tracks by
the same artist should have the same 'artist' tag, and so on. The closer
the media is to being rigorously tagged, the better the server will work.

`vlc-server` can browse specific files and folders, but being able to do
this really depends on the files being grouped meaningfully into a 
directory heirarchy. However, when working with metadata (tags), `vlc-server`
doesn't care about filesystem organization -- except in one instance.

That one instance is that proper handling of cover art depends on the 
tracks from each album being collected into a single directory. That is
because each directory can only have one cover-art image file.

### Cover art issues

Cover images will be displayed, if they are available, on the Albums or
Files pages. The file scanner will extract cover art from various audio
file types, and store it as local files. For reasons of efficiency, the
server will not attempt to read cover art from audio files at runtime.

Any of the usual filenames -- <code>folder.png</code>, <code>cover.png</code>,
etc -- can be used for a cover art image, if you want to install them
manually. Be aware, however, that there's little point installing an
image larger than 64x64 pixels. That is the size that the cover art images
are displayed on web pages; if they are larger, the browser will have
to scale them down. And, of course, transferring a larger image from
the server to the browser takes longer. 


