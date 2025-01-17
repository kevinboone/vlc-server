# Prerequisites for building this package

This list of prerequisites is focused on the Raspberry Pi, but probably
applies to most Linux systems. The package names are the same on Debian-based
systems (e.g., Ubuntu), but slightly different of RHEL-like systems.
For Gentoo systems, please see the separate GENTOO.md.

To build `vlc-server` you will need at least the following
dependencies:

- `gcc` 
- `make` 
- `libvlc-dev (a.k.a. `vlc-devel`)
- `libmicrohttpd12` (a.k.a. `libmicrohttpd`) 
- `libmicrohttpd-dev` (a.k.a `libmicrohttp-devel`) 
- `libcurl4-gnutls-dev` (a.k.a. `libcurl-devel`) 
- `libc6-dev` (a.k.a. `glibc-devel`) 
- `binutils-arm-linux-gnueabihf` (Raspberry Pi only) 
- `binutils`
- `libncurses-dev` (a.k.a. `ncurses-devel`) 

The following are likely to be useful, but not essential:

- sqlite3 (for inspecting the media database)
- curl (for simulating HTTP API calls on the server)
- a modern web browser

Some of these packages are likely to be installed by default on 
mainstream Linux distributions. Note that all have sub-dependencies, 
some a huge number.

Note that this application has only been tested with VLC 3.0.x -- it's not
clear that it will even build with 4.0. On RHEL/Fedora, `libvlc-dev` is called
`vlc-devel`. It is not in the main repositories, but it is in RPM Fusion
(rpmfusion.org). Note that there is a bug in VLC 3.0.18 that affects
FLAC playback.

## A note about Gentoo

`vlc-server` builds and runs fine on Gentoo, with the appropriate dependencies.
Gentoo doesn't have specific 'dev' or 'devel' dependencies -- the development
parts are generally included in the main package. An exception is for 
components that are _only_ used for development. So the microhttpd
library is, for example `net-libs/libmicrohttpd`. Of course, you'll need
VLC as well, but there is no specific 'dev' package for it.

If you run an ALSA-only Gentoo system be aware that, if you configure
`vlc-server` to use a specific ALSA hardware device (rather than
just `default` or `plughw`), the server will lock the hardware whilst it
is playing. This will prevent any other application using the same audio
device.

This is true for any ALSA-only Linux, of course, but I'm not aware of any
mainstream Linux that doesn't provide Pulse Audio or Pipewire or similar.
These frameworks create their own problems, but exclusive access to the
audio hardware isn't one of them.






