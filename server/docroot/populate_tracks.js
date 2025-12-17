populate_tracks_covers = 0;
populate_tracks_start = 0;
populate_tracks_where = '';

function load_current_tracks ()
  {
  var request;
  if (populate_tracks_where != '')
    {
    var escaped_where = populate_tracks_where.replaceAll ("'", "SUBSTME");
    escaped_where = escaped_where.replaceAll ("\b", "SUBSTM2");
    escaped_where = encodeURIComponent (escaped_where); 
    escaped_where = escaped_where.replaceAll ("SUBSTME", "%27"); 
    escaped_where = escaped_where.replaceAll ("SUBSTM2", "%5Cb"); 

    request = "/api/list-tracks2?where=" + escaped_where
      + "&count=30&start=" + populate_tracks_start;
    }
  else
    request = "/api/list-tracks2?count=30&start=" + populate_tracks_start;
  set_message ("Loading..."); 
  make_fn_request (request, get_tracks_callback);
  }

function populate_tracks (where, start, covers)
  {
  var thing_to_scroll = window; 
  thing_to_scroll.addEventListener ('scroll', () => 
    {
    if (window.innerHeight + window.scrollY >= document.body.offsetHeight)
      {
      populate_tracks_next();
      }
    });

  populate_tracks_where = where;
  populate_tracks_covers = covers;
  populate_tracks_start = start;
  load_current_tracks();
  }

function populate_tracks_next ()
  {
  populate_tracks_start += 30;
  load_current_tracks();
  }

function add_track_to_div (response_obj)
  {
  var path = response_obj.path;
  var name = response_obj.name;
  var album = response_obj.album;
  var artist = response_obj.artist; 
  var composer = response_obj.composer; 
  var genre = response_obj.genre; 
  var escaped_path = encodeURIComponent (path); 
  escaped_path = escaped_path.replaceAll ("'", "\\'");
  var sql_album = album.replaceAll ("'", "''");
  var sql_artist = artist.replaceAll ("'", "''");
  var sql_composer = composer.replaceAll ("'", "''");
  var sql_genre = genre.replaceAll ("'", "''");
  var play_href = "<a href=\"javascript:cmd_play('" + escaped_path + "')\">[play]</a>\n";
  var add_href = "<a href=\"javascript:cmd_add('" + escaped_path + "')\">[add]</a>\n";
  var album_href = "<a href=\"/gui/albums2?where=album='" + encodeURIComponent (sql_album) 
         +  "'&covers=" + populate_tracks_covers + "\">" + album + "</a>\n";
  var artist_href = "<a href=\"/gui/albums2?where=artist='" + encodeURIComponent (sql_artist) 
         +  "'&covers=" + populate_tracks_covers + "\">" + artist + "</a>\n";
  var composer_href = "<a href=\"/gui/albums2?where=composer='" + encodeURIComponent (sql_composer) 
         +  "'&covers=" + populate_tracks_covers + "\">" + composer + "</a>\n";
  var genre_href = "<a href=\"/gui/albums2?where=genre='" + encodeURIComponent (sql_genre) 
         +  "'&covers=" + populate_tracks_covers + "\">" + genre + "</a>\n";

  var body =  "<p>" + play_href + " " + add_href 
    + " <span style='display:inline-block;width:15%;vertical-align:top;overflow-wrap: break-word'>" 
    + name + "</span>" 
    + " <span style='display:inline-block;width:15%;vertical-align:top;overflow-wrap: break-word'>" 
    + album_href + "</span>" 
    + " <span style='display:inline-block;width:15%;vertical-align:top;overflow-wrap: break-word'>" 
    + artist_href + "</span>" 
    + " <span style='display:inline-block;width:15%;vertical-align:top;overflow-wrap: break-word'>" 
    + composer_href + "</span>" 
    + " <span style='display:inline-block;width:15%;vertical-align:top;overflow-wrap: break-word'>" 
    + genre_href + "</span>" 
    +  "</p>\n"; 
  var divstart = "<div>\n";

  document.getElementById ("trackslist").insertAdjacentHTML 
    ('beforeend', divstart + body + "</div>\n");
  }

function get_tracks_callback (response_text)
  {
  var obj = eval ('(' + response_text + ')');
  if (obj.status == 0)
    {
    var track_list = obj.list;
    track_list.forEach (add_track_to_div);
    set_message (""); 
    }
  else
    {
    set_message ("Request to get artist list failed"); 
    }
  }


