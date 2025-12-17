populate_artists_covers = 0;
populate_artists_start = 0;
populate_artists_where = '';

function load_current_artists ()
  {
  var request;
  if (populate_artists_where != '')
    request = "/api/list-artists?where=" + encodeURIComponent (populate_artists_where) 
      + "&count=30&start=" + populate_artists_start;
  else
    request = "/api/list-artists?count=30&start=" + populate_artists_start;
  set_message ("Loading..."); 
  make_fn_request (request, get_artists_callback);
  }

function populate_artists (where, start, covers)
  {
  var thing_to_scroll = window; 
  thing_to_scroll.addEventListener ('scroll', () => 
    {
    if (window.innerHeight + window.scrollY >= document.body.offsetHeight)
      {
      populate_artists_next();
      }
    });

  populate_artists_where = where;
  populate_artists_covers = covers;
  populate_artists_start = start;
  load_current_artists();
  }

function populate_artists_next ()
  {
  populate_artists_start += 30;
  load_current_artists();
  }

function add_artist_to_div (response_obj)
  {
  var name = response_obj;
  var escaped_name = encodeURIComponent (name); 
  escaped_name = escaped_name.replaceAll ("'", "''");
  var albums_href = "<a href=\"/gui/albums2?covers=" + populate_artists_covers 
    + "&where=artist='" + escaped_name +  "'\">[albums]</a>\n";

  var body = albums_href + " " + name; 
  var divstart = "<div>\n";

  document.getElementById ("artistslist").insertAdjacentHTML 
    ('beforeend', divstart + body + "</div>\n");

  }

function get_artists_callback (response_text)
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


