/*======================================================================
  
  vlc-server

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
#include "media_database.h" 
#include "scanner.h" 
#include "file.h" 

#define DATABASE_FILENAME "vlc-server-db.sqlite"

/*======================================================================
  
  show_help

======================================================================*/
void show_help (void)
  {
  printf ("Usage: " NAME " [options] {command} [arguments]\n");
  printf ("  -a,--patterns {patterns} list of accepted file patterns\n");
  printf ("  -d,--database {file}     media database file\n");
  printf ("  -e,--entries {number}    max number of search results per page\n");
  printf ("  -f,--foreground          run in foreground\n");
  printf ("     --list-devices        list output devices\n");
  printf ("  -l,--log-level {0..4}    log verbosity (2)\n");
  printf ("  -n,--name {string}       server name (hostname)\n");
  printf ("  -p,--port {number}       server port (30000)\n");
  printf ("  -q,--quickscan           quick scan of the media database\n");
  printf ("  -r,--media-root {dir}    media root directory ($HOME/Music)\n");
  printf ("  -s,--scan                full scan of the media database\n");
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
  char *patterns = strdup (file_get_default_extensions());
  char *media_root = NULL;
  char *canon_media_root = NULL;
  char *mdb_file = NULL;
  char instance_name[200];
  BOOL flag_foreground = FALSE;
  BOOL flag_version = FALSE;
  BOOL flag_help = FALSE;
  BOOL flag_list_devices = FALSE;
  BOOL quickscan = FALSE;
  BOOL scan = FALSE; 

  static struct option long_options[] =
    {
      {"help", no_argument, NULL, '?'},
      {"patterns", required_argument, NULL, 'a'},
      {"database", required_argument, NULL, 'd'},
      {"entries", required_argument, NULL, 'e'},
      {"foreground", no_argument, NULL, 'f'},
      {"list-devices", no_argument, NULL, 'i'},
      {"version", no_argument, NULL, 'v'},
      {"log-level", required_argument, NULL, 'l'},
      {"name", required_argument, NULL, 'n'},
      {"port", required_argument, NULL, 'p'},
      {"quickscan", no_argument, NULL, 'q'},
      {"media-root", required_argument, NULL, 'r'},
      {"scan", no_argument, NULL, 's'},
      {0, 0, 0, 0}
    };

  gethostname (instance_name, sizeof (instance_name) - 1);

  int opt = 0;

  while (ret == 0)
    {
    int option_index = 0;
    opt = getopt_long (argc, argv, "?d:fil:vp:a:r:e:fsqn:", long_options, &option_index);

    if (opt == -1) break;

    switch (opt)
      {
      case 'f':
        flag_foreground = TRUE;
        break;
      case 's':
        scan = TRUE;
        break;
      case 'q':
        quickscan = TRUE;
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
      case 'e':
        folders_per_page = atoi (optarg);
        break;
      case 'n':
        strncpy (instance_name, optarg, sizeof (instance_name) - 1);
        break;
      case 'l':
        log_level = atoi (optarg);
        break;
      case 'a':
        patterns = strdup (optarg);
        break;
      case 'd':
        mdb_file = strdup (optarg);
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
    vs_log_set_level (log_level);

    if (media_root == NULL)
      {
      asprintf (&media_root, "%s/Music", getenv("HOME"));
      vs_log_warning ("media-root not set; defaulting to %s", media_root);
      }

    if (mdb_file == NULL)
      {
      asprintf (&mdb_file, "%s/%s", media_root, DATABASE_FILENAME);
      vs_log_info ("Media database not set; defaulting to %s", mdb_file);
      }

    canon_media_root = canonicalize_file_name (media_root);

    if (access (canon_media_root, X_OK) != 0)
      {
      ret = errno;
      vs_log_error ("media-root %s is not a readable directory.", 
        media_root); 
      }

    if (ret == 0)
      {
      MediaDatabase *mdb = media_database_create (mdb_file);
      char *error = NULL;
      media_database_init (mdb, TRUE, &error);
      if (error)
        {
        fprintf (stderr, "Can't initialize media database: %s\n", error);
        free (error);
        }

      // We can continue without a media database

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

     if (quickscan || scan)
       {
       if (media_database_is_init (mdb))
         scanner_run (canon_media_root, scan, mdb, patterns);
       else
         {
         vs_log_error ("Can't scan -- database not initialized");
         ret = -1; 
         }
       }
     else
       {
       HttpServer *http_server = http_server_new (port, media_root, 
             folders_per_page, mdb, instance_name);
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
      media_database_destroy (mdb);
      }
    }

  if (patterns) free (patterns);
  if (media_root) free (media_root);
  if (canon_media_root) free (canon_media_root);
  if (mdb_file) free (mdb_file);
  if (ret == -1) ret = 0;
  return ret;
  }


