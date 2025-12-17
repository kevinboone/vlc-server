# vlc-server on Raspberry Pi

`vlc-server` and all its supporting bits build fine on Raspberry Pi, and
work tolerably well on Pi versions 3 and later. You'll need a stack of
dependencies to make it build plus, of course, all the usual C build
tooling.

I favour the Pi 3B+ for this application. It's fast enough to be useful,
but can still run completely fanless. Indexing a large media collection is
slow on any Pi, but the speed is worst with USB2 storage devices. 

You can store media on a removable USB stick, which makes it easy to copy
files from another computer. However, this is generally slower than storing
on the SD card that the Pi boots from.  An external solid-state or magnetic
drive will offer the best storage and speed, but bus-powered drives will
need rather fiddly power-supply arrangements with Pi models earlier than 4.

A satistfactory build for the Pi needs `libvlc` version 3.0.19 or later. Earlier
versions have a horrible bug in the FLAC support, that causes dropouts
during playback.

One of the reasons the Web interface offers a way to browse albums without
cover art, is because serving up all the cover art images from a Pi can be
quite slow. In practice, though, the new 'infinite-scolling' display 
(since 0.2e) works fine with the default fetch size of 30 records. 
