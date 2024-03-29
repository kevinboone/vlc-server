/*======================================================================
  
  nv-vlc-client 

  nc-vlc-client/src/view_menu.c

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
#include <ncursesw/curses.h>
#include "message.h" 
#include "status.h" 
#include "keys.h" 
#include "app_context.h" 
#include "view_control.h" 
#include "view_albums.h" 
#include "view_playlist.h" 
#include "view_genres.h" 
#include "view_artists.h" 
#include "view_streams.h" 
#include "view_help.h" 
#include "view_sys_info.h" 
#include "view_main_menu.h" 
#include "control.h" 

#define MENU_ALBUMS "Albums"
#define MENU_GENRES "Genres"
#define MENU_ARTISTS "Artists"
#define MENU_STREAMS "Streams"
#define MENU_PLAY_RANDOM_ALBUM "Random album"
#define MENU_PLAYLIST "Playlist"
#define MENU_CONTROL "Control"
#define MENU_SYS_INFO "System info"
#define MENU_HELP "Help"

extern WINDOW *main_window; 

/*======================================================================
  
  populate_main_menu

======================================================================*/
static VSList *populate_main_menu (void)
  {
  VSList *ret = vs_list_create (free);
  vs_list_append (ret, strdup (MENU_ALBUMS));
  vs_list_append (ret, strdup (MENU_GENRES));
  vs_list_append (ret, strdup (MENU_ARTISTS));
  vs_list_append (ret, strdup (MENU_STREAMS));
  vs_list_append (ret, strdup (MENU_PLAY_RANDOM_ALBUM));
  vs_list_append (ret, strdup (MENU_PLAYLIST));
  vs_list_append (ret, strdup (MENU_CONTROL));
  vs_list_append (ret, strdup (MENU_SYS_INFO));
  vs_list_append (ret, strdup (MENU_HELP));
  return ret;
  }


/*======================================================================
  
  select_menu 

======================================================================*/
static void select_menu (LibVlcServerClient *lvsc, const char *line,
      const AppContext *context)
  {
  if (strcmp (line, MENU_ALBUMS) == 0)
      view_albums (main_window, lvsc, LINES - 3 - 5, COLS, 5, 0, 
        NULL, NULL, context);
  else if (strcmp (line, MENU_GENRES) == 0)
      view_genres (main_window, lvsc, LINES - 3 - 5, COLS, 5, 0, context);
  else if (strcmp (line, MENU_ARTISTS) == 0)
      view_artists (main_window, lvsc, LINES - 3 - 5, COLS, 5, 0, context);
  else if (strcmp (line, MENU_STREAMS) == 0)
      view_streams (main_window, lvsc, LINES - 3 - 5, COLS, 5, 0, context);
  else if (strcmp (line, MENU_PLAYLIST) == 0)
      view_playlist (main_window, lvsc, LINES - 3 - 5, COLS, 
      5, 0, context);
  else if (strcmp (line, MENU_HELP) == 0)
      view_help (main_window, lvsc, LINES - 3 - 5, COLS, 5, 0, context);
  else if (strcmp (line, MENU_CONTROL) == 0)
      view_control (main_window, lvsc, LINES - 3 - 5, COLS, 5, 0, context);
  else if (strcmp (line, MENU_SYS_INFO) == 0)
      view_sys_info (main_window, lvsc, LINES - 3 - 5, COLS, 5, 0, context);
  else if (strcmp (line, MENU_PLAY_RANDOM_ALBUM) == 0)
      control_play_random_album (lvsc, context);
  }

/*======================================================================
  
  view_main_menu

======================================================================*/
void view_main_menu (WINDOW *main_window, LibVlcServerClient *lvsc, 
       int h, int w, int row, int col, const AppContext *context)
  {
  VSList *list = populate_main_menu ();

  view_list (main_window, lvsc, h, w, row, col, list, 
       select_menu, "Main menu", context);

  vs_list_destroy (list);
  }

