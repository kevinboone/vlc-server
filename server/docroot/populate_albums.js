populate_albums_covers = 0;
populate_albums_start = 0;
populate_albums_where = '';

function load_current_albums ()
  {
  var request;
  if (populate_albums_where != '')
    {
    var escaped_where = populate_albums_where.replaceAll ("'", "SUBSTME");

    escaped_where = encodeURIComponent (escaped_where); 
    escaped_where = escaped_where.replaceAll ("SUBSTME", "%27"); 

    request = "/api/list-albums2?where=" + escaped_where
      + "&count=30&start=" + populate_albums_start;
    }
  else
    request = "/api/list-albums2?count=30&start=" + populate_albums_start;
  set_message ("Loading..."); 
  make_fn_request (request, get_albums_callback);
  }

function populate_albums (where, start, covers)
  {
  var thing_to_scroll = window; 
  thing_to_scroll.addEventListener ('scroll', () => 
    {
    if (window.innerHeight + window.scrollY >= document.body.offsetHeight)
      {
      populate_albums_next();
      }
    });

  populate_albums_where = where;
  populate_albums_covers = covers;
  populate_albums_start = start;
  load_current_albums();
  }

function populate_albums_next ()
  {
  populate_albums_start += 30;
  load_current_albums();
  }

function add_album_to_div (response_obj)
  {
  var name = response_obj.name;
  var dir = response_obj.cover;

  var escaped_name = encodeURIComponent (name); 
  escaped_name = escaped_name.replaceAll ("'", "\\'");
  var sql_name = name.replaceAll ( "'", "''");

/*
  var escaped_name = escaped_name.replaceAll ( "'", "SUBSTME");
  escaped_name = encodeURIComponent (escaped_name); 
  escaped_name = escaped_name.replaceAll ("SUBSTME", "%27"); 
*/

  var escaped_dir = dir.replaceAll ("'", "SUBSTME"); 
  escaped_dir = encodeURIComponent (escaped_dir); 
  escaped_dir = escaped_dir.replaceAll ("SUBSTME", "%27"); 

  var play_href = "<a href=\"javascript:cmd_play_album('" + escaped_name + "')\">[play]</a>\n"; 
  var add_href = "<a href=\"javascript:cmd_add_album('" + escaped_name + "')\">[add]</a>\n"; 
  var show_href = "<a href=\"/gui/tracks2?where=album='" + sql_name +  "'&covers=" 
    + populate_albums_covers + "\">[show]</a>\n";

  var img_html = "";
  if (populate_albums_covers != "0" && escaped_dir != null)
    {
    img_html = "<img class='coverimg' src='/cover/" + escaped_dir + "'/>";
    }

  var body = img_html + play_href + " " + add_href + " " + show_href + " " + name; 
  var divstart = "<div>\n";

  if (populate_albums_covers != "0")
    {
    divstart = "<div class='dircell_cover'>\n";
    }

  document.getElementById ("albumlist").insertAdjacentHTML 
    ('beforeend', divstart + body + "</div>\n");

  }

function get_albums_callback (response_text)
  {
  var obj = eval ('(' + response_text + ')');
  if (obj.status == 0)
    {
    var album_list = obj.list;
    album_list.forEach (add_album_to_div);
    set_message (""); 
    }
  else
    {
    set_message ("Request to get album list failed"); 
    }
  }


