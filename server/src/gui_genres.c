/*======================================================================
  
  vlc-server

  server/src/gui_tracks.c

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
#include "gui_genres.h"
#include "media_database.h"

#define SAFE(x)((x) ? (x) : "")

/*======================================================================
  gui_genres_genre_cell
======================================================================*/
void gui_genres_genre_cell (VSString *body, const char *genre, 
       BOOL covers, MediaDatabase *mdb)
  {
  (void)mdb;
  vs_string_append (body, "<tr>");

  vs_string_append (body, "<td>");
  vs_string_append (body, genre); 
  vs_string_append (body, "</td>");

  vs_string_append (body, "<td>\n");

  char *enc2 = media_database_escape_sql (genre);
  VSString *enc3 = vs_string_encode_url (enc2);

  char *where;
  asprintf (&where, "genre='%s'", vs_string_cstr (enc3));
  free (enc2);
  vs_string_destroy (enc3);

  char *album_href;
    asprintf (&album_href, 
      "<a href=\"/gui/albums?where=%s&covers=%d\">[albums]</a>", where, 
        covers);

  vs_string_append (body, album_href);
  free (album_href);
  vs_string_append (body, "</td>\n");

  vs_string_append (body, "<td>\n");
  char *artist_href;
    asprintf (&artist_href, 
      "<a href=\"/gui/artists?where=%s&covers=%d\">[artists]</a>", where, 
        covers);

  vs_string_append (body, artist_href);
  free (artist_href);
  vs_string_append (body, "</td>\n");

  vs_string_append (body, "<td>\n");
  char *composer_href;
    asprintf (&composer_href, 
      "<a href=\"/gui/composers?where=%s&covers=%d\">[composers]</a>", where, 
        covers);

  vs_string_append (body, composer_href);
  free (composer_href);
  vs_string_append (body, "</td>\n");

  free (where);

  vs_string_append (body, "</tr>\n");
  }

/*======================================================================
  gui_genres_get_body
======================================================================*/
VSString *gui_genres_get_body (const char *path, 
            const VSProps *arguments, int count, MediaDatabase *mdb)
  {
  return gui_get_results_page (path, arguments, count, mdb, "genres",  
    MDB_COL_GENRE, TRUE, gui_genres_genre_cell, TRUE);
  }


