/*======================================================================
  
  vlc-rest-server

  client/src/main.c

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <vlc-server/api-client.h>
#include <vlc-server/vs_search_constraints.h>
#include "cmd.h"
 
/*======================================================================
  
  command table 

======================================================================*/
typedef struct 
  {
  const char *name;
  CmdFn fn;
  const char *help;
  } CmdEntry;

static CmdEntry cmd_table[] = 
  {
  { "add-urls", cmd_add_urls, "add one or more URLs or files to the playlist" },
  { "clear", cmd_clear , "clear the playlist" },
  { "index", cmd_index , "selects a specific item in the playlist" },
  { "list-albums", cmd_list_albums, 
       "list albums in media database" },
  { "list-artists", cmd_list_artists, 
       "list artists in media database" },
  { "list-composers", cmd_list_composers, 
       "list composers in media database" },
  { "list-dirs", cmd_list_dirs, 
       "list directories, relative to media root" },
  { "list-files", cmd_list_files, 
       "list files, relative to media root" },
  { "list-genres", cmd_list_genres, 
       "list genres in media database" },
  { "list-tracks", cmd_list_tracks, 
       "list tracks in media database" },
  { "next", cmd_next , "skip to next item in playlist" },
  { "pause", cmd_pause , "pause playback" },
  { "play", cmd_play , "play the items immediately" },
  { "playlist", cmd_playlist , "show the playlist" },
  { "prev", cmd_prev , "skip to previous item in the playlist" },
  { "shutdown", cmd_shutdown , "shut down the server" },
  { "scan", cmd_scan , "start a rescan of the media directory" },
  { "start", cmd_start , "play from start, or resume if paused" },
  { "stat", cmd_stat , "show the current player status" },
  { "stop", cmd_stop , "stop playback, reset to start of playlist" },
  { "volume", cmd_volume , "get or set audio volume, in range 0-100" },
  { NULL, NULL, NULL }
  };


/*======================================================================
  
  show_help

======================================================================*/
void show_help (void)
  {
  printf ("Usage: " NAME " [options] {command} [arguments]\n");
  printf ("  -v                      show version\n");
  printf ("  -h,--host {hostname}    VLC server host (localhost)\n");
  printf ("  -p,--port {number}      VLC server port (30000)\n");
  printf ("\nCommands:\n");
  int i = 0;
  BOOL found = FALSE;
  CmdEntry *e = &cmd_table[0];
  do
    {
    printf ("  %s -- %s\n", e->name, e->help);
    i++;
    e = &cmd_table[i];
    } while (e->name && !found);
  }

/*======================================================================
  
  show_version 

======================================================================*/
void show_version (void)
  {
  printf (NAME " version " VERSION "\n");
  printf ("Copyright (c)2022 Kevin Boone\n");
  printf ("Distributed under the terms of the GNU Public Licence, v3.0\n");
  }

/*======================================================================
  
  main

======================================================================*/
int main (int argc, char **argv)
  {
  char *host = strdup ("localhost");
  int port = 30000;
  BOOL flag_version = FALSE;
  BOOL flag_help = FALSE;
  int log_level = VSLOG_INFO;

  static struct option long_options[] =
    {
      {"help", no_argument, NULL, '?'},
      {"version", no_argument, NULL, 'v'},
      {"port", required_argument, NULL, 'p'},
      {"host", required_argument, NULL, 'h'},
      {"log-level", required_argument, NULL, 'l'},
      {0, 0, 0, 0}
    };

  int opt = 0;
  int ret = 0;

  while (ret == 0)
    {
    int option_index = 0;
    opt = getopt_long (argc, argv, "?h:l:vp:w:", long_options, &option_index);

    if (opt == -1) break;

    switch (opt)
      {
      case '?':
        flag_help = TRUE;
        break;
      case 'h':
        if (host) free (host);
        host = strdup (optarg); 
        break;
      case 'v':
        flag_version = TRUE;
        break;
      case 'p':
        port = atoi (optarg);
        break;
      case 'l':
        log_level = atoi (optarg);
        break;
      default:
        ret = 1;
      }
  }

  if (flag_version)
    {
    show_version();
    ret = -1;
    }

  if (flag_help && ret == 0)
    {
    show_help();
    ret = -1;
    }

  if (ret == 0)
    {
    vs_log_set_level (log_level);
    if (optind != argc)
      {
      LibVlcServerClient *lvsc = libvlc_server_client_new (host, port);

      libvlc_server_client_destroy (lvsc);
      
      int i = 0;
      BOOL found = FALSE;
      CmdEntry *e = &cmd_table[0];
      do
        {
        if (strcmp (argv[optind], e->name) == 0)
          {
          int new_argc = argc - optind - 1;
          char **new_argv = &(argv[optind]);
          CmdContext context;
          context.host = host;
          context.port = port;
          VSSearchConstraints *mdc = vs_search_constraints_new();
          context.mdc = mdc;
          ret = e->fn (e->name, new_argc, new_argv, &context);
          vs_search_constraints_destroy (mdc);
          found = TRUE;
          }
        i++;
        e = &cmd_table[i];
        } while (e->name && !found);
      if (!found)
        {
        fprintf (stderr, "%s: bad command '%s'\n", argv[0], argv[optind]); // TODO
        ret = EINVAL;
        }
      }
    else
      {
      fprintf (stderr, "%s: no command specified\n", argv[0]); // TODO
      ret = EINVAL;
      }
    }

  if (host) free (host);

  if (ret == -1) ret = 0;
  return ret;
  }

/*======================================================================
  
  cmd_handle_response

======================================================================*/
extern void cmd_handle_response (const char *argv0, int err_code, char *msg)
  {
  if (err_code)
    {
    if (msg) 
      {
      fprintf (stderr, "%s: %s: %s\n", argv0, 
        vs_util_strerror(err_code), msg);
      free (msg);
      }
    else
      {
      fprintf (stderr, "%s: %s\n", argv0, vs_util_strerror (err_code));
      }
    }
  else
    {
    if (msg) 
      {
      printf ("%s: %s\n", argv0, msg);
      free (msg);
      }
    }
  }



