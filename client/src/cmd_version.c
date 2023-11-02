/*======================================================================
  
  vlc-rest-server

  client/src/cmd_version.c

  Copyright (c)2023 Kevin Boone, GPL v3.0

======================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <vlc-server/api-client.h>
#include "cmd.h"
 
/*======================================================================
  
  cmd_version

======================================================================*/
int cmd_version (const char *argv0, int argc, char **new_argv, 
    const CmdContext *context)
  {
  LibVlcServerClient *client = libvlc_server_client_new 
    (context->host, context->port);
  char *msg = NULL;
  VSApiError err_code;
  char *version = libvlc_server_client_get_version (client, 
      &err_code, &msg);
  if (version)
    {
    printf ("Server version: %s\n", version);
    free (version);
    printf ("Client version: %s\n", VERSION);
    }
  else
    cmd_handle_response (argv0, err_code, msg);

  libvlc_server_client_destroy (client);

  return 0;
  }




