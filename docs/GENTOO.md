# Running vlc-server on Gentoo

`vlc-server` builds and runs fine on Gentoo, but there are a couple of
things to bear in mind.

## Dependencies

You'll need to `emerge` at least the following dependencies:

    media-video/vlc
    net-libs/libmicrohttpd
    net-misc/curl

Compiler and build tools go without saying on Gentoo ;) `vlc` and its
dependencies take hours to install on Gentoo, if they have to be built from
source.

## Oddities

You'll need to add the `tinfow` library:

    EXTRA_LDFLAGS=-ltinfow make

This only affects the ncurses client, but it won't hurt to add it to
the other builds (they don't use it).

By default, the Gentoo build of ncurses(w) doesn't include libtinfo(w).
You'll have to use the seperate library. Note that if you use 
ncurses with libtinfo (not libtinfow) then ncurses will segfault at
initialization.

I didn't make this change in the distribution, because other Linux systems
don't seem to have a separate libtinfow.


