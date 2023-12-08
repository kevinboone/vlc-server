# Building and installing vlc-server

`vlc-server` is designed to be used in an embedded Linux system. As such,
it is impossible to give general installation instructions. These
instructions describe building and installing on a desktop, or desktop-like,
system. Naturally, they will need to be adapted.

1. Assemble the pre-requisites. There are many: please see
   `docs/PREREQUISITES.md` for a full list.

2. Unpack the source bundle into some convenient directory.

3. Build and install:

    $ make
    $ sudo make install

4. Assemble some music files. `vlc-server` assumes that all music files are
   in the same top-level directory. Symbolic links can be used to collect
   multiple directories together, if required.

5. Start the `libvlc-server`, in the foreground for testing purposes:

    $ vlc-server -f -r /home/kevin/Music

    INFO Media database not set; defaulting to /home/kevin/Music/vlc-server-db.sqlite
    INFO Creating an empty media database
    INFO Creating database tables
    INFO Starting HTTP server on port 30000
    INFO Audio output device is default

*Do not run the server as root*.

A `sqlite3` database will be created in the specific media directory. It
will be called `vlc-server-db.sqlite` by default. If the media directory
is not writeable, you can use the `-d` switch to put the database in a 
different place.

If port 30000 is not available, use the `-p` switch to choose a different
port. 

   You should now be able to connect a web browser to port 30000, or run
   the console client `nc-vlc-client`. The latter takes host and port 
   arguments, if you aren't running it on the same machine as the server.

6. Scan the music files for metadata.
   Navigate either the browser interface or the console interface to find
   the option to scan the media directory. On the console interface it's

    Control -> Scan

   At this stage it doesn't matter whether you choose a 'full' scan or a
   normal scan -- the database is empty. The web interface at present only
   allows a normal scan.

   The scan could take a long time, if there are many music files.

   At the command line you can check progress by running

    $ vlc-server-client stat

   This will tell you how many files have been scanned. When it stops saying
   this, it's finished. The web interface and the console client also report
   file scanner progress, on at least some pages.
 
7. Check that sound is generated

   Using any of the client interfaces, try to play some music. Depending
   on how your system is set up, you might need to tell the server which
   audio device to use. See `docs/ALSA.md` for guidance on this point.

8. Arrange for the server to start automatically.

   It's not possible to give instructions here. See `docs/EMBEDDED.md`
   for suggestions how to do this in an embedded system.  
    




