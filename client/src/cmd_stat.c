/*======================================================================
  
  vlc-rest-server

  client/src/cmd_stat.c

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
  
  cmd_stat_print_time
  Splits a msec time into HH:MM:SS

======================================================================*/
void cmd_stat_print_time (int time)
  {
  if (time >= 0) 
    {
    int hours = time / 3600000;
    printf ("%02d:", hours);
    time -= hours * 3600000;
    int mins = time / 60000; 
    printf ("%02d:", mins);
    time -= mins * 60000;
    printf ("%02d", time / 1000);
    }
  else
    printf ("??:??:??");
  }
 
/*======================================================================
  
  cmd_stat

======================================================================*/
int cmd_stat (const char *argv0, int argc, char **new_argv, 
    const CmdContext *context)
  {
  LibVlcServerClient *client = libvlc_server_client_new 
    (context->host, context->port);
  char *msg = NULL;
  VSApiError err_code;
  VSServerStat *stat = libvlc_server_client_stat (client, &err_code, &msg);

  if (stat)
    {
    printf ("current media url: ");
    const char *mrl = vs_server_stat_get_mrl (stat);
    if (mrl && mrl[0])
      {
      printf ("%s\n", mrl);
      }
    else
      {
      printf ("none\n");
      }
    VSApiTransportStatus ts = vs_server_stat_get_transport_status (stat);
    printf ("transport: ");
    switch (ts)
      {
      case VSAPI_TS_ERROR: printf ("error\n"); break;
      case VSAPI_TS_STOPPED: printf ("stopped\n"); break;
      case VSAPI_TS_PLAYING: printf ("playing\n"); break;
      case VSAPI_TS_PAUSED: printf ("paused\n"); break;
      case VSAPI_TS_OPENING: printf ("opening\n"); break;
      case VSAPI_TS_BUFFERING: printf ("buffering\n"); break;
      case VSAPI_TS_ENDED: printf ("ended\n"); break;
      }
    int duration = vs_server_stat_get_duration (stat);
    printf ("duration: ");
    cmd_stat_print_time (duration);
    printf ("\n");

    int position = vs_server_stat_get_position (stat);
    printf ("position: ");
    cmd_stat_print_time (position);
    printf ("\n");

    int index = vs_server_stat_get_index (stat);
    printf ("playlist index: %d\n", index);

    int volume = vs_server_stat_get_volume (stat);
    printf ("volume: %d\n", volume);

    const VSMetadata *amd = vs_server_stat_get_metadata (stat);

    const char *title = vs_metadata_get_title (amd);
    printf ("title: %s\n", title);
    const char *album = vs_metadata_get_album (amd);
    printf ("album: %s\n", album);
    const char *artist = vs_metadata_get_artist (amd);
    printf ("artist: %s\n", artist);
    const char *genre = vs_metadata_get_genre (amd);
    printf ("genre: %s\n", genre);
    const char *composer = vs_metadata_get_composer (amd);
    printf ("composer: %s\n", composer);

    int scanner_progress = vs_server_stat_get_scanner_progress (stat);
    if (scanner_progress < 0)
      printf ("scanner: not running\n"); 
    else
      printf ("scanner: %d files scanned\n", scanner_progress);

    vs_server_stat_destroy (stat);
    }
  else
    cmd_handle_response (argv0, err_code, msg);

  libvlc_server_client_destroy (client);
  return 0;
  }


