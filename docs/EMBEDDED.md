# Embedded set-up

My Raspberry Pi set-up uses a custom Linux, that starts a minimum of services
and then does an auto-login for a specific user -- the only user that has an
account. That user gets a session on psueo-terminal `/dev/tty2`, for reasons
that are not important here.

The (only) user's `.bashrc` file starts the server and the ncurses client.
Another possibility would be to start an X session and a web browser, and run
the web UI. However, the ncurses client takes no time at all to start.

So this is what the `.bashrc` file looks like:

    TTY=`tty`
    if [[ $TTY =~ "tty2" ]] ; then
      vlc-server -r /path/to/media/root -- \
        -A alsa --alsa-audio-device plughw:CARD=C10,DEV=0 &
      exec nc-vlc-client -k
    fi

The test for `tty2` is to ensure that the server and client are only started on
a single virtual console -- `tty2`.  I can still `ssh` to the Pi for
maintenance as the single defined user. With an `ssh` login, `tty` outputs a
string with `pts` in the name, rather than `tty`. Similarly, I can use
Alt+Function keys to get a shell on a different virtual terminal.

I run the ncurses client in kiosk ('-k') mode so the user cannot accidentally
log out. However, if the user kills the client using ctrl-C, `init` will just
start a new session on the same virtual console, and so start the ncurses
client again. 
 
I'm sure a more complex system would need a more elaborate start-up process.

