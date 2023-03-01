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
#include "gui_tracks.h"
#include "media_database.h"

#define SAFE(x)((x) ? (x) : "")

/*======================================================================
  gui_tracks_track_cell
======================================================================*/
void gui_tracks_track_cell (VSString *body, const char *path, 
       BOOL covers, MediaDatabase *mdb)
  {
  vs_string_append (body, "<tr>");

  VSString *enc_path = http_util_encode_for_js (path); 
  vs_string_append (body, "<td>");
  vs_string_append (body, "<a href=\"javascript:cmd_play('");
  vs_string_append (body, vs_string_cstr (enc_path)); 
  vs_string_append (body, "')\">[play]</a> ");
  vs_string_append (body, "<a href=\"javascript:cmd_add('");
  vs_string_append (body, vs_string_cstr (enc_path)); 
  vs_string_append (body, "')\">[add]</a> ");
  vs_string_append (body, "</td>");
  vs_string_destroy (enc_path);
  

  AudioMetadata *amd = media_database_get_amd (mdb, path);

  if (amd)
    {
    // Path
    vs_string_append (body, "<td>");
    const char *title = SAFE(audio_metadata_get_title (amd));
    if (title && title[0])
      vs_string_append (body, title);
    else
      vs_string_append (body, path);
    vs_string_append (body, "</td>");
    vs_string_append (body, "<td>");
    vs_string_append (body, SAFE(audio_metadata_get_track (amd)));
    vs_string_append (body, "</td>");

    // Album
    
    vs_string_append (body, "<td>");

    const char *album = SAFE (audio_metadata_get_album (amd));
    char *enc2 = media_database_escape_sql (album);
    VSString *enc_album = http_util_encode_for_js (enc2); 
    char *where;
    asprintf (&where, "album='%s'", vs_string_cstr(enc_album));
    vs_string_destroy (enc_album);
    free (enc2);

    char *album_href;
    asprintf (&album_href, 
      "<a href=\"/gui/albums?where=%s&covers=%d\">%s</a>", where, 
        covers, album);

    free (where);

    vs_string_append (body, album_href);
    free (album_href);

    vs_string_append (body, "</td>");

    // Artist
    
    vs_string_append (body, "<td>");

    const char *artist = SAFE (audio_metadata_get_artist (amd));

    enc2 = media_database_escape_sql (artist);
    VSString *enc_artist = http_util_encode_for_js (enc2); 
    asprintf (&where, "artist='%s'", vs_string_cstr(enc_artist));
    vs_string_destroy (enc_artist);
    free (enc2);

    asprintf (&album_href, 
      "<a href=\"/gui/albums?where=%s&covers=%d\">%s</a>", where, 
        covers, artist);

    free (where);

    vs_string_append (body, album_href);
    free (album_href);

    vs_string_append (body, "</td>");

    // Album artist
    
    vs_string_append (body, "<td>");

    const char *album_artist = SAFE (audio_metadata_get_album_artist (amd));

    enc2 = media_database_escape_sql (album_artist);
    VSString *enc_album_artist = http_util_encode_for_js (enc2); 
    asprintf (&where, "album_artist='%s'", vs_string_cstr(enc_album_artist));
    vs_string_destroy (enc_album_artist);
    free (enc2);

    asprintf (&album_href, 
      "<a href=\"/gui/albums?where=%s&covers=%d\">%s</a>", where, 
        covers, artist);

    free (where);

    vs_string_append (body, album_href);
    free (album_href);

    vs_string_append (body, "</td>");

    // Composer

    vs_string_append (body, "<td>");

    const char *composer = SAFE (audio_metadata_get_composer (amd));

    enc2 = media_database_escape_sql (composer);
    VSString *enc_composer = http_util_encode_for_js (enc2); 
    asprintf (&where, "composer='%s'", vs_string_cstr(enc_composer));
    vs_string_destroy (enc_composer);
    free (enc2);

    char *composer_href;
    asprintf (&composer_href, 
      "<a href=\"/gui/albums?where=%s&covers=%d\">%s</a>", where, 
        covers, composer);

    free (where);

    vs_string_append (body, composer_href);
    vs_string_append (body, "</td>");
    free (composer_href);

    // Genre

    vs_string_append (body, "<td>");

    const char *genre = SAFE (audio_metadata_get_genre (amd));

    enc2 = media_database_escape_sql (genre);
    VSString *enc_genre = http_util_encode_for_js (enc2); 
    asprintf (&where, "genre='%s'", vs_string_cstr(enc_genre));
    vs_string_destroy (enc_genre);
    free (enc2);

    char *genre_href;
    asprintf (&genre_href, 
      "<a href=\"/gui/albums?where=%s&covers=%d\">%s</a>", where, 
        covers, genre);

    free (where);

    vs_string_append (body, genre_href);
    free (genre_href);

    vs_string_append (body, "</td>");


    audio_metadata_destroy (amd);
    }
  else
    {
    vs_string_append (body, "<td>");
    vs_string_append (body, path);
    vs_string_append (body, "</td>");
    vs_string_append (body, "<td></td>");
    vs_string_append (body, "<td></td>");
    vs_string_append (body, "<td></td>");
    vs_string_append (body, "<td></td>");
    vs_string_append (body, "<td></td>");
    }

  vs_string_append (body, "</tr>\n");
  }

/*======================================================================
  gui_tracks_get_body
======================================================================*/
VSString *gui_tracks_get_body (const char *path, 
            const VSProps *arguments, int count, MediaDatabase *mdb)
  {
  return gui_get_results_page (path, arguments, count, mdb, "tracks",  
    "Tracks", MDB_COL_PATH, TRUE, gui_tracks_track_cell, TRUE);
  }


