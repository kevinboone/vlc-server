/*======================================================================
  
  nv-vlc-client 

  nc-vlc-client/src/view_albums.c

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/

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

/*======================================================================
  
  populate_album_list

======================================================================*/
static VSList *populate_album_list (LibVlcServerClient *lvsc, 
         char **error)
  {
  VSApiError err_code;
  VSList *list = libvlc_server_client_list_albums (lvsc, 
      NULL, &err_code, error);
  if (list)
    return list;
  else
    return NULL;
  }


/*======================================================================
  
  play_album 

======================================================================*/
void play_album (LibVlcServerClient *lvsc, const char *album)
  {
  VSApiError err_code;
  char *error = NULL;
  libvlc_server_client_play_album (lvsc, 
     &err_code, &error, album);
  if (error)
    {
    message_show (error);
    free (error);
    }
  }

/*======================================================================
  
  view_albums 

======================================================================*/
void view_albums (WINDOW *main_window, LibVlcServerClient *lvsc, 
       int h, int w, int row, int col)
  {
  char *error = NULL;
  message_show ("Loading albums...");
  VSList *album_list = populate_album_list (lvsc, &error);

  if (error)
    {
    message_show (error); 
    free (error);
    }
  else
    {
    view_list (main_window, lvsc, h, w, row, col, album_list, play_album,
       "Albums", FALSE);
    }

  if (album_list) vs_list_destroy (album_list);
  }
