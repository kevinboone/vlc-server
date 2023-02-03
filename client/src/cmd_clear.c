/*======================================================================
  
  vlc-rest-server

  client/src/cmd_clear.c

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
  
  cmd_clear

======================================================================*/
int cmd_clear (const char *argv0, int argc, char **new_argv, 
    const CmdContext *context)
  {
  LibVlcServerClient *client = libvlc_server_client_new 
    (context->host, context->port);
  char *msg = NULL;
  VSApiError err_code;
  libvlc_server_client_clear (client, &err_code, &msg);
  cmd_handle_response (argv0, err_code, msg);

  libvlc_server_client_destroy (client);

  return 0;
  }


