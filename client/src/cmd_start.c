/*======================================================================
  
  vlc-rest-server

  client/src/cmd_start.c

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
  
  cmd_start

======================================================================*/
int cmd_start (const char *argv0, int argc, char **argv, 
    const CmdContext *context)
  {
  VSApiError err_code = 0;
  char *msg = NULL;
  LibVlcServerClient *client = libvlc_server_client_new 
      (context->host, context->port);

  libvlc_server_client_start (client, &err_code, &msg);
  cmd_handle_response (argv0, err_code, msg);
  return err_code;
  }




