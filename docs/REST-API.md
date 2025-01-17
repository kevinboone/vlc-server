# vlc-server REST interface

This document describes the `vlc-server` REST API. All functions of
the API can be exercised using simple `curl` operations, or the
command-line client `vlc-server-client`.

## Background 

The VLC server supports a REST-type HTTP interface. Please note that it is
not encrypted, and there is no security. It should not be run in environments
where security is a concern. 

All HTTP requests may be GET or POST, and have the following form of URL:

http://host:port/api/function/file

where the "/file" part only applies to certain functions.

If the server is able to process the request -- that is, it is syntactically
correct, it will return HTTP response code 200, and a message body in 
JSON format. If it is unable to process the request, the response code
will usually be 400 or 500, and the message body will be an error message
in plain text.

The JSON response will depend on the function being invoked, but all 
responses will begin like this:

    { "status": N, ...}

The status code N will be 0 if the operation completely successfully,
or one of the `API_ERR_XXX` codes defined in 
`api/include/vlc-server/api-common.h` otherwise. 

If the status is non-zero, it will be followed by a text message indicating
the fault:

    { "status": N, "message": "some text" }

Clients should check the status -- the presence of the "message" field does
not, by itself, indicate a fault. Some operations, if successful, will
return

    { "status": 0, "message": "OK" }

In some cases, there may be a more informative message.

## Media root

Some of the API operations take place in the context of a "media root".
In particular, operations that query the local filesystem are all relative
to this root. It is anticipated that local media files will be grouped
under the media root -- multiple media roots are not supported, but
symlinks could be used to create a similar effect.

Operations that take a path/file/URI argument can optionally be specified
relative to the media root, by prepending the "@" character. For example,
the `add` API can use absolute pathnames, or remote URLs -- it doesn't have
to refer to the media root. However, `list-files` and `list-dirs` all
work relative to the media root: we wouldn't want to provide a way that
the server can be abused to allow the complete filesystem to be explored.

The results of a `list-files` operation should be able to be passed
directly to an `add` operation, simply by prepending an @ to the 
values returned.

When a client queries the playlist, anything that can be interpreted
as being relative to the media root -- whether it was added to the playlist
that way or not -- will be replaced by an "@" filename. The intention
here is that the actual filesystem contents are concealed from clients.

Note that none of the above is remotely relevant when playing 
network streams.

The `vlc-server` program includes APIs for listing files because
it is intended for use in remote control applications. The client that
is coordinating the media playback will likely not be on the same host
as the server, and probably will not know what files are available
except by querying the `vlc-server` instance.

## Playlist

Everything played by `vlc-server` is in a playlist, even if it's
a list of only one item. The general procedure for playing
media is:

- (optionally) issue a `clear` operation to clear the playlist
- issue one or more `add` operations to add items to the playlist
- issue the `start` operation. 

All three operations can be combined in an invocation of the `play` API.

## API details

*NOTE: some API function names have the underscore (_) character as*
*a separator, and some have a minus (-). This is for historical reasons,*
*and not easy to change.*

### add

    /api/add/url

Adds a URL or file to the playlist; playback is unchanged. The URL can be a 
local file or a network stream. It can't be a playlist -- the client
is expected to unpack playlist files. Note that the URL must be
URL-encoded, using hexadecimal characters exclusively. That is
"my/file" is "my%2Ffile", and "my file" is "my%20file" (not "my+file").

If the URL begins with the character '@', then it is interpreted as
being relative to the server's media root. Again, this is only meaningful
for local files. The server will block attempts to add files outside
the media root directory by specifying ".." in the URL.

The URL may be a local directory (whether absolute or relative to the
media root), in which case all files in the directory are considered
for addition to the playlist. This operation is not recursive --
only files in the specific, selected directory are added.

If the server is started with a list of accepted file patterns
(using the `--patterns` switch), then files are only added if they
match one of the specified pathnames. This is particulary important
feature if the client application is adding whole directories --
there could well be unplayable files in a directory (cover art, for
example), so indiscriminate adding of a directory's contents should
be avoided. 

### add\_album

    /api/add_album/{album-name}

Adds all the tracks from the specified album to the playlist. This
API function only works if the media database is enabled and the 
media root has been scanned.

### clear

    /api/clear

Clear the playlist. If something is playing, it will contine to play
until the end. The playback will continue from the start of any
new playlist items.

### fullscan

    /api/fullscan

Start a scan of the media root, and a database update. This is a slow scan: 
it's even slower that deleting the whole database and doing a quick
scan. This because each file scanned requires a row of the database
to be modifed, which is slower than appending a row that does not already
exist.

## list-albums

    /api/list-albums?where=[where clause]

List all albums in the database that match the where clause. If there is
no where clause, return a list of all albums.

## list-artists

    /api/list-artists?where=[where clause]

List all artists in the database that match the where clause. If there is
no where clause, return a list of all artists.

## list-composers

    /api/list-composers?where=[where clause]

List all composers in the database that match the where clause. If there is
no where clause, return a list of all composers.

## list-dirs

    /api/list-dirs/[path]

List directories, relative to the media root. See `list-files` for
details. Directory names beginning with "." are not included.

## list-files

    /api/list-files/[path]

Return, in JSON format, a list of files at the specified path. The path
must be URL-encoded. It may, or may not, start with '/'. Since paths
are interpreted relative to the media root, there is no concept of a 
'relative' path. 

If the server has been configured to accept only certain file patterns
for playback, then only files in the directory that match that pattern
will appear in the list.

Hidden files (beginning with '.') are not included.

All results returned are full pathnames, relative to the media root,
but with no initial '/'. 

The format of the JSON response is:

    {"status": 0, "list": [
     "path/to/file1", "path/to/file2", ... ] }

## list-genres

    /api/list-genres?where=[where clause]

List all genres in the database that match the where clause. If there is
no where clause, return a list of all genres.

## list-tracks

    /api/list-tracks?where=[where clause]

List all tracks in the database that match the where clause. If there is
no where clause, return a list of all tracks.

## next

    /api/next

Start playback from the next playlist item. It isn't considered an
error if there is nothing else in the playlist.

## pause

    /api/pause

Pauses playback at the current position. Use `play` to resume -- 
"pause" is not a toggle.

## play

   /api/play/uri

Plays the specified URI immediately. That is, clear the playlist, add
the item, and start playback. The URL might be a local file or folder,
and it might be relative to the media route. See the discussion of 
`/api/add` for more on this point. Note that if the specified URI can't
be played, that won't become apparent until after the current playlist
has been cleared. `/api/play` will stop the current playback and clear
the playlist anyway.

### play\_album

    /api/play_album/{album-name}

Plays the specified album immediately. That is, clear the playlist,
add the tracks from the album, and start playback. This
API function only works if the media database is enabled and the 
media root has been scanned.

### play\_random\_album

    /api/play_random_album

Selects a random album and plays it, clearing any existing playlist.  Note
that, by doing the randomization in the media database, this API call is much
faster then getting a complete list of albums and selecting one of them.

### play\_random\_tracks

    /api/play_random_tracks

Selects 50 (by default) tracks and starts playing them, clearing any existing
playlist.  Note that, by doing the randomization in the media database, this
API call is much faster then getting a complete list of tracks and selecting
some of them. 

### play\_stream

    /api/play_stream/{stream-name}

Plays the specified radio stream immediately. That is, clear the playlist,
add the URI of the stream, and start playback. The argument is the name
of the stream, as stored in the `streams` table in the media database.
To play an arbitrary stream (not known to the media database) 
just add its URI to the playlist using the `add` API function. 

### playlist

Returns the current playlist as a JSON array.

    {"status": 0, "list": [
     "item1", "item2", ... ] }

If the playlist is empty, the "list" field will still be present, but the
array will be empty. The returned items will be full URLs, unless they
appear to be relative to the media root. In that case, the media root
portion of the URL will be replaced by "@". 

### prev 

    /api/prev

Start playback from the previous playlist item. It isn't considered an
error if there is nothing earlier in the playlist. Playback always
goes to the previous item (if there is one) even if the playback position
is only a few seconds into the current one.

### scan

    /api/scan

Start a scan of the media root, and a database update. This is a 'quick' 
scan -- items that are in the database already will not be scanned again.

### shutdown

    /api/shutdown

Shuts down the server, stopping playback immediately.

### start

  /api/start

If playback is stopped, start playing from the start of the playlist.
If paused, resume. If playback is stopped and there is no playlist, 
the JSON response will indicate that an error occurred. 

### stat

    /api/stat

Returns the current playback status, in the following form.

    {"status": 0, 
     "playlist_length": 0, 
     "transport_status": 0, 
     "transport_status_str": "stopped", 
     "position": 0, 
     "duration": 0, 
     "index": -1,
     "volume": 100,
     "mrl": "URL"}

`playlist_length` is the number if items in the playlist which may, of
course, be zero. `transport_status` indicates playing, paused, etc -- one
of the `API_TS_XXX` constants in `api-common.h`. `transport_status_str`
provides a brief textual representation of this status.

`position` is the position of playback in the current track; `duration` is
the length of the current track. Both are in milliseconds. Clients should
be prepared for either or both of these to be "-1" in conditions of
uncertainty (e.g., a network stream).

`index` is the current item in the playlist. This will be -1 if there
is no playlist, or if nothing in the playlist has yet been played.

`volume` is the audio volume, in the range 0-100.

`mrl` is the URL of the current track. This will always be a full URL,
even if a local file was added. Note that "mrl" will be empty, not null,
in the event that nothing is playing. 

Clients should not assume that a non-empty value for "mrl" indicates that
something is playing -- check the "transport\_status" field.

### stop

    /api/stop

Stops playback, and resets the playlist to the beginning. Note 
that the whole playlist is reset, not just the current track.

### storage

    /api/storage

Reports the storage usage of the server, as follows: 

    {"status": 0, 
    "capacity_mb": 511035, 
    "free_mb": 108204, 
    "albums": 110, 
    "tracks": 1789}

The capacity and free space figures are taken from the filesystem that 
contains the media root. Of course there's no easy way for the program
to exclude other files in the same filesystem.

Album and track counts come from the media database -- it would be too slow
to enumerate the media root and count the actual files.

This API always succeeds, and returns a zero status. However, any one of
the individual metrics may be meaningless, depending on the system
status. An error condition is usually indicated by a value of -1. There
will be -1 tracks and albums, for example, if the media database did not
initialize properly. Clients should be careful about working out the
proportion of storage that is free by dividing anything by `capacity_mb`:
it may be zero, because Linux is unable to report the capacity for some
filesystem types.

### volume

    /api/volume/N

Sets the audio volume, nominally in the range 0-100. VNC may interpret
values greater than 100, but 100 is 0dB, so very likely these values will
lead to clipping. 

The volume is not set to any particular level when the server starts --
it will be the VLC default, which is usally full volume.

Note that this command only sets the VLC output volume, and does not affect
the Linux mixer in any way. There could well be additional volume controls
in effect in the system, that cannot be changed remotely.

### volume\_down

    /api/volume_down

Reduces the audio volume in 10% steps.

### volume\_up

    /api/volume_up

Increases the audio volume in 10% steps.

### version 

    /api/version

Returns the server version as a JSON string. 

    {"status": 0, "version": "0.1c"}

Note that there is, at present, no particular format for the version
string. There really isn't much that a client can do, other than
check it matches its own version. If it doesn't, most likely the 
API request would have failed, anyway. Clients might want to display
the server version for troubleshooting purposes.


