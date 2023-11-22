/*======================================================================
  
  nv-vlc-client 

  nc-vlc-client/src/main.c

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <locale.h>
#include <unistd.h>
#include <vlc-server/api-client.h>
#include <ncursesw/curses.h>
#include "message.h" 
#include "status.h" 
#include "view_control.h" 
#include "view_albums.h" 
#include "view_playlist.h" 
#include "view_main_menu.h" 

WINDOW *status_window = NULL; 
WINDOW *message_window = NULL; 
WINDOW *main_window = NULL; 
 
/*======================================================================
  
  show_help

======================================================================*/
void show_help (void)
  {
  printf ("Usage: " NAME " [options] {command} [arguments]\n");
  printf ("  -v                      show version\n");
  printf ("  -h,--host {hostname}    VLC server host (localhost)\n");
  printf ("  -p,--port {number}      VLC server port (30000)\n");
  printf ("  -k,--kiosk              Kiosk mode (no exit)\n");
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
  BOOL flag_kiosk = FALSE;
  // No point setting a sensible log level, as the logging will be
  //   lost under ncurses windows anyway.
  int log_level = VSLOG_ERROR;

  static struct option long_options[] =
    {
      {"help", no_argument, NULL, '?'},
      {"version", no_argument, NULL, 'v'},
      {"port", required_argument, NULL, 'p'},
      {"host", required_argument, NULL, 'h'},
      {"kiosk", no_argument, NULL, 'k'},
      {"log-level", required_argument, NULL, 'l'},
      {0, 0, 0, 0}
    };

  int opt = 0;
  int ret = 0;

  while (ret == 0)
    {
    int option_index = 0;
    opt = getopt_long (argc, argv, "?h:kl:vp:w:", long_options, &option_index);

    if (opt == -1) break;

    switch (opt)
      {
      case '?':
        flag_help = TRUE;
        break;
      case 'k':
        flag_kiosk = TRUE;
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
    // I don't know why I need this, but UTF-8 characters don't display
    //   correctly otherwise.
    setlocale(LC_ALL, ""); 

    vs_log_set_level (log_level);
    LibVlcServerClient *lvsc = libvlc_server_client_new (host, port);

    VSServerStat *stat = NULL;
    VSApiError err_code;
    char *error = NULL;
    if (flag_kiosk)
      {
      do
        {
        stat = libvlc_server_client_stat (lvsc, &err_code,
           &error);
        if (err_code) 
          {
          if (error) 
            {
            fprintf (stderr, "%s\n", error);
            free (error);
            }
          printf ("Waiting for server to start...\n");
          usleep (3000000);
          }
        } while (err_code);
      }
    else
      {
      stat = libvlc_server_client_stat (lvsc, &err_code,
        &error);
      }

    if (err_code == 0)
      {
      if (stat) vs_server_stat_destroy (stat);

      main_window = initscr();
      status_window = subwin (main_window, 5, COLS, 0, 0);
      message_window = subwin (main_window, 3, COLS, LINES - 3, 0);

      status_update (lvsc);
      view_main_menu (main_window, lvsc, LINES - 3 - 5, COLS, 5, 0, flag_kiosk);
      //view_albums (main_window, lvsc, LINES - 3 - 5, COLS, 5, 0);

      delwin (status_window);
      delwin (message_window);
      delwin (main_window);
      endwin ();
      refresh ();
      }
    else
      {
      fprintf (stderr, "%s: can't initialize connection", argv[0]);
      if (error)
        {
        fprintf (stderr, ":\n%s", error);
        free (error);
        }
     
      fprintf (stderr, "\n");
      ret = err_code;
      }

    libvlc_server_client_destroy (lvsc);
    }

  if (host) free (host);

  if (ret == -1) ret = 0;
  return ret;
  }

