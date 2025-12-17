populate_genres_covers = 0;
populate_genres_start = 0;
populate_genres_where = '';

function load_current_genres ()
  {
  var request;
  if (populate_genres_where != '')
    request = "/api/list-genres?where=" + encodeURIComponent (populate_genres_where) 
      + "&count=30&start=" + populate_genres_start;
  else
    request = "/api/list-genres?count=30&start=" + populate_genres_start;
  set_message ("Loading..."); 
  make_fn_request (request, get_genres_callback);
  }

function populate_genres (where, start, covers)
  {
  var thing_to_scroll = window; 
  thing_to_scroll.addEventListener ('scroll', () => 
    {
    if (window.innerHeight + window.scrollY >= document.body.offsetHeight)
      {
      populate_genres_next();
      }
    });

  populate_genres_where = where;
  populate_genres_covers = covers;
  populate_genres_start = start;
  load_current_genres();
  }

function populate_genres_next ()
  {
  populate_genres_start += 30;
  load_current_genres();
  }

function add_artist_to_div (response_obj)
  {
  var name = response_obj;
  var escaped_name = encodeURIComponent (name); 
  escaped_name = escaped_name.replaceAll ("'", "''");
  var albums_href = "<a href=\"/gui/albums2?covers=" + populate_genres_covers 
    + "&where=genre='" + escaped_name +  "'\">[albums]</a>\n";
  var artists_href = "<a href=\"/gui/artists2?covers=" + populate_genres_covers 
    + "&where=genre='" + escaped_name +  "'\">[artists]</a>\n";
  var album_artists_href = "<a href=\"/gui/album_artists2?covers=" + populate_genres_covers 
    + "&where=genre='" + escaped_name +  "'\">[album artists]</a>\n";
  var composers_href = "<a href=\"/gui/composers2?covers=" + populate_genres_covers 
    + "&where=genre='" + escaped_name +  "'\">[composers]</a>\n";

  var body = albums_href + " " + artists_href + " " + album_artists_href + " "  
    + composers_href + "&nbsp;" + name; 
  var divstart = "<div>\n"

  document.getElementById ("genreslist").insertAdjacentHTML 
    ('beforeend', divstart + body + "</div>\n");

  }

function get_genres_callback (response_text)
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


