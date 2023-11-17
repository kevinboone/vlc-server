/*======================================================================
  
  nv-vlc-client 

  nc-vlc-client/src/view_misc.c

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

/*======================================================================
  
  view_misc_run_scanner

======================================================================*/
void view_misc_run_scanner (LibVlcServerClient *lvsc)
  {
  char *message = NULL;
  VSApiError err_code;
  libvlc_server_client_scan (lvsc, &err_code, &message);
  if (message)
    {
    message_show (message);
    free (message);
    }
  }

/*======================================================================
  
  view_misc_toggle_pause 

======================================================================*/
void view_misc_toggle_pause (LibVlcServerClient *lvsc)
  {
  char *message = NULL;
  VSApiError err_code;
  libvlc_server_client_toggle_pause (lvsc, &err_code, &message);
  if (message)
    {
    message_show (message);
    free (message);
    }
  }

/*======================================================================
  
  view_misc_stop

======================================================================*/
void view_misc_stop (LibVlcServerClient *lvsc)
  {
  char *message = NULL;
  VSApiError err_code;
  libvlc_server_client_stop (lvsc, &err_code, &message);
  if (message)
    {
    message_show (message);
    free (message);
    }
  }

/*======================================================================
  
  view_misc_next

======================================================================*/
void view_misc_next (LibVlcServerClient *lvsc)
  {
  char *message = NULL;
  VSApiError err_code;
  libvlc_server_client_next (lvsc, &err_code, &message);
  if (message)
    {
    message_show (message);
    free (message);
    }
  }

/*======================================================================
  
  view_misc_prev

======================================================================*/
void view_misc_prev (LibVlcServerClient *lvsc)
  {
  char *message = NULL;
  VSApiError err_code;
  libvlc_server_client_prev (lvsc, &err_code, &message);
  if (message)
    {
    message_show (message);
    free (message);
    }
  }

/*======================================================================
  
  view_misc_volume_down

======================================================================*/
void view_misc_volume_down (LibVlcServerClient *lvsc)
  {
  char *message = NULL;
  VSApiError err_code;
  libvlc_server_client_volume_down (lvsc, &err_code, &message);
  if (message)
    {
    message_show (message);
    free (message);
    }
  else
    {
    VSServerStat *stat = libvlc_server_client_stat 
         (lvsc, &err_code, NULL);
    if (err_code == 0)
      {
      char s[30];
      sprintf (s, "Volume %d%%", 
         vs_server_stat_get_volume (stat));
      message_show (s);
      }
    if (stat) 
      vs_server_stat_destroy (stat);
    }
  }

/*======================================================================
  
  view_misc_volume_up

======================================================================*/
void view_misc_volume_up (LibVlcServerClient *lvsc)
  {
  char *message = NULL;
  VSApiError err_code;
  libvlc_server_client_volume_up (lvsc, &err_code, &message);
  if (message)
    {
    message_show (message);
    free (message);
    }
  else
    {
    VSServerStat *stat = libvlc_server_client_stat 
         (lvsc, &err_code, NULL);
    if (err_code == 0)
      {
      char s[30];
      sprintf (s, "Volume %d%%", 
         vs_server_stat_get_volume (stat));
      message_show (s);
      }
    if (stat) 
      vs_server_stat_destroy (stat);
    }
  }

/*======================================================================
  
  view_misc_fit_string

  NOTE this won't work for strings that contain wide characters, and
    it may even corrupt them.

======================================================================*/
char *view_misc_fit_string (const char *s, int w)
  {
  int l = strlen (s);
  if (l <= w) return strdup (s);
  int remove = l - w; 
  int first = (l - remove) / 2;
  char *ret = strdup (s);
  ret[first] = 0;
  strcat (ret, "...");
  strcat (ret, s + first + remove + 3);
  return ret; 
  }

/*======================================================================
  
  update_window 

======================================================================*/
static void update_window (WINDOW *my_window, const VSList *album_list,
         int rows, int width, int first_index_on_screen, 
         int current_selection, int list_length, const char *title)
  {
  werase (my_window);
  box (my_window, 0, 0);
  mvwaddstr (my_window, 0, 3, title);

  if (album_list)
    {
    int index = first_index_on_screen;
    int row = 0;
    while (index < list_length  && row < rows)
        {
        char *s = vs_list_get ((VSList *)album_list, index);
        if (current_selection == index)
           wattron (my_window, A_BOLD);	
        mvwaddnstr (my_window, row + 1, 1, s, width - 2);
        if (current_selection == index)
           wattroff (my_window, A_BOLD);	
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
       BOOL kiosk)
  {
  int ch;
  halfdelay (50);  
  noecho();
  keypad (stdscr, TRUE);
  curs_set (0);
  WINDOW *my_window = subwin (main_window, h, w, row, col); 

  message_show (""); // TODO -- helpful message
  int list_length = vs_list_length ((VSList *)list);
  int first_index_on_screen = 0;
  int current_index = 0;
  int timeouts = 0;
  update_window (my_window, list, h - 2, w, first_index_on_screen,
     current_index, list_length, title);
  while ((ch = getch ()) != keys_quit || kiosk)
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
	  first_index_on_screen, current_index, list_length, title);
      }
    else if (ch == keys_page_down)
      {
      current_index += h - 2;
      if (current_index >= list_length) current_index = list_length - 1;
      if (current_index - first_index_on_screen >= h - 2)
	first_index_on_screen = current_index - h  + 3; 
      update_window (my_window, list, h - 2, w, 
	  first_index_on_screen, current_index, list_length, title);
      }
    else if (ch == keys_line_up)
      {
      if (current_index > 0) current_index--;
      if (current_index - first_index_on_screen < 0)
	first_index_on_screen = current_index; 
      update_window (my_window, list, h - 2, w, 
	  first_index_on_screen, current_index, list_length, title);
      }
    else if (ch == keys_page_up)
      { 
      if (current_index > 0) current_index -= h - 2;
      if (current_index < 0) current_index = 0;
      if (current_index - first_index_on_screen < 0)
	first_index_on_screen = current_index; 
      update_window (my_window, list, h - 2, w, 
	  first_index_on_screen, current_index, list_length, title);
      }
    else if (ch == keys_select)
      {
      if (list_length == 0) break;
      const char *s= vs_list_get ((VSList *)list, current_index);
      curs_set (1);
      echo();
      select_function (lvsc, s);
      noecho();
      curs_set (0);
      update_window (my_window, list, h - 2, w, 
	  first_index_on_screen, current_index, list_length, title);
      status_update (lvsc);
      }
    else if (ch == keys_toggle_pause)
      {
      view_misc_toggle_pause (lvsc);
      status_update (lvsc);
      }
    else if (ch == keys_stop)
      {
      view_misc_stop (lvsc);
      status_update (lvsc);
      }
    else if (ch == keys_volume_down)
      {
      view_misc_volume_down (lvsc);
      status_update (lvsc);
      }
    else if (ch == keys_volume_up)
      {
      view_misc_volume_up (lvsc);
      status_update (lvsc);
      }
    else if (ch == keys_next)
      {
      view_misc_next (lvsc);
      status_update (lvsc);
      }
    else if (ch == keys_prev)
      {
      view_misc_prev (lvsc);
      status_update (lvsc);
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
	    first_index_on_screen, current_index, list_length, title);
        }
      }
    else if (ch == ERR) 
      {
      status_update (lvsc);
      message_show ("");
      if (!kiosk)
        {
        timeouts++;
        if (timeouts >= 3) 
          goto quit;
        }
      }
    }

quit:
  delwin (my_window);
  echo();
  curs_set (1);
  }

