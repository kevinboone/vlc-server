/*======================================================================
  
  nv-vlc-client 

  nc-vlc-client/src/view_genres.c

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
#include "view_misc.h" 
#include "view_albums.h" 
#include "view_genres.h" 

extern WINDOW *main_window; 

/*======================================================================
  
  populate_genre_list

======================================================================*/
static VSList *populate_genre_list (LibVlcServerClient *lvsc, 
         char **error)
  {
  VSApiError err_code;
  VSList *list = libvlc_server_client_list_genres (lvsc, 
      NULL, &err_code, error);
  if (list)
    return list;
  else
    return NULL;
  }


/*======================================================================
  
  show_genre 

======================================================================*/
static void show_genre (LibVlcServerClient *lvsc, const char *genre)
  {
  char *where;
  char *escaped_genre = libvlc_escape_sql (genre);
  asprintf (&where, "genre='%s'", escaped_genre);
  view_albums (main_window, lvsc, LINES - 3 - 5, COLS, 5, 0, 
     "Genres", where);
  free (escaped_genre);
  free (where);
  }

/*======================================================================
  
  view_genres

======================================================================*/
void view_genres (WINDOW *main_window, LibVlcServerClient *lvsc, 
       int h, int w, int row, int col)
  {
  char *error = NULL;
  message_show ("Loading genres...");
  VSList *genre_list = populate_genre_list (lvsc, &error);

  if (error)
    {
    message_show (error); 
    free (error);
    }
  else
    {
    view_list (main_window, lvsc, h, w, row, col, genre_list, show_genre,
       "Genres", FALSE);
    }

  if (genre_list) vs_list_destroy (genre_list);
  }

