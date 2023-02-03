/*======================================================================
  
  vlc-rest-server

  client/src/cmd_volume.c

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
  
  cmd_volume

======================================================================*/
int cmd_volume (const char *argv0, int argc, char **new_argv, 
    const CmdContext *context)
  {
  LibVlcServerClient *client = libvlc_server_client_new 
    (context->host, context->port);
  char *msg = NULL;
  VSApiError err_code;
  if (argc >= 1)
    {
    if (new_argv[1][0] == 'u')
      {
      libvlc_server_client_volume_up (client, &err_code, &msg);
      }
    else if (new_argv[1][0] == 'd')
      {
      libvlc_server_client_volume_down (client, &err_code, &msg);
      }
    else
      {
      int vol = atoi (new_argv[1]);
      libvlc_server_client_set_volume (client, &err_code, &msg, vol);
      }
    if (err_code)
      cmd_handle_response (argv0, err_code, msg);
    }
  else
    {
    LibVlcServerStat *stat = libvlc_server_client_stat 
      (client, &err_code, &msg);

    if (stat)
      {
      int volume = libvlc_server_stat_get_volume (stat);
      printf ("%d\n", volume);

      libvlc_server_stat_destroy (stat);
      }
    else
      cmd_handle_response (argv0, err_code, msg);
    }

  libvlc_server_client_destroy (client);

  return 0;
  }




