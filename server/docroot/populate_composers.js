populate_composers_covers = 0;
populate_composers_start = 0;
populate_composers_where = '';

function load_current_composers ()
  {
  var request;
  if (populate_composers_where != '')
    request = "/api/list-composers?where=" + encodeURIComponent (populate_composers_where) 
      + "&count=30&start=" + populate_composers_start;
  else
    request = "/api/list-composers?count=30&start=" + populate_composers_start;
  set_message ("Loading..."); 
  make_fn_request (request, get_composers_callback);
  }

function populate_composers (where, start, covers)
  {
  var thing_to_scroll = window; 
  thing_to_scroll.addEventListener ('scroll', () => 
    {
    if (window.innerHeight + window.scrollY >= document.body.offsetHeight)
      {
      populate_composers_next();
      }
    });

  populate_composers_where = where;
  populate_composers_covers = covers;
  populate_composers_start = start;
  load_current_composers();
  }

function populate_composers_next ()
  {
  populate_composers_start += 30;
  load_current_composers();
  }

function add_artist_to_div (response_obj)
  {
  var name = response_obj;
  var escaped_name = encodeURIComponent (name); 
  escaped_name = escaped_name.replaceAll ("'", "''");
  var albums_href = "<a href=\"/gui/albums2?covers=" + populate_composers_covers 
    + "&where=composer='" + escaped_name +  "'\">[albums]</a>\n";

  if (name == "")
    body = albums_href + " [blank]"; 
  else
    body = albums_href + " " + name; 

  var divstart = "<div>\n";

  document.getElementById ("composerslist").insertAdjacentHTML 
    ('beforeend', divstart + body + "</div>\n");

  }

function get_composers_callback (response_text)
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


