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
#include "app_context.h" 
#include "view_control.h" 
#include "view_albums.h" 
#include "view_playlist.h" 
#include "view_main_menu.h" 
#include "colour.h" 

WINDOW *status_window = NULL; 
WINDOW *message_window = NULL; 
WINDOW *main_window = NULL; 
 
/*======================================================================
  
  show_help

======================================================================*/
void show_help (void)
  {
  printf ("Usage: " NAME " [options] {command} [arguments]\n");
  printf ("  -c,--colour             Use colour\n");
  printf ("  -h,--host {hostname}    VLC server host (localhost)\n");
  printf ("  -k,--kiosk              Kiosk mode (no exit)\n");
  printf ("  -p,--port {number}      VLC server port (30000)\n");
  printf ("  -t,--title {text}       Set displayed program title\n");
  printf ("  -v                      show version\n");
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
  char *title = strdup (NAME);
  int port = 30000;
  BOOL flag_version = FALSE;
  BOOL flag_help = FALSE;
  BOOL flag_colour = FALSE;
  BOOL flag_kiosk = FALSE;
  /* local is TRUE unless the user gives a hostname for the server. 
     In local mode, the server and this client are assumed to be on the
     same host, which changes the menu selections slightly. It follows
     that users should not set a hostname specifically unless the
     server is actually on a different host. */
  BOOL local = TRUE;
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
      {"colour", no_argument, NULL, 'c'},
      {"title", required_argument, NULL, 't'},
      {"log-level", required_argument, NULL, 'l'},
      {0, 0, 0, 0}
    };

  int opt = 0;
  int ret = 0;

  while (ret == 0)
    {
    int option_index = 0;
    opt = getopt_long (argc, argv, "?h:kl:vp:w:ct:", long_options, &option_index);

    if (opt == -1) break;

    switch (opt)
      {
      case '?':
        flag_help = TRUE;
        break;
      case 'k':
        flag_kiosk = TRUE;
        break;
      case 'c':
        flag_colour = TRUE;
        break;
      case 'h':
        if (host) free (host);
        host = strdup (optarg); 
        local = false;
        break;
      case 't':
        if (title) free (title);
        title = strdup (optarg); 
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
      printf ("Connecting to music server...\n");
      do
        {
        stat = libvlc_server_client_stat (lvsc, &err_code,
           &error);
        if (err_code) 
          {
          if (error) 
            {
            if (!strstr (error, "refused"))
              fprintf (stderr, "%s\n", error);
            free (error);
            }
          printf ("Please wait...\n");
          usleep (3000000); // 3 sec
          }
        } while (err_code);
      printf ("Connected.\n");
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

      AppContext context;
      memset (&context, 0, sizeof (context));
      context.kiosk = flag_kiosk;
      context.local = local;
      context.port = port;
      context.host = host;
      context.title = title;

      if (has_colors() && flag_colour)
        context.colour = TRUE;
      else
        context.colour = FALSE; 

      if (context.colour)
        {
        start_color();
        init_pair (CPAIR_MENU_HIGHLIGHT, 
          FGCOLOUR_MENU_HIGHLIGHT, BGCOLOUR_MENU_HIGHLIGHT);
        init_pair (CPAIR_BOX, 
          FGCOLOUR_BOX, BGCOLOUR_BOX);
        init_pair (CPAIR_MENU_CAPTION, 
          FGCOLOUR_MENU_CAPTION, BGCOLOUR_MENU_CAPTION);
        }

      status_window = subwin (main_window, 5, COLS, 0, 0);
      message_window = subwin (main_window, 3, COLS, LINES - 3, 0);

      status_update (lvsc, &context);
      view_main_menu (main_window, lvsc, LINES - 3 - 5, COLS, 
         5, 0, &context); 
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
  if (title) free (title);

  if (ret == -1) ret = 0;
  return ret;
  }

