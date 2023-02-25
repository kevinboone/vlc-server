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
#include "gui_composers.h"
#include "media_database.h"

#define SAFE(x)((x) ? (x) : "")

/*======================================================================
  gui_composers_composer_cell
======================================================================*/
void gui_composers_composer_cell (VSString *body, const char *composer, 
       BOOL covers, MediaDatabase *mdb)
  {
  IN
  (void)mdb;

  vs_string_append (body, "<tr>");

  vs_string_append (body, "<td>");
  if (composer && composer[0])
    vs_string_append (body, composer); 
  else
    vs_string_append (body, "[blank]"); 
  vs_string_append (body, "</td>");

  char *enc2 = media_database_escape_sql (composer);
  VSString *enc_composer = vs_string_encode_url (enc2); 

  char *where;
  //asprintf (&where, "composer='%s'", vs_string_cstr(enc_composer));
  asprintf (&where, "composer='%s'", vs_string_cstr (enc_composer));
  vs_string_destroy (enc_composer);
  free (enc2);

  char *album_href;
    asprintf (&album_href, 
      "<a href=\"/gui/albums?where=%s&covers=%d\">[albums]</a>", where, 
	covers);

  free (where);

  vs_string_append (body, "<td>\n");
  vs_string_append (body, album_href);
  vs_string_append (body, "</td>\n");
  free (album_href);

  vs_string_append (body, "</tr>\n");
  OUT
  }

/*======================================================================
  gui_composers_get_body
======================================================================*/
VSString *gui_composers_get_body (const char *path, 
            const VSProps *arguments, int count, MediaDatabase *mdb)
  {
  return gui_get_results_page (path, arguments, count, mdb, "composers",  
    "Composers", MDB_COL_COMPOSER, TRUE, gui_composers_composer_cell, TRUE);
  }


