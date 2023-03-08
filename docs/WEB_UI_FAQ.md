# Web user interface: frequently-asked questions

_Why do the search results pages sometimes show "[blank]" for the name?_

This simply indicates that one or more audio files did not have a 
particular tag. For example, I don't fill in the 'album artist' tag
when ripping CDs where the individual tracks are all by the same artist.
`vlc-server` displays "[blank]" to indicate that the search found
some results, but the relevant tag was missing.

The alternative would be to suppress results where tags are missing,
but sometimes it's helpful to be able to check which items have
missing tags.


_Can I change the number of tracks/folder/artists displayed on a page?_

Not at present through the web interface. This value is set when
starting the server. On a specific page, you can hack the URL in the browser
manually, and add 'count=-1'. However, this won't necessarily be passed
to new pages that are linked from the results. And it's probably not
a good idea.

This is considered a feature, not a limitation. `vlc-server` is designed
for use in embedded Linux systems, where memory may be scarce. With a
large collection, allowing users to display thousands of items on a page
could break the server.


_What's the difference between an 'artist' and an 'album artist'?_

These are completely separate meta-data items (tags) in audio files. Media
player software is generally vague about which tag they use when asked to
display artists.  

My Astell and Kern player, for example, uses 'album artist', by default,
and falls back to 'artist' if the 'album artist' tag is empty.

`vlc-server` display, and can search in, both. It's up to the user to
decide how to use these tags.


_Does vlc-server's web interface create a security hazard?_

Yes, undeniably. It provides no access control or encryption. It is
designed for use in friendly environments, such as within a particular
household. It is absolutely not designed to be exposed to the Internet
or large corporate networks.


_Why is the output quiet, when the volume slider is at 100%?_

`vlc-server` just tells VLC to adjust the volume. What VLC does depends
on how it is built, and on the particular platform. If the platform
has an audio mixer (most do), it might be necessary to use that to
adjust the maximum volume that VLC can provide.


_Can I change the theme, page layout, etc?_

No. The web interface is designed to be completely stateless. It does
not store any information about the user -- no cookies or anything
of that nature. So even if the user could configure the display, there
would be no way to store a particular user's preferences.


_What's the difference between [add] and [play]?_

The [play] link will play the selected item immediately, replacing
any existing playlist. [add] will add the item to the playlist, but
won't necessarily play it.


_What makes a file 'recent'?_

The web user interface has options to show files that have been 
'updated' within a particular time. It's not very clear what
'updated' means in this context. In `vlc-server`, 'updated' refers
to the timestamp of the file on disk. In future, I may add a way to
search by the time the track was added to the database by the scanner,
but this would require storing additional information in the
database.

Many audio file formats have ways to store a date or time. However,
these are in plain text, not recognized date-time formats. If you
use these date/time fields, and do so consistently, you could use
the Custom SQL Query feature to search on them.

_What does 'database is locked' mean?_

Probably the media scanner is running. It will lock the database during
this process. It's also possible that a lock file got left behind, if
the scanner process crashed or was killed. The lock file has the same
name as the media database file, but with `.lock` appended.

