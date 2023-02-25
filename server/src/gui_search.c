/*======================================================================
  
  vlc-server

  server/src/gui_help.c

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
#include "gui_albums.h"
#include "gui_artists.h"
#include "gui_album_artists.h"
#include "gui_genres.h"
#include "gui_composers.h"
#include "gui_tracks.h"
#include "gui_search.h"
#include "gui.h"

/*======================================================================
  
  gui_search_get_body

======================================================================*/
VSString *gui_search_get_body (MediaDatabase *mdb, 
           const VSProps *_arguments, int matches_per_page)
  {
  IN
  // Nasty reinterpretation of the const _arguments. I know that it's 
  //   safe to do this here, but I don't want to make the arguments
  //   property list writeable all the way through the call stack. Ideally,
  //   I would create a non-const copy in this method, but it's just a
  //   waste of memory.
  VSProps *arguments = (VSProps*)_arguments;
  const char *keyword = vs_props_get (arguments, "keyword");

  VSString *body = vs_string_create ("");
  if (keyword && keyword[0])
    {
    char *esc_keyword = media_database_escape_sql (keyword); 

    // Albums
    char *where;
    asprintf (&where, "album regexp '\\b%s\\b'", esc_keyword);
    vs_props_add (arguments, "where", where); 
    VSString *fragment = gui_get_limited_results_page 
       (arguments, matches_per_page, 
       mdb, "albums",  "Album matches", MDB_COL_ALBUM, TRUE, 
       gui_albums_album_cell, FALSE);

    vs_string_append (body, vs_string_cstr (fragment));
    free (where);
    vs_string_destroy (fragment);
    
    // Paths
    asprintf (&where, "title regexp '\\b%s\\b'", esc_keyword);
    vs_props_add (arguments, "where", where); 
    fragment = gui_get_limited_results_page 
       (arguments, matches_per_page,
       mdb, "tracks",  "Track title matches", MDB_COL_PATH, TRUE, 
       gui_tracks_track_cell, TRUE);

    vs_string_append (body, vs_string_cstr (fragment));
    free (where);
    vs_string_destroy (fragment);
    
    // Artists 
    asprintf (&where, "artist regexp '\\b%s\\b'", esc_keyword);
    vs_props_add (arguments, "where", where); 
    fragment = gui_get_limited_results_page 
       (arguments, matches_per_page,
       mdb, "artists",  "Artist matches", MDB_COL_ARTIST, TRUE, 
       gui_artists_artist_cell, TRUE);

    vs_string_append (body, vs_string_cstr (fragment));
    free (where);
    vs_string_destroy (fragment);
    
    // Album artists 
    asprintf (&where, "album_artist regexp '\\b%s\\b'", esc_keyword);
    vs_props_add (arguments, "where", where); 
    fragment = gui_get_limited_results_page 
       (arguments, matches_per_page,
       mdb, "album_artists",  "Album artist matches", MDB_COL_ALBUM_ARTIST, 
       TRUE, gui_album_artists_artist_cell, TRUE);

    vs_string_append (body, vs_string_cstr (fragment));
    free (where);
    vs_string_destroy (fragment);
    
    // Composers 
    asprintf (&where, "composer regexp '\\b%s\\b'", esc_keyword);
    vs_props_add (arguments, "where", where); 
    fragment = gui_get_limited_results_page 
       (arguments, matches_per_page,
       mdb, "composers",  "Composer matches", MDB_COL_COMPOSER, TRUE, 
       gui_composers_composer_cell, TRUE);

    vs_string_append (body, vs_string_cstr (fragment));
    free (where);
    vs_string_destroy (fragment);
    
    // Genres 
    asprintf (&where, "genre regexp '\\b%s\\b'", esc_keyword);
    vs_props_add (arguments, "where", where); 
    fragment = gui_get_limited_results_page 
       (arguments, matches_per_page, 
       mdb, "genres",  "Genre matches", MDB_COL_GENRE, TRUE, 
       gui_genres_genre_cell, TRUE);

    vs_string_append (body, vs_string_cstr (fragment));
    free (where);
    vs_string_destroy (fragment);
  
    free (esc_keyword);
    }
  else
    {
    vs_string_append (body, "No search term was provided"); 
    }

  OUT
  return body;
  }


