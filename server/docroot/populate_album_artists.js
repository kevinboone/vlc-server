populate_album_artists_covers = 0;
populate_album_artists_start = 0;
populate_album_artists_where = '';

function load_current_album_artists ()
  {
  var request;
  if (populate_album_artists_where != '')
    request = "/api/list-album-artists?where=" + encodeURIComponent (populate_album_artists_where) 
      + "&count=30&start=" + populate_album_artists_start;
  else
    request = "/api/list-album-artists?count=30&start=" + populate_album_artists_start;
  set_message ("Loading..."); 
  make_fn_request (request, get_album_artists_callback);
  }

function populate_album_artists (where, start, covers)
  {
  var thing_to_scroll = window; 
  thing_to_scroll.addEventListener ('scroll', () => 
    {
    if (window.innerHeight + window.scrollY >= document.body.offsetHeight)
      {
      populate_album_artists_next();
      }
    });

  populate_album_artists_where = where;
  populate_album_artists_covers = covers;
  populate_album_artists_start = start;
  load_current_album_artists();
  }

function populate_album_artists_next ()
  {
  populate_album_artists_start += 30;
  load_current_album_artists();
  }

function add_artist_to_div (response_obj)
  {
  var name = response_obj;
  var escaped_name = encodeURIComponent (name); 
  escaped_name = escaped_name.replaceAll ("'", "''");
  var albums_href = "<a href=\"/gui/albums2?covers=" + populate_album_artists_covers 
    + "&where=album_artist='" + escaped_name +  "'\">[albums]</a>\n";

  var body;
  if (name == "")
    body = albums_href + " [blank]"; 
  else
    body = albums_href + " " + name; 
  var divstart = "<div>\n";

  document.getElementById ("albumartistslist").insertAdjacentHTML 
    ('beforeend', divstart + body + "</div>\n");

  }

function get_album_artists_callback (response_text)
  {
  var obj = eval ('(' + response_text + ')');
  if (obj.status == 0)
    {
    var artist_list = obj.list;
    artist_list.forEach (add_artist_to_div);
    set_message (""); 
    }
  else
    {
    set_message ("Request to get artist list failed"); 
    }
  }


