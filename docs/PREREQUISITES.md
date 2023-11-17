# Prerequisites for building this package

`gcc` and `make`, of course.

LibVLC development headers and libraries -- `vlc-devel` (`libvlc-dev`).
Note that this application has only been tested with VLC 3.0.x -- it's 
not clear that it will even build with 4.0. On Fedora, vlc-devel is
not in the main repositories, but it is in RPM Fusion (rpmfusion.org).

MicroHTTPD development headers and libraries -- 
`libmicrohttpd-devel` (`microhttp-dev`)

Curl development libraries and headers -- 
libcurl4-openssl-dev or `libcurl4-gnutls-dev` (`libcurl-devel`)

The command-line `curl` utility will probably be helpful for testing.


