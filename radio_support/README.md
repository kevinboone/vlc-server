# Internet radio support in vlc-server

As of 0.2a, `vlc-server` has preliminary support for Internet radio streams.
It's preliminary because, although there is provision for streams to
be stored in the media database, and the clients can display and select
them, there is at present no elegant way to add new streams. The only
way to add streams is directly to the database. There isn't even a REST
API call to manage streams yet -- the administrator will need log-in
access to the server host.

The structure of the relevant database table is documented in 
`docs/MEDIA_DATABASE.md`. 

My approach to managing radio streams at present is to maintain them
in a tab-separated text file, and use a script to populate the database
table from this file using `sqlite3`. Of course, to use this approach
we might as well just as well have `vlc-server` just read the station
list from a text file -- but using the media database allows for future
development.

The only metadata stored about radio streams, other than the name and
URL, are a location code and list of genre tags. Neither the location
nor the genre tags are actually used at present -- they just get displayed
by the web interface.

In this directory is a sample station list file `stations.tsv`, which
contains four columns for the four elements we store about streams. 
There is also a simple script `vlc-server-add-station-list.sh` which
parses the text file, and feeds SQL commands into `sqlite3`. So populating
the stream list amounts to

    $ vlc-server-add-station-list stations.tsv /path/to/vlc-server.sqlite

## What works

Assuming there thare are some streams in the media database...

- The `vlc-server-client` command line utility has `list-streams` and
  `play-stream` commands.
- The Web client has a page for streams, that displays the stream names
  and metadata. 
- The ncurses console client has a 'Streams' page which lists all the
  available streams. It looks and works the same as the 'Albums' page.

## Limitations

Apart from the obvious huge problem that there is no simple way to edit
the stream database, there are smaller issues.

- The web interface just dumps the entire stream list on a single page.
  There is no paging or sorting. In practice, because there are no 
  images or icons assocated with streams, this isn't a problem unless there 
  are tens of thousands of streams in the database.
- The web interface's 'search' feature does not find streams, only 
  local files. 
- If a stream won't play -- and that's surprisingly common -- none of the
  clients provide much indication why. You probably won't even get an
  error message, just silence. 


## The general problem with Internet radio 

Internet radio stations are ephemeral. They come and go, almost on a daily
basis. Even the long-lasting ones change their URLs from time to time.  So
manual maintenance of a large list of Internet radio stations is really not
practical. A choice has to be made between importing a large number of stations
from a database like `radio-browser.info`, and accepting that many will be
uninteresting or dysfunctional, or carefully selecting a few stations.

There are commercial stream curators like TuneIn, but their services are
not free. There's no way to integrate a TuneIn search into a free software
application, so far as I can see. The nearest thing to a free radio
stream database is `radio-browser.info`. This is pretty good, and I use
it all the time, but it's a hobby effort, and its maintainers accept
that. It would be interesting to integrate a radio-browser search into
the `vlc-server` web interface, and not all that difficult. But the
long-term future of `radio-browser` cannot be assured (or so it seems to
me), and I don't want to make `vlc-server` dependent on it. 

Any manual station curation will require regular maintenance but, if the
number of stations is small, that's usually not too difficult. In this
directory is a script `vlc-server-get-stations.pl` which demonstrates
how to retrieve and filter the station list from `radio-browser.info`,
and generate a file that can be loaded into the database -- probably
after editing.

This manual approach works fine for me, but I can't see it being useful
for anybody else. I could, in principle, provide a user interface to
add stations manually (along with their metadata). At present, however,
the REST API calls that would need to underly such an operation don't
exist. And, worse, implementing them would require radical changes to
the REST API. At present, the API works with simple HTTP GET requests,
each of which takes a single argument appended to the request URL itself.
Adding station streams would require a generally more complicated
API architecture than currently exists. 

## Other gotchas

Some of the radio station URLs given by radio-browser.info are actually
playlists, containing multiple streams. VLC will usually handle these fine, and
play the first working stream in the playlist. These streams typically have
URLs that end in `.pls`, but not always.  The problem with these streams is in
the extraction of metadata.  If the radio stream URL is in the media database,
then the web interface and console interface will display the metadata (title,
genre, location) that goes with the URL. Unfortunately, this fails if the
stream is a playlist.  This is because VLC will already have parsed the
playlist into the stream it is playing by the time it notifies `vlc-server`
that it has started playing new media. So when we look up the stream URI in the
media database, it isn't there, even when the correct stream is actually
playing.  This doesn't stop anythng playing but, if VLC itself can't provide
any metaata, the display will look odd.

On a related note, I've found that VLC can usually not obtain metadata from a
radio stream, even if it can play it. I don't know why.  Some stations transmit
metadata showing what is actually playing at a particular time, but none of the
`vlc-server` clients can display it, as VLC does not provide it.

If a radio station is not operating, it might simply not respond to
requests (so `vlc-server` will fail to connect to it at all). Or it
might return an error response of some kind. Or it might connect and
transmit silence. There's no real way to know whether the failure is
permanent, or the station will be online again later. That's just the
way Internet radio is.

## Revision history

Janary 2024
- Complete revision, with new database table and API calls
- Updated sample station list
- Corrected BBC station URLs (again)

December 2023
- Updated sample station list
- Corrected BBC station URLs (again)

