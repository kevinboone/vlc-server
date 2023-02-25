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
#include <vlc-server/vs_list.h>
#include <vlc-server/api-client.h>
#include <vlc-server/media_database_constraints.h>
#include <ncurses/curses.h>
#include "message.h" 
#include "status.h" 
#include "keys.h" 
#include "view_misc.h" 
#include "view_playlist.h" 

/*======================================================================
  
  populate_playlist

======================================================================*/
static VSList *populate_playlist (LibVlcServerClient *lvsc, 
         char **error)
  {
  VSApiError err_code;
  char *msg = NULL;
  VSList *ret = NULL;

  LibVlcServerPlaylist *playlist =  libvlc_server_client_get_playlist 
        (lvsc, &err_code, &msg);

  if (playlist)
    {
    ret = vs_list_create (free);
    int l = libvlc_server_playlist_length (playlist);
    for (int i = 0; i < l; i++)
      {
      const char *mrl = libvlc_server_playlist_get (playlist, i);
      const char *p = strrchr (mrl, '/');
      char *s;
      if (p)
        asprintf (&s, "[%03d] %s", i + 1, strdup (p + 1)); 
      else
        asprintf (&s, "[%03d] %s", i + 1, mrl); 

      vs_list_append (ret, s); 
      }
    libvlc_server_playlist_destroy (playlist);
    }
  else
    {
    if (msg)
      {
      message_show (msg);
      free (msg);
      }
    }
  return ret;
  }


/*======================================================================
  
  play_index

======================================================================*/
void play_index (LibVlcServerClient *lvsc, const char *line)
  {
  VSApiError err_code;
  char *error = NULL;
  int index = atoi (line + 1) - 1;
  if (index < 0) index = 0;
  libvlc_server_client_set_index (lvsc, 
     &err_code, &error, index);
  if (error)
    {
    message_show (error);
    free (error);
    }
  }

/*======================================================================
  
  view_playlist 

======================================================================*/
void view_playlist (WINDOW *main_window, LibVlcServerClient *lvsc, 
       int h, int w, int row, int col)
  {
  char *error = NULL;
  message_show ("Loading playlist...");
  VSList *playlist = populate_playlist (lvsc, &error);

  if (playlist)
    {
    view_list (main_window, lvsc, h, w, row, col, playlist, 
       play_index, "Playlist");
    vs_list_destroy (playlist);
    }
  else
    {
    if (error)
      {
      message_show (error); 
      free (error);
      }
    }
  }

