# Internet radio support in vlc-server

`vlc-server` has no specific support for Internet radio. However, Internet
radio streams can be added to the media database along with local files. In the
`files` table, the `path` column usually indicates a file relative to the media
root. However, any `path` that begins with `=` is assumed to be literal, and is
passed unchanged to VLC when the corresponding item is played.

The '=' sign has another significance: the media scanner usually deletes
entries in the database that have no corresponding local files, assuming that
the files have been deleted. However, entries whose paths begin '=' are not
automatically deleted. 

Paths that begin with '=' have a specific meaning to the web UI as well: these
streams have an specific, 'radio tower' icon in the album list.  Normally the
album list will show cover art from the local folder, when playing local files.
However, streams do not have a local folder, hence the specific icon. 

So, with patience, Internet radio streams could be added to the database. You
could use the sqlite3 command-line tool, for example, to add whatever entries
you like. So long as the `path` column begins with '=' it should play.

However, such an entry will be hard to find, and have no metadata that will
allow it to be grouped.

## Structuring stations in the database

I can think of two basic ways to structure streams in the database, depending
on the level of automation required, and the number of streams to be added.

1. Group a lot of 'similar' stations into a specific 'album', or
2. create an 'album' for each stream. 

Of course, these aren't real albums -- just a way to group similar content.
'Similar' in this context is not well-defined -- radio stations might be
'similar' if they based in the same location, or broadcast the same genre of
music or speech, or something else.

### Manually adding stations from a list 

For a small number of stations, I find it easiest just to maintain a list in a
text file, and use a script to add them to the database. Of course, this
approach only works if the stations have relatively stable URLs -- and many do
not. It's also only practical for a relatively small number of streams.

With this approach, I create each stream as its own 'album'. So, in the
database, the 'title' field is the name of the stream, and the 'album' field
will be the same, but perhaps with 'Radio station -' prepended to the name.
Using a prefix like this makes it easy to find stations in the album list.

Making each station its own album is particularly useful for players like
`nc-vlc-client`, which can only play content by album. In the source bundle
there is a sample station list `stations.tsv`, and a Perl script
`vlc-server-add-station-list.pl` that reads that list into the database. There's
also a shell scrip `vlc-server-add-station-list.sh` that reads a station
list file and outputs an SQL file that can be fed into, for example,
`sqlite3`. This script is included for emedded systems that don't have
Perl. 

However the stations are added, it's entirely possible, however that the
stations in `stations.tsv` are no longer broadcasting, or have changed their
URLs -- that's the general problem with Internet radio. `stations.tsv` is
included as an example of how to structure a station list -- there's no
guarantee that any of the stations are actually working.

It's possible to use large albums of stations with `nc-vlc-client`, but it's
ugly -- select the album from the console interface, which will start playing
the first station URL in the album. Then change to the Playlist tab, and select
the specific station required.

### Automatically adding large numbers of stations

Here each 'album' might consist of 100-200 stations, so it wouldn't be played
like a regular album. But it would appear in the list of albums, which could
expanded into separate tracks (stations, in this case). If the album name
contains a clear identifier, like 'XXX radio stations', then the album would
show up in a search for 'radio stations', as well as in the album list.

But where do we get the large number of stations from, in the first place?

Commercial Internet radio products generally use one of the proprietary
databases of stations. These, sadly, are not free to use. Fortunately, the
volunteer-maintained database at `radio-browser.info` _is_ free to use.
Moreover, it will supply its entire database in a handy XML format, with one
line per station. There are problems with this resource, which I will outline
later; but generally I've found it quite useful.

The method I use to maintain large numbers of Internet radio stations
automatically is to use a Perl script that does the following:

- Downloads the entire station database from `radio-browser.info`
- Filters it according to various criteria that suit me, to identify
  stations that I want in my media database
- Turns each station's data into an SQL insert statement, to store the
  details in the `vlc-server` media database
- Executes all the SQL using `sqlite3`.

The script `vlc-server-add-stations.pl` is an example of how to do this, but
it's only an example. The tricky part is working out how to get the stations
you want, without getting thousands that you don't. The `radio-browser` list
currently runs to ~35,000 stations, of which more than 8,000 are
English-language. It's certainly possible to load many thousands of stations
into the `vlc-server` database, but that would make it essentially an Internet
radio player -- you wouldn't be able to find anything _except_ radio stations.

The example script includes only stations that have a UK location, and
broadcast only in English. There are about 400 of these, which is a manageable
number. There are a few US and Canadian stations I like to listen to but,
frankly, it's easier to add them manually to the database, than to identify
search criteria that will find the specific stations I like.

The `radio-browser` database does not have any consistent organizing structure.
It does store the location and language of a station, although these fields are
filled in somewhat inconsistently. It also stores 'tags', and each station can
have more than one tag. The tags are sometimes genres, like 'rock', and
sometimes more nebulous, like 'public radio'. These tags are also not used
completely consistently -- but they are really the only way to identify content
that might be interesting, other than language perhaps. 

Another attribute that is of profound importance in the `radio-browser` list is
`lastcheckok`. This attribute is set to '1' if the station has been tested in
the last few days.  

Because my Perl script scans the full station list several times, looking for a
different tag each time, some stations may be found more than once.  This leads
to error messages like this:

    Error: near line 420: UNIQUE constraint failed: files.path

because the database won't access the same `path` in more than one database
row. But this isn't a problem, since these are duplicate stations.

## The general problem with Internet radio 

Internet radio stations are ephemeral. They come and go, almost on a daily
basis. Even the long-lasting ones change their URLs from time to time.  So
manual maintenance of a large list of Internet radio stations is really not
practical. A choice has to be made between importing a large number of stations
from a database like `radio-browser.info`, and accepting that many will be
uninteresting or dysfunctional, or carefully selecting a few stations.

Any manual station selection will require regular maintenance but, if the
number of stations is small, that's usually not too difficult.

## Gotchas

Some of the radio station URLs given by radio-browser.info are actually
playlists, containing multiple streams. VLC will usually handle
these fine, and play the first working stream in the playlist. These
streams typically have URLs that end in `.pls`, but not always.
The problem with these streams is in the extraction of metadata. 
If the radio URL is in the media database, then the web interface and
console interface will display the metadata (genre, title) that goes
with the URL. Unfortunately, this fails if the stream is a playlist.
This is because VLC will already have parsed the playlist into the
stream it is playing by the time it notifies `vlc-server` that it has
started playing new media. This doesn't stop anythng working but,
if VLC itself can't provide any metaata, the display will look odd.

On a related note, I've found that VLC can usually not obtain metadata
from a radio stream, even if it can play it. I don't know why.


## Revision history

December 2023
- Updated sample station list
- Corrected BBC station URLs (again)

