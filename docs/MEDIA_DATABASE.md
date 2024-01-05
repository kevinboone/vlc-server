# vlc-server media database

## Overview

At present, the media database consists of two tables and a bunch
of indexes.

## "files" table

The `files` table is considered to be ephemeral, in that it is usually
built by the media scanner during scanning of the media directories.
No user action other than starting the file scanner can change its
contents.

    path    The media path or URI
    size    size of the medium file in bytes, or 0 if unknown
    mtime   Unix epoch time when the item was added to the database
    title   The title of the item, probably from metadata
    album   The album of the item, probably from metadata
    genre   The genre of the item, probably from metadata
    composer The composer of the item, probably from metadata
    artist   The artist of the item, probably from metadata
    album_artist The album artist of the item, probably from metadata
    track   The track number of the item (as a string, not a number)
    comment Any arbitrary comment
    year    The date found in the item metadata
    exist   A number that is normally 0, set to 1 by the media scanner
              if the corresponding file is found during scanning

`path` may start with a `@`, in which case it is a filesystem location
to the media root directory. If it contains a ':' it is treated as a
URI, which could be to a local file, but more likely a remote stream.
A file that contains a ':' and begins with '=' is a 'permanent' stream,
that is, one that will not be deleted when the media scanner runs.
This feature is currently not used.

All the metadata fields are arbitrary, in that vlc-server does not care
what they are. Of course, they probably ought to make sense to the user.
Note that `year` and `track` are character fields, not numbers -- they
are, in practice, direct copies from tags in the media file, and may 
not actually be numbers.

## "streams" table

The `streams` table stores Internet radio streams. It is envisaged that there
will eventually be a way for users to edit this directly but, at present,
no such facility exists -- it has to be populated by direct action on the
database using `sqlite3`. See the information in the `radio-support/` 
directory for more information.

    name     The display name of the stream (e.g., 'BBC Radio 4')
    location The 2-character country code of the station location
    tags     A comma-separated list of genre tags
    uri      The URI of the stream

At present, only the `name` and `uri` tags are actually used, although
the interfaces may display the others. It's helpful for a user to
know where a stream originates, for example, and what kind of content it
might provide. However, `vlc-server` won't complain if these fields are
empty.



