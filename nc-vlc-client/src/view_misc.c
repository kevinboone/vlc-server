/*======================================================================
  
  nv-vlc-client 

  nc-vlc-client/src/view_misc.c

  Assorted helper functions used by the view_xxx modules.

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <vlc-server/api-client.h>
#include <ncursesw/curses.h>
#include "keys.h" 
#include "message.h" 
#include "status.h" 
#include "view_misc.h" 
#include "info_window.h" 
#include "control.h" 
#include "colour.h" 

/*======================================================================
  
  view_misc_handle_non_menu_key

======================================================================*/
BOOL view_misc_handle_non_menu_key (LibVlcServerClient *lvsc, int ch,
       const AppContext *context)
  {
  if (ch == keys_toggle_pause)
      {
      control_toggle_pause (lvsc, context);
      status_update (lvsc, context);
      return TRUE;
      }
  else if (ch == keys_stop)
      {
      control_stop (lvsc, context);
      status_update (lvsc, context);
      return TRUE;
      }
  else if (ch == keys_volume_down)
      {
      control_volume_down (lvsc, context);
      status_update (lvsc, context);
      return TRUE;
      }
  else if (ch == keys_volume_up)
      {
      control_volume_up (lvsc, context);
      status_update (lvsc, context);
      return TRUE;
      }
  else if (ch == keys_next)
      {
      control_next (lvsc, context);
      status_update (lvsc, context);
      return TRUE;
      }
  else if (ch == keys_prev)
      {
      control_prev (lvsc, context);
      status_update (lvsc, context);
      return TRUE;
      }
  else if (ch == keys_play_random)
      {
      control_play_random_album (lvsc, context);
      status_update (lvsc, context);
      return TRUE;
      }
  return FALSE;
  }

/*======================================================================
  
  update_window 

======================================================================*/
static void update_window (WINDOW *my_window, const VSList *album_list,
         int rows, int width, int first_index_on_screen, 
         int current_selection, int list_length, const char *title,
         const AppContext *context)
  {
  werase (my_window);
  if (context->colour)
    wattron (my_window, COLOR_PAIR (CPAIR_BOX));
  box (my_window, 0, 0);
  if (context->colour)
    wattroff (my_window, COLOR_PAIR (CPAIR_BOX));
  if (context->colour)
    wattron (my_window, COLOR_PAIR (CPAIR_MENU_CAPTION));
  mvwaddstr (my_window, 0, 3, title);
  if (context->colour)
    wattroff (my_window, COLOR_PAIR (CPAIR_MENU_CAPTION));

  if (album_list)
    {
    int index = first_index_on_screen;
    int row = 0;
    while (index < list_length  && row < rows)
        {
        char *s = vs_list_get ((VSList *)album_list, index);
        if (current_selection == index)
           {
           wattron (my_window, A_BOLD);	
           if (context->colour)
             wattron (my_window, COLOR_PAIR (CPAIR_MENU_HIGHLIGHT));
           }
        mvwaddnstr (my_window, row + 1, 1, s, width - 2);
        if (current_selection == index)
           {
           wattroff (my_window, A_BOLD);	
           if (context->colour)
             wattroff (my_window, COLOR_PAIR (CPAIR_MENU_HIGHLIGHT));
           }
        index++;
        row++;
        }
    }
  else
    mvwaddnstr (my_window, 1, 1, "No results", width - 2);
  
  wrefresh (my_window);
  }

/*======================================================================
  
  find_in_list 

======================================================================*/
static int find_in_list (const VSList *list, int list_length, 
            const char *s)
  {
  int ls = strlen (s);
  for (int i = 0; i < list_length; i++)
    {
    const char *entry = vs_list_get ((VSList *)list, i);
    if (strncasecmp (s, entry, ls) == 0) return i;
    }
  return -1;
  }

/*======================================================================
  
  view_list

======================================================================*/
void view_list (WINDOW *main_window, LibVlcServerClient *lvsc, 
       int h, int w, int row, int col, const VSList *list,
       VMSelectFunction select_function, const char *title,
       const AppContext *context)
  {
  int ch;
  halfdelay (50);  
  noecho();
  keypad (stdscr, TRUE);
  curs_set (0);
  WINDOW *my_window = subwin (main_window, h, w, row, col); 

  message_show ("", context); // TODO -- helpful message
  int list_length = vs_list_length ((VSList *)list);
  int first_index_on_screen = 0;
  int current_index = 0;
  int timeouts = 0;
  update_window (my_window, list, h - 2, w, first_index_on_screen,
     current_index, list_length, title, context);
  while ((ch = getch ()) != keys_quit || context->kiosk)
    {
    if (ch != ERR) timeouts = 0;
    //char s[20];
    //sprintf (s, "%d", ch);
    //message_show (s);
    if (ch == keys_line_down)
      {
      if (current_index < list_length - 1) current_index++;
      if (current_index - first_index_on_screen >= h - 2)
	first_index_on_screen = current_index - h + 3; 
      update_window (my_window, list, h - 2, w, 
	  first_index_on_screen, current_index, list_length, title, context);
      }
    else if (ch == keys_page_down)
      {
      current_index += h - 2;
      if (current_index >= list_length) current_index = list_length - 1;
      if (current_index - first_index_on_screen >= h - 2)
	first_index_on_screen = current_index - h  + 3; 
      update_window (my_window, list, h - 2, w, 
	  first_index_on_screen, current_index, list_length, title, context);
      }
    else if (ch == keys_line_up)
      {
      if (current_index > 0) current_index--;
      if (current_index - first_index_on_screen < 0)
	first_index_on_screen = current_index; 
      update_window (my_window, list, h - 2, w, 
	  first_index_on_screen, current_index, list_length, title, context);
      }
    else if (ch == keys_page_up)
      { 
      if (current_index > 0) current_index -= h - 2;
      if (current_index < 0) current_index = 0;
      if (current_index - first_index_on_screen < 0)
	first_index_on_screen = current_index; 
      update_window (my_window, list, h - 2, w, 
	  first_index_on_screen, current_index, list_length, title, context);
      }
    else if (ch == keys_select)
      {
      if (list_length == 0) break;
      const char *s= vs_list_get ((VSList *)list, current_index);
      curs_set (1);
      echo();
      AppContext temp_context;
      memcpy (&temp_context, context, sizeof (AppContext));
      temp_context.kiosk = FALSE;
      select_function (lvsc, s, &temp_context);
      noecho();
      curs_set (0);
      update_window (my_window, list, h - 2, w, 
	  first_index_on_screen, current_index, list_length, title, context);
      status_update (lvsc, context);
      }
    else if (view_misc_handle_non_menu_key (lvsc, ch, context))
      {
      // Nothing to do
      }
    else if isalnum (ch)
      {
      char s[2];
      s[0] = (char)ch; s[1] = 0;
      int index = find_in_list (list, list_length, s);
      if (index >= 0)
        {
        current_index = index; 
        first_index_on_screen = current_index; 
	update_window (my_window, list, h - 2, w, 
	    first_index_on_screen, current_index, list_length, title, context);
        }
      }
    else if (ch == ERR) 
      {
      status_update (lvsc, context);
      message_show("", context);

      timeouts++;
      if (timeouts >= 3) 
        {
        message_show("", context);
        info_window_run (main_window, lvsc, context);
        update_window (my_window, list, h - 2, w, 
	  first_index_on_screen, current_index, list_length, title, context);
        status_update (lvsc, context);
        timeouts = 0;
        message_show("", context);
        }
      }
    }

  delwin (my_window);
  echo();
  curs_set (1);
  }

