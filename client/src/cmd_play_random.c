/*======================================================================
  
  vlc-rest-server

  client/src/cmd_play_random.c

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
  
  cmd_play_random_album

======================================================================*/
int cmd_play_random_album (const char *argv0, int argc, char **argv, 
    const CmdContext *context)
  {
  LibVlcServerClient *client = libvlc_server_client_new 
    (context->host, context->port);
  char *msg = NULL;
  VSApiError err_code;
  libvlc_server_client_play_random_album (client, &err_code, &msg);
  cmd_handle_response (argv0, err_code, msg);

  libvlc_server_client_destroy (client);
  return 0;
  }

/*======================================================================
  
  cmd_play_random_tracks

======================================================================*/
int cmd_play_random_tracks (const char *argv0, int argc, char **argv, 
    const CmdContext *context)
  {
  LibVlcServerClient *client = libvlc_server_client_new 
    (context->host, context->port);
  char *msg = NULL;
  VSApiError err_code;
  libvlc_server_client_play_random_tracks (client, &err_code, &msg);
  cmd_handle_response (argv0, err_code, msg);

  libvlc_server_client_destroy (client);
  return 0;
  }




