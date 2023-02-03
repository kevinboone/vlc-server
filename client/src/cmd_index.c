/*======================================================================
  
  vlc-rest-server

  client/src/cmd_index.c

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
  
  cmd_index

======================================================================*/
int cmd_index (const char *argv0, int argc, char **new_argv, 
    const CmdContext *context)
  {
  LibVlcServerClient *client = libvlc_server_client_new 
    (context->host, context->port);
  char *msg = NULL;
  VSApiError err_code;
  if (argc >= 1)
    {
    int index = atoi (new_argv[1]);
    libvlc_server_client_set_index (client, &err_code, &msg, index);
    cmd_handle_response (argv0, err_code, msg);
    }
  else
    {
    LibVlcServerStat *stat = libvlc_server_client_stat 
      (client, &err_code, &msg);

    if (stat)
      {
      int index = libvlc_server_stat_get_index (stat);
      printf ("%d\n", index);

      libvlc_server_stat_destroy (stat);
      }
    else
      cmd_handle_response (argv0, err_code, msg);
    }

  libvlc_server_client_destroy (client);

  return 0;
  }





