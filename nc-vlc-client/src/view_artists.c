/*======================================================================
  
  nv-vlc-client 

  nc-vlc-client/src/view_artists.c

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <vlc-server/vs_list.h>
#include <vlc-server/api-client.h>
#include <vlc-server/vs_search_constraints.h>
#include <ncursesw/curses.h>
#include "message.h" 
#include "status.h" 
#include "keys.h" 
#include "app_context.h" 
#include "view_misc.h" 
#include "view_albums.h" 
#include "view_artists.h" 

extern WINDOW *main_window; 

/*======================================================================
  
  populate_artist_list

======================================================================*/
static VSList *populate_artist_list (LibVlcServerClient *lvsc, 
         char **error)
  {
  VSApiError err_code;
  VSList *list = libvlc_server_client_list_artists (lvsc, 
      NULL, &err_code, error);
  if (list)
    return list;
  else
    return NULL;
  }


/*======================================================================
  
  show_artist 

======================================================================*/
static void show_artist (LibVlcServerClient *lvsc, const char *artist,
     const AppContext *context)
  {
  (void)context;
  char *where;
  char *escaped_artist = libvlc_escape_sql (artist);
  asprintf (&where, "artist='%s'", escaped_artist);
  view_albums (main_window, lvsc, LINES - 3 - 5, COLS, 5, 0, 
     "Genres", where, context);
  free (escaped_artist);
  free (where);
  }

/*======================================================================
  
  view_artists

======================================================================*/
void view_artists (WINDOW *main_window, LibVlcServerClient *lvsc, 
       int h, int w, int row, int col, const AppContext *context)
  {
  char *error = NULL;
  message_show ("Loading artists...", context);
  VSList *artist_list = populate_artist_list (lvsc, &error);

  if (error)
    {
    message_show (error, context); 
    free (error);
    }
  else
    {
    view_list (main_window, lvsc, h, w, row, col, artist_list, show_artist,
       "Genres", context);
    }

  if (artist_list) vs_list_destroy (artist_list);
  }

