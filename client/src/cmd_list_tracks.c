/*======================================================================
  
  vlc-rest-server

  client/src/cmd_list_tracks.c

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <vlc-server/api-client.h>
#include "string.h"
#include "cmd.h"

/*======================================================================
  
  cmd_list_tracks

======================================================================*/
int cmd_list_tracks (const char *argv0, int argc, char **argv, 
    const CmdContext *context)
  {
  VSApiError err_code = 0;

  const char *where = NULL;
  if (argc >= 1)
    {
    where = argv[1];
    }
 
  char *msg = NULL;
  LibVlcServerClient *client = libvlc_server_client_new 
      (context->host, context->port);
 
  VSList *list = libvlc_server_client_list_tracks (client, 
      where, &err_code, &msg);
  if (list)
    { 
    int l = vs_list_length (list);
    for (int i = 0; i < l; i++)
      {
      printf ("%s\n", (char *)vs_list_get (list, i));
      }
    }
  else
    {
    fprintf (stderr, "%s: %s\n", argv0, msg);
    free (msg);
    }

  vs_list_destroy (list);

  libvlc_server_client_destroy (client);
  return err_code;
  }






