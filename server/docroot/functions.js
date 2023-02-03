var API_BASE = "/api/"
var transport_status_interval = 5000;
var timeout_msg = 0;

function clear_message ()
  {
  document.getElementById ("messagecell").innerHTML = "";
  timeout_msg = 0;
  }

function cmd_add (path)
  {
  var apiFn = API_BASE + "add/@" + encodeURIComponent (path); 
  make_fn_request (apiFn, response_callback_gen_status);
  }

function cmd_index (number)
  {
  var apiFn = API_BASE + "index/" + encodeURIComponent (number); 
  make_fn_request (apiFn, response_callback_gen_status);
  }

function cmd_next()
  {
  var apiFn = API_BASE + "next?dummy";
  make_fn_request (apiFn, response_callback_gen_status);
  }

function cmd_pause()
  {
  var apiFn = API_BASE + "pause?dummy";
  make_fn_request (apiFn, response_callback_gen_status);
  }

function cmd_play(path)
  {
  var apiFn = API_BASE + "play/@" + encodeURIComponent (path); 
  make_fn_request (apiFn, response_callback_gen_status);
  }

function cmd_prev()
  {
  var apiFn = API_BASE + "prev?dummy";
  make_fn_request (apiFn, response_callback_gen_status);
  }

function cmd_start()
  {
  var apiFn = API_BASE + "start?dummy";
  make_fn_request (apiFn, response_callback_gen_status);
  }

function cmd_stop()
  {
  var apiFn = API_BASE + "stop?dummy";
  make_fn_request (apiFn, response_callback_gen_status);
  }

function do_http_request_complete (callback, http_request)
  {
  if (http_request.readyState == 4)
    {
    //if (http_request.status == 200)
      {
      set_message ("");
      callback (http_request.responseText);
      }
    }
  }

function make_fn_request (fn, callback)
  {
  self_uri = parse_uri (window.location.href);

  fn_uri = "http://" + self_uri.host  + ":" + self_uri.port + fn; 

  make_request (fn_uri, callback);
  }

function make_request (uri, callback)
  {
  var http_request = false;
  if (window.XMLHttpRequest)
    { // Mozilla, Safari, ...              
    http_request = new XMLHttpRequest();
    if (http_request.overrideMimeType)
      {
      http_request.overrideMimeType('text/plain');
      }
    }
  else if (window.ActiveXObject)
    { // IE              
    try
      {
      http_request = new ActiveXObject("Msxml2.XMLHTTP");
      }
    catch (e)
      {
      try
        {
        http_request = new ActiveXObject("Microsoft.XMLHTTP");
        }
      catch (e)
        {}
      }
    }
if (!http_request)
    {
    alert('Giving up :( Cannot create an XMLHTTP instance');
    return false;
    }
  http_request.onreadystatechange = function()
    {
    do_http_request_complete (callback, http_request);
    };
  http_request.open('GET', uri, true);
  http_request.timeout = 10000; 
  http_request.send (null);
  }

function onload_generic()
  {
  var slider = document.getElementById("volumeslider");
  slider.oninput = function() 
    { set_volume (this.value); }
  setInterval (transport_status_tick, transport_status_interval);
  }

function parse_uri (str) {
        var     o   = parse_uri.options,
                m   = o.parser[o.strictMode ? "strict" : "loose"].exec(str),
                uri = {},
                i   = 14;

        while (i--) uri[o.key[i]] = m[i] || "";

        uri[o.q.name] = {};
        uri[o.key[12]].replace(o.q.parser, function ($0, $1, $2) {
                if ($1) uri[o.q.name][$1] = $2;
        });

        return uri;
};

parse_uri.options = {
        strictMode: false,
        key: ["source","protocol","authority","userInfo","user","password","host","port","relative","path","directory","file","query","anchor"],
        q:   {
                name:   "queryKey",
                parser: /(?:^|&)([^&=]*)=?([^&]*)/g
        },
        parser: {
                strict: /^(?:([^:\/?#]+):)?(?:\/\/((?:(([^:@]*)(?::([^:@]*))?)?@)?([^:\/?#]*)(?::(\d*))?))?((((?:[^?#\/]*\/)*)([^?#]*))(?:\?([^#]*))?(?:#(.*))?)/,
                loose:  /^(?:(?![^:@]+:[^:@\/]*@)([^:\/?#.]+):)?(?:\/\/)?((?:(([^:@]*)(?::([^:@]*))?)?@)?([^:\/?#]*)(?::(\d*))?)(((\/(?:[^?#](?![^?#\/]*\.[^?#\/.]+(?:[?#]|$)))*\/?)?([^?#\/]*))(?:\?([^#]*))?(?:#(.*))?)/
        }
};

function refresh_playback_status()
  {
  var apiFn = API_BASE + "stat?dummy";
  make_fn_request (apiFn, response_callback_refresh_playback_status);
  }

function response_callback_gen_status (response_text)
  {
  var obj = eval ('(' + response_text + ')');
  set_message (obj.message);
  }

function response_callback_null (response_text)
  {
  }

function response_callback_refresh_playback_status (response_text)
  {
  var obj = eval ('(' + response_text + ')'); 

  if (obj.status != "0")
    {
    // Should we take some action here? It should never happen
    return;
    }

  if (obj.mrl != "")
    {
    mrlObj = parse_uri (obj.mrl);
    title = mrlObj.file;
    }
  else
    {
    title = "";
    }

  document.getElementById ("transportstatuscell").innerHTML 
    = obj.transport_status_str;
  document.getElementById ("titlecell").innerHTML 
    = title;
  document.getElementById ("positioncell").innerHTML 
    = msec_to_minsec (obj.position);
  document.getElementById ("durationcell").innerHTML 
    = msec_to_minsec (obj.duration);

  index = obj.index;
  if (index >= 0)
    {
    indexStr = "";
    indexStr += "(";
    indexStr += index + 1; 
    pl_len = obj.playlist_length;
    if (pl_len > 0)
      {
      indexStr += " of ";
      indexStr += pl_len; 
      }
    indexStr += ")";
    document.getElementById ("indexcell").innerHTML = indexStr; 
    }
  }

function msec_to_minsec (msec)
  {
  if (msec < 0) msec = 0; 
  totalsec = Math.floor (msec / 1000);
  var min = Math.floor (totalsec / 60);
  var sec = totalsec - min * 60;
  var smin = "" + min;
  if (min < 10) smin = "0" + smin;
  var ssec = "" + sec;
  if (sec < 10) ssec = "0" + ssec;
  return "" + smin + ":" + ssec; 
  }

function set_message (msg)
  {
  document.getElementById ("messagecell").innerHTML = msg;
  if (timeout_msg != 0)
    clearTimeout (timeout_msg);
  timeout_msg = setTimeout (clear_message, 3000);
  }

function set_volume (volume)
  {
  var apiFn = API_BASE + "volume/" + volume;
  make_fn_request (apiFn, response_callback_null);
  }

function transport_status_tick()
  {
  refresh_playback_status();
  }

function url_form_close()
  {
  document.getElementById("urlformwrapper").style.display = "none";
  }

function url_form_open()
  {
  document.getElementById("urlformwrapper").style.display = "block";
  }

function url_form_process()
  {
  url = document.getElementById("url_form_url").value.trim();
  url_form_close();
  var apiFn = API_BASE + "play/" + encodeURIComponent (url); 
  make_fn_request (apiFn, response_callback_gen_status);
  }


