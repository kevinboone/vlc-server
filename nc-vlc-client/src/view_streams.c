/*======================================================================
  
  nv-vlc-client 

  nc-vlc-client/src/view_streams.c

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
#include "view_streams.h" 

extern WINDOW *main_window; 

/*======================================================================
  
  populate_stream_list

======================================================================*/
static VSList *populate_stream_list (LibVlcServerClient *lvsc, 
         char **error)
  {
  VSApiError err_code;
  VSList *list = libvlc_server_client_list_streams (lvsc, 
      NULL, &err_code, error);
  if (list)
    return list;
  else
    return NULL;
  }


/*======================================================================
  
  play_stream 

======================================================================*/
static void play_stream (LibVlcServerClient *lvsc, const char *stream,
     const AppContext *context)
  {
  (void)context;
  VSApiError err_code;
  char *error = NULL;
  libvlc_server_client_play_stream (lvsc, 
     &err_code, &error, stream);
  if (error)
    {
    message_show (error, context);
    free (error);
    }
  }

/*======================================================================
  
  view_streams

======================================================================*/
void view_streams (WINDOW *main_window, LibVlcServerClient *lvsc, 
       int h, int w, int row, int col, const AppContext *context)
  {
  char *error = NULL;
  message_show ("Loading streams...", context);
  VSList *stream_list = populate_stream_list (lvsc, &error);

  if (error)
    {
    message_show (error, context); 
    free (error);
    }
  else
    {
    view_list (main_window, lvsc, h, w, row, col, stream_list, play_stream,
       "Genres", context);
    }

  if (stream_list) vs_list_destroy (stream_list);
  }

