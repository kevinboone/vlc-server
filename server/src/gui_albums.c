/*======================================================================
  
  vlc-server

  server/src/gui_albums.c

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <microhttpd.h>
#include <vlc-server/vs_log.h> 
#include <vlc-server/vs_util.h> 
#include "template_manager.h"
#include "http_util.h"
#include "gui.h"
#include "gui_albums.h"
#include "media_database.h"

/*======================================================================
  gui_albums_album_cell
======================================================================*/
void gui_albums_album_cell (VSString *body, const char *album, 
       BOOL covers, MediaDatabase *mdb)
  {
  if (covers)
    vs_string_append (body, "<div class=\"dircell_cover\">");
  else
    vs_string_append (body, "<div class=\"dircell_nocover\">");

  if (covers)
    {
    vs_string_append (body, "<img class=\"coverimg\" src=\"/cover/");
    char *dir= api_get_dir_for_album (mdb, album);
    if (dir)
      {
      VSString *enc_dir = vs_string_encode_url (dir); 
      vs_string_append (body, vs_string_cstr (enc_dir));
      vs_string_destroy (enc_dir);
      free (dir);
      }
    else
      {
      vs_string_append (body, "unknown");
      }
    vs_string_append (body, "\"/>\n");
    }

  char *enc2 = media_database_escape_sql (album);
  VSString *enc_album = http_util_encode_for_js (enc2); 

  vs_string_append (body, "<a href=\"javascript:cmd_add_album('");
  vs_string_append (body, vs_string_cstr (enc_album)); 
  vs_string_append (body, "\')\">[add]</a>\n");

  vs_string_append (body, "<a href=\"javascript:cmd_play_album('");
  vs_string_append (body, vs_string_cstr (enc_album)); 
  vs_string_append (body, "\')\">[play]</a>\n");

  vs_string_append (body, "<a href=\"/gui/tracks?where=album='");
  vs_string_append (body, enc2);
  vs_string_append (body, "\'");
  if (covers)
    vs_string_append (body, "&covers=1");
  else
    vs_string_append (body, "&covers=0");
    
  vs_string_append (body, "\">[show]</a>\n");

  vs_string_destroy (enc_album);
  free (enc2);

  if (album[0])
    vs_string_append (body, album);
  else
    vs_string_append (body, "[blank]");

  vs_string_append (body, "</div>\n");
  vs_string_append (body, "</div>\n");
  }


/*======================================================================
  gui_albums_get_body
======================================================================*/
VSString *gui_albums_get_body (const char *path, 
            const VSProps *arguments, int count, MediaDatabase *mdb)
  {
  return gui_get_results_page (path, arguments, count, mdb, "albums",  
    MDB_COL_ALBUM, TRUE, gui_albums_album_cell, FALSE);
  }


