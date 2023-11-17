# vlc-server on Raspberry Pi

`vlc-server` and all its supporting bits build fine on Raspberry Pi, and
work tolerably well on Pi versions 3 and later. You'll need a stack of
dependencies to make it build plus, of course, all the usual C build
tooling.

I favour the Pi 3B+ for this application. It's fast enough to be useful,
but can still run completely fanless. Indexing a large media collection is
slow on any Pi, but the speed is worst with USB2 storage devices. 

You can store media on a removeable USB stick, which makes it easy to copy
files from another computer. However, this is generally slower than storing
on the SD card that the Pi boots from.  An external solid-state or magnetic
drive will offer the best storage and speed, but bus-powered drives will
need rather fiddly power-supply arrangements with Pi models earlier than 4.

There is a glaring problem with the Pi, however. At the time of writing,
the latest version of `libvlc` in any mainstream Pi distribution is 3.0.18.
This version has a horrible bug in the FLAC support, that causes audio
dropouts (clicks, pops). There is no workaround that I know of.  The Pi
repository does have 3.0.19, which works fine. However, you'll have to
install this manually, which is a drag. It's not a problem for me, as I
build my own Pi distribution. I hope that regular distros will have this
new VLC version soon.

One of the reasons the Web interface offers a way to browse albums without
cover art, is because serving up all the cover art images from a Pi can be
quite slow. In practice, though, so long as I don't configure `vlc-server`
to show too many albums on a page, it's fast enough, even on a Pi 3.
