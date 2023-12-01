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
#include <vlc-server/api-client.h>
#include <ncursesw/curses.h>
#include "message.h" 
#include "status.h" 
#include "keys.h" 
#include "app_context.h" 
#include "view_misc.h" 
#include "view_help.h" 

/*======================================================================
  
  mykeyname 

======================================================================*/
static const char *mykeyname (int key)
  {
  switch (key)
    {
    case ' ': return "SPACE";
    case 27: return "ESCAPE";
    case 10: return "ENTER";
    }
  return keyname(key);
  }

/*======================================================================
  
  populate_help

======================================================================*/
static VSList *populate_help (void)
  {
  VSList *ret = vs_list_create (free);
  char *s;
  asprintf (&s, "Letters a-z select starting point in list/menu");
  vs_list_append (ret, s);
  asprintf (&s, "Play/pause: '%s'", mykeyname(keys_toggle_pause));
  vs_list_append (ret, s);
  asprintf (&s, "Stop: '%s'", mykeyname(keys_stop));
  vs_list_append (ret, s);
  asprintf (&s, "Play random: '%s'", mykeyname(keys_play_random));
  vs_list_append (ret, s);
  asprintf (&s, "Previous menu/quit: '%s'", mykeyname(keys_quit));
  vs_list_append (ret, s);
  asprintf (&s, "Select item: '%s'", mykeyname(keys_select));
  vs_list_append (ret, s);
  asprintf (&s, "Move up a line: '%s'", mykeyname(keys_line_up));
  vs_list_append (ret, s);
  asprintf (&s, "Move up a page: '%s'", mykeyname(keys_page_up));
  vs_list_append (ret, s);
  asprintf (&s, "Move down a line: '%s'", mykeyname(keys_line_down));
  vs_list_append (ret, s);
  asprintf (&s, "Move down a page: '%s'", mykeyname(keys_page_down));
  vs_list_append (ret, s);
  asprintf (&s, "Play previous in playlist: '%s'", mykeyname(keys_prev));
  vs_list_append (ret, s);
  asprintf (&s, "Play next in playlist: '%s'", mykeyname(keys_next));
  vs_list_append (ret, s);
  asprintf (&s, "Volume up: '%s'", mykeyname(keys_volume_up));
  vs_list_append (ret, s);
  asprintf (&s, "Volume down: '%s'", mykeyname(keys_volume_down));
  vs_list_append (ret, s);
  return ret;
  }


/*======================================================================
  
  select_menu 

======================================================================*/
static void select_menu (LibVlcServerClient *lvsc, const char *line,
         const AppContext *context)
  {
  (void)lvsc; (void)line; (void)context;
  message_show ("Can't select in this page", context);
  }

/*======================================================================
  
  view_help

======================================================================*/
void view_help (WINDOW *main_window, LibVlcServerClient *lvsc, 
       int h, int w, int row, int col, const AppContext *context)
  {
  VSList *list = populate_help ();

  view_list (main_window, lvsc, h, w, row, col, list, 
       select_menu, "Help", context);

  vs_list_destroy (list);
  }

