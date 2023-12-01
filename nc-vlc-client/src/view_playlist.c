/*======================================================================
  
  nv-vlc-client 

  nc-vlc-client/src/view_playlist.c

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
#include "view_playlist.h" 

/*======================================================================
  
  populate_playlist

======================================================================*/
static VSList *populate_playlist (LibVlcServerClient *lvsc, 
         const AppContext *context, char **error)
  {
  (void)error;
  VSApiError err_code;
  char *msg = NULL;
  VSList *ret = NULL;

  VSPlaylist *playlist =  libvlc_server_client_get_playlist 
        (lvsc, &err_code, &msg);

  if (playlist)
    {
    ret = vs_list_create (free);
    int l = vs_playlist_length (playlist);
    for (int i = 0; i < l; i++)
      {
      const char *mrl = vs_playlist_get (playlist, i);
      const char *p = strrchr (mrl, '/');
      char *s;
      if (p)
        asprintf (&s, "[%03d] %s", i + 1, strdup (p + 1)); 
      else
        asprintf (&s, "[%03d] %s", i + 1, mrl); 

      vs_list_append (ret, s); 
      }
    vs_playlist_destroy (playlist);
    }
  else
    {
    if (msg)
      {
      message_show (msg, context);
      free (msg);
      }
    }
  return ret;
  }


/*======================================================================
  
  play_index

======================================================================*/
void play_index (LibVlcServerClient *lvsc, const char *line, 
       const AppContext *context)
  {
  (void)context;
  VSApiError err_code;
  char *error = NULL;
  int index = atoi (line + 1) - 1;
  if (index < 0) index = 0;
  libvlc_server_client_set_index (lvsc, 
     &err_code, &error, index);
  if (error)
    {
    message_show (error, context);
    free (error);
    }
  }

/*======================================================================
  
  view_playlist 

======================================================================*/
void view_playlist (WINDOW *main_window, LibVlcServerClient *lvsc, 
       int h, int w, int row, int col, const AppContext *context)
  {
  char *error = NULL;
  message_show ("Loading playlist...", context);
  VSList *playlist = populate_playlist (lvsc, context, &error);

  if (playlist)
    {
    view_list (main_window, lvsc, h, w, row, col, playlist, 
       play_index, "Playlist", context);
    vs_list_destroy (playlist);
    }
  else
    {
    if (error)
      {
      message_show (error, context); 
      free (error);
      }
    }
  }

