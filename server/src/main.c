/*======================================================================
  
  vlc-rest-server

  main.c

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <vlc-server/vs_log.h> 
#include "http_server.h" 
#include "player.h" 

/*======================================================================
  
  show_help

======================================================================*/
void show_help (void)
  {
  printf ("Usage: " NAME " [options] {command} [arguments]\n");
  printf ("  -a,--patterns {patterns} list of accepted file patterns\n");
  printf ("  -f,--foreground          run in foreground\n");
  printf ("     --list-devices        list output devices\n");
  printf ("  -l,--log-level {0..4}    log verbosity (2)\n");
  printf ("  -n,--folders {number}    number of folders per page\n");
  printf ("  -p,--port {number}       server port (30000)\n");
  printf ("  -r,--media-root {dir}    media root directory ($HOME/Music)\n");
  printf ("  -v                       show version\n");
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
  
  show_devices

======================================================================*/
void show_devices (void)
  {
  VSApiError e;
  Player *player = player_new (NULL, "", 0, NULL, &e);
  player_list_devices (player);
  player_destroy (player);
  }

/*======================================================================
  
  main 

======================================================================*/
int main (int argc, char* argv[])
  {
  int ret = 0;
  int port = 30000; 
  int log_level = VSLOG_INFO; 
  int folders_per_page = 30;
  char *patterns = NULL;
  char *media_root = NULL;
  char *canon_media_root = NULL;
  BOOL flag_foreground = FALSE;
  BOOL flag_version = FALSE;
  BOOL flag_help = FALSE;
  BOOL flag_list_devices = FALSE;

  static struct option long_options[] =
    {
      {"help", no_argument, NULL, '?'},
      {"version", no_argument, NULL, 'v'},
      {"port", required_argument, NULL, 'p'},
      {"folders", required_argument, NULL, 'n'},
      {"patterns", required_argument, NULL, 'a'},
      {"media-root", required_argument, NULL, 'r'},
      {"log-level", required_argument, NULL, 'l'},
      {"foreground", no_argument, NULL, 'f'},
      {"list-devices", no_argument, NULL, 'i'},
      {0, 0, 0, 0}
    };

  int opt = 0;

  while (ret == 0)
    {
    int option_index = 0;
    opt = getopt_long (argc, argv, "?d:fil:vp:a:r:n:", long_options, &option_index);

    if (opt == -1) break;

    switch (opt)
      {
      case 'f':
        flag_foreground = TRUE;
        break;
      case '?':
        flag_help = TRUE;
        break;
      case 'i':
        flag_list_devices = TRUE;
        break;
      case 'v':
        flag_version = TRUE;
        break;
      case 'p':
        port = atoi (optarg);
        break;
      case 'n':
        folders_per_page = atoi (optarg);
        break;
      case 'l':
        log_level = atoi (optarg);
        break;
      case 'a':
        patterns = strdup (optarg);
        break;
      case 'r':
        media_root = strdup (optarg);
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

  if (flag_list_devices && ret == 0)
    {
    vs_log_syslog = FALSE;
    vs_log_console = FALSE;
    show_devices();
    ret = -1;
    }

  if (ret == 0)
    {
    if (flag_foreground)
      {
      vs_log_syslog = FALSE;
      vs_log_console = TRUE;
      }
    else
      {
      vs_log_syslog = TRUE;
      vs_log_console = FALSE;
      daemon (0, 0);
      }

    vs_log_set_level (log_level);

    if (media_root == NULL)
      {
      asprintf (&media_root, "%s/Music", getenv("HOME"));
      vs_log_warning ("media-root not set; defaulting to %s", media_root);
      }

    canon_media_root = canonicalize_file_name (media_root);

    if (access (canon_media_root, X_OK) != 0)
      {
      vs_log_warning ("media-root %s is not a readable directory.", 
        media_root); 
      }

    HttpServer *http_server = http_server_new (port, media_root, 
         folders_per_page);
    if (http_server_start (http_server))
      {
      char **new_argv = argv + optind;
      int new_argc = argc - optind;
      VSApiError e = 0;
      Player *player = player_new (patterns, canon_media_root, 
         new_argc, (const char *const *)new_argv, &e);
      if (e == 0)
        {
        http_server_set_player (http_server, player);
        while (http_server_is_running (http_server))
	  {
	  sleep (1);
	  }
        }
      else
        {
// TODO
        }
      if (player) player_destroy (player);
      }
    else
      {
      vs_log_error ("Can't start HTTP server -- is port %d in use?", port);
      ret = errno;
      }

    http_server_stop (http_server);
    http_server_destroy (http_server);
    }

  if (patterns) free (patterns);
  if (media_root) free (media_root);
  if (canon_media_root) free (canon_media_root);
  if (ret == -1) ret = 0;
  return ret;
  }


