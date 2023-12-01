/*======================================================================
  
  nv-vlc-client 

  nc-vlc-client/src/message.c

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <ncursesw/curses.h>
#include "app_context.h" 
#include "colour.h" 

extern WINDOW *message_window;

/*======================================================================
  
  message_show

======================================================================*/
void message_show (const char *message, const AppContext *context)
  {
  char *nlpos = strchr (message, '\r');
  if (nlpos) *nlpos = 0;
  werase (message_window);
  if (context->colour)
    wattron (message_window, COLOR_PAIR (CPAIR_BOX));
  box (message_window, 0, 0);
  if (context->colour)
    wattroff (message_window, COLOR_PAIR (CPAIR_BOX));
  mvwaddnstr (message_window, 1, 1, message, COLS - 2);
  wrefresh (message_window);
  }

