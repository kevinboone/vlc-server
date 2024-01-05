/*======================================================================
  
  vlc-rest-server

  client/src/cmd_add_urls.c

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
  
  cmd_play_stream

======================================================================*/
int cmd_play_stream (const char *argv0, int argc, char **argv, 
    const CmdContext *context)
  {
  if (argc == 1)
    {
    LibVlcServerClient *client = libvlc_server_client_new 
      (context->host, context->port);

    char *msg = NULL;
    VSApiError err_code = 0;
    libvlc_server_client_play_stream (client, &err_code, &msg, argv[1]);
    if (err_code)
      {
      fprintf (stderr, "%s: ", argv[1]);
      cmd_handle_response (argv0, err_code, msg);
      }
     
    libvlc_server_client_destroy (client);
    return 0;
    }
  else
    {
    fprintf (stderr, "%s: no stream name specified\n", argv0);
    return EINVAL;
    }
  }

