# vlc-server server: frequently-asked questions

_What platforms does `vlc-server` run on?_

`vlc-server` is designed to run on Linux systems, particular Raspberry Pi.
However, it builds and runs fine on Ubuntu and Fedora Linux, with the
appropriate dependencies installed.

It is unlikely that this software will run on anything other than Linux, or
that it can be compiled using anything other than GCC.


_What are the runtime dependencies of `vlc-server`?_

VLC, `libmicrohttd`, `libcurl` (probably called `libcurl4-openssl` or
`libcurl4-gnutls` on Ubuntu and similar). VLC is the biggest of these, by far
-- it has many dependencies of its own. On mainstream Linux platforms, the
package manager will take care of installing them.


_Why do I sometimes get skips or dropouts in audio output?_

It must be stressed that `vlc-server` does not play anything -- it just tells
VLC what to play. `vlc-server` is completely at the mercy of VLC in this
respect. VLC has dozens of command-line arguments that might affect playback
quality (run `vlc -H` to see them all). There is a huge amount of discussion of
this subject on web forums. All VLC settings are available to `vlc-server` --
the tricky part if figuring out what to set.

It might be easier to experiment with these settings using the ordinary VLC
graphical application, to find what works best. Of course, it's also worth
ensuring that the latest 3.x version of VLC is installed.  The 3.0.18 version
of VLC has a bug in FLAC playback that cannot be worked around in configuration
-- do not use this version if you mostly play FLAC.

_Does `vlc-server` support 'bit-perfect' audio playback?_

See above -- `vlc-server` supports whatever VLC supports, on a particular
platform. You can avoid unwanted audio tampering by Pulse, et al., by sending
audio output direct to ALSA: see `docs/ALSA.md`.


_What version of VLC is `vlc-server` using?_

`vlc-server` does not embed VLC -- it just uses whatever VLC is available on
the host. So whatever version you discover from VLC itself should be the
version that `vlc-server` uses.  On a Linux system, you should be able to get
the VLC version from the command line:

    $ vlc --version

On all platforms, the VLC user interface, if installed, will be able to provide
this information.


_What system resources does `vlc-server` use?_

When playing audio, `vlc-server` needs about 300Mb RAM. It works fine on a
Raspberry Pi 3 with 1Gb RAM. CPU load depends on the hardware, and ranges from
< 1% on a high-end desktop system, to about 25% on a Raspberry Pi 3.

The installed size of the server on disk is about 2Mb. However, VLC will use a
_lot_ more than that.


_What HTTP port(s) does `vlc-server` use?_

It only uses one port, which provides the web user interface and the REST
interface that other clients might use. The default port number is 30000, but
this can be changed on the `vlc-server` command line.


_How should audio files by organized on disk, to use `vlc-server`?_

`vlc-server` is designed to display and search audio files using metadata
(tags). In most respects it does not care about filesystem layout.

However, `vlc-server` does provide a way to browse directories, and this will
only be effective if there is some sort of structure.  `vlc-server` doesn't
care what it is, however.

The one exception to the notion that `vlc-server` does not care about file
organization is in the use of album cover art.  If you want to display album
cover art, you'll need to ensure that the tracks for each album are in a single
directory for that album. 


_Does `vlc-server` support "baked-in" cover art images?_

To some extent: the media scanner will extract these images if it finds any,
and write them to disk in the same directory as the audio files. The filename
will depend on the image format baked into the file. Again, this only works if
files are organized on an album-per-directory basis.


_What filenames does `vlc-server` use for cover art images?_

All the common names should be supported: `.folder.png`, `cover.gif`, etc. If
there are more than one in the same directory, it's pot luck which one will be
displayed. (Well, it's not random -- it's just not documented.)


_What facilities are offered by the REST API?_

See the separate document `REST-API.md` for full details of the REST interface.


_Does vlc-server support Internet radio?_

VLC server will play any stream that VLC can play. Internet radio stations that
stream using HTTP, IceCast, etc., will usually play. However, `vlc-server` has
no built-in support for Internet radio. Streams can be added to the media
database along with local media, but `vlc-server` provides no way to put them
there. See the `radio-support/` directory for ideas on how to populate the
media database with radio streams.


_How do I select the audio output device?_

Use VLC command-line options: `vlc-server` accepts all the command-line
switches that the `cvlc` player does. So, to set a specific ALSA device:

    vlc-server ... -- -A alsa --alsa-audio-device plughw:CARD=my_card

If you're using Pulse Audio (and I guess somebody has to), this selection can
be made using Pulse controls. See `docs/ALSA.md` for more information.


_How do I do a full scan of the media directory?_

Shut down the server, delete the media database (`vlc-server.sqlite`) and start
a new scan. You can also run `vlc-server-client fullscan`, but it's very slow.
There's really no reason to run a full scan, unless you really, really can't
shut the server down, even for a few minutes. 

_Why is a 'quick' scan so slow?_ 

Even a 'quick' scan requires an exhaustive read of the media directories, and
an exhaustive search of the database. This is because `vlc-server` needs to
know whether a file has been updated (e.g., new metadata) since the last scan. 


_Does the quick scan detect changed tags?_

Only if the timestamp of the file has been updated.


_Does `vlc-server` support gapless playback?_

Alas, no. Nor is it likely to. The client-server architecture of the sofware
makes this very difficult.


