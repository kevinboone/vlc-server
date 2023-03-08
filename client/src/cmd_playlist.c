/*======================================================================
  
  vlc-rest-server

  client/src/cmd_playlist.c

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <vlc-server/api-client.h>
#include "cmd.h"
 
/*======================================================================
  
  cmd_playlist

======================================================================*/
int cmd_playlist (const char *argv0, int argc, 
    char **new_argv, const CmdContext *context)
  {
  LibVlcServerClient *client = libvlc_server_client_new 
    (context->host, context->port);
  char *msg = NULL;
  VSApiError err_code = 0;

  VSPlaylist *playlist = libvlc_server_client_get_playlist
      (client, &err_code, &msg);

  if (playlist)
    {
    int len = vs_playlist_length (playlist);
    for (int i = 0; i < len; i++)
      {
      printf ("%s\n", vs_playlist_get (playlist, i));
      }
    vs_playlist_destroy (playlist);
    }
  else
    cmd_handle_response (argv0, err_code, msg);
 
  libvlc_server_client_destroy (client);

  return err_code;
  }



