/*======================================================================
  
  vlc-rest-server

  client/src/cmd_storage.c

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
  
  cmd_storage

======================================================================*/
int cmd_storage (const char *argv0, int argc, char **new_argv, 
    const CmdContext *context)
  {
  LibVlcServerClient *client = libvlc_server_client_new 
    (context->host, context->port);
  char *msg = NULL;

  VSApiError err_code;
  VSStorage *storage = libvlc_server_client_storage (client, &err_code, &msg);
  if (storage)
    {
    int capacity_mb = vs_storage_get_capacity_mb (storage);
    int free_mb = vs_storage_get_free_mb (storage);
    int albums = vs_storage_get_albums (storage);
    int tracks = vs_storage_get_tracks (storage);
    printf ("Storage capacity: ");
    if (capacity_mb >= 0)
      printf ("%d Mb\n", capacity_mb);
    else
      printf ("?? Mb\n");
    printf ("Storage free: ");
    if (free_mb >= 0)
      printf ("%d Mb (%d%%)\n", free_mb, 
        capacity_mb > 0 ? free_mb * 100 / capacity_mb : 0);
    else
      printf ("?? Mb\n");
    printf ("Albums: ");
    if (albums >= 0)
      printf ("%d\n", albums);
    else
      printf ("??\n");
    printf ("Tracks: ");
    if (tracks >= 0)
      printf ("%d\n", tracks);
    else
      printf ("??\n");
    vs_storage_destroy (storage);
    }
  else
    cmd_handle_response (argv0, err_code, msg);

  libvlc_server_client_destroy (client);
  return 0;
  }



