/*======================================================================
  
  nv-vlc-client 

  nc-vlc-client/src/view_control.c

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
#include "control.h" 

#define MENU_STOP "Stop"
#define MENU_PLAY_PAUSE "Play/pause"
#define MENU_NEXT "Next track"
#define MENU_PREV "Previous track"
#define MENU_VOL_UP "Volume up"
#define MENU_VOL_DOWN "Volume down"
#define MENU_SCAN "Run quick scan"
#define MENU_FULLSCAN "Run full scan (slow)"
#define MENU_PLAY_RANDOM "Play random album"

/*======================================================================
  
  populate_control_menu

======================================================================*/
static VSList *populate_control_menu (void)
  {
  VSList *ret = vs_list_create (free);
  vs_list_append (ret, strdup (MENU_PLAY_PAUSE));
  vs_list_append (ret, strdup (MENU_STOP));
  vs_list_append (ret, strdup (MENU_PLAY_RANDOM));
  vs_list_append (ret, strdup (MENU_NEXT));
  vs_list_append (ret, strdup (MENU_PREV));
  vs_list_append (ret, strdup (MENU_VOL_UP));
  vs_list_append (ret, strdup (MENU_VOL_DOWN));
  vs_list_append (ret, strdup (MENU_SCAN));
  vs_list_append (ret, strdup (MENU_FULLSCAN));
  return ret;
  }


/*======================================================================
  
  select_menu 

======================================================================*/
static void select_menu (LibVlcServerClient *lvsc, const char *line,
         const AppContext *context)
  {
  (void)context;
  if (strcmp (line, MENU_STOP) == 0)
     control_stop (lvsc, context); 
  else if (strcmp (line, MENU_PLAY_PAUSE) == 0)
     control_toggle_pause (lvsc, context); 
  else if (strcmp (line, MENU_NEXT) == 0)
     control_next (lvsc, context); 
  else if (strcmp (line, MENU_PREV) == 0)
     control_prev (lvsc, context); 
  else if (strcmp (line, MENU_VOL_UP) == 0)
     control_volume_up (lvsc, context); 
  else if (strcmp (line, MENU_VOL_DOWN) == 0)
     control_volume_down (lvsc, context); 
  else if (strcmp (line, MENU_SCAN) == 0)
     control_run_scanner (lvsc, context); 
  else if (strcmp (line, MENU_FULLSCAN) == 0)
     control_run_full_scanner (lvsc, context); 
  else if (strcmp (line, MENU_PLAY_RANDOM) == 0)
     control_play_random_album (lvsc, context); 
  }

/*======================================================================
  
  view_control

======================================================================*/
void view_control (WINDOW *main_window, LibVlcServerClient *lvsc, 
       int h, int w, int row, int col, const AppContext *context)
  {
  VSList *list = populate_control_menu ();

  view_list (main_window, lvsc, h, w, row, col, list, 
       select_menu, "Control", context);

  vs_list_destroy (list);
  }

