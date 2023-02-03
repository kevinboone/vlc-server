/*======================================================================
  
  vlc-rest-server

  client/src/cmd_play.c

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
  
  cmd_play

======================================================================*/
int cmd_play (const char *argv0, int argc, char **argv, 
    const CmdContext *context)
  {
  if (argc >= 1)
    {
    LibVlcServerClient *client = libvlc_server_client_new 
      (context->host, context->port);
    char *msg = NULL;
    VSApiError err_code = 0;
    int i = 1;
    int files_added = 0;
    while (err_code == 0 && i <= argc)
      {
      libvlc_server_client_play (client, &err_code, &msg, argv[i]);
      if (err_code)
	{
	fprintf (stderr, "%s: ", argv[i]);
	cmd_handle_response (argv0, err_code, msg);
	}
      else
	files_added++;
      i++;
      }

    if (files_added >= 2)
      printf ("Added %d items to playlist\n", files_added);

    libvlc_server_client_destroy (client);
    return 0;
    }
  else
    {
    fprintf (stderr, "%s: no URLs specified\n", argv0);
    return EINVAL;
    }
  }




