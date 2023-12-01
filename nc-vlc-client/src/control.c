/*======================================================================
  
  nv-vlc-client 

  nc-vlc-client/src/control.c

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <vlc-server/api-client.h>
#include <ncursesw/curses.h>
#include "keys.h" 
#include "message.h" 
#include "status.h" 
#include "control.h" 
#include "info_window.h" 

/*======================================================================
  
  control_run_scanner

======================================================================*/
void control_run_scanner (LibVlcServerClient *lvsc,
       const AppContext *context)
  {
  char *message = NULL;
  VSApiError err_code;
  libvlc_server_client_scan (lvsc, &err_code, &message);
  if (message)
    {
    message_show (message, context);
    free (message);
    }
  }

/*======================================================================
  
  control_toggle_pause 

======================================================================*/
void control_toggle_pause (LibVlcServerClient *lvsc,
       const AppContext *context)
  {
  char *message = NULL;
  VSApiError err_code;
  libvlc_server_client_toggle_pause (lvsc, &err_code, &message);
  if (message)
    {
    message_show (message, context);
    free (message);
    }
  }

/*======================================================================
  
  control_stop

======================================================================*/
void control_stop (LibVlcServerClient *lvsc,
       const AppContext *context)
  {
  char *message = NULL;
  VSApiError err_code;
  libvlc_server_client_stop (lvsc, &err_code, &message);
  if (message)
    {
    message_show (message, context);
    free (message);
    }
  }

/*======================================================================
  
  control_next

======================================================================*/
void control_next (LibVlcServerClient *lvsc,
       const AppContext *context)
  {
  char *message = NULL;
  VSApiError err_code;
  libvlc_server_client_next (lvsc, &err_code, &message);
  if (message)
    {
    message_show (message, context);
    free (message);
    }
  }

/*======================================================================
  
  control_prev

======================================================================*/
void control_prev (LibVlcServerClient *lvsc,
       const AppContext *context)
  {
  char *message = NULL;
  VSApiError err_code;
  libvlc_server_client_prev (lvsc, &err_code, &message);
  if (message)
    {
    message_show (message, context);
    free (message);
    }
  }

/*======================================================================
  
  control_volume_down

======================================================================*/
void control_volume_down (LibVlcServerClient *lvsc,
       const AppContext *context)
  {
  char *message = NULL;
  VSApiError err_code;
  libvlc_server_client_volume_down (lvsc, &err_code, &message);
  if (message)
    {
    message_show (message, context);
    free (message);
    }
  else
    {
    VSServerStat *stat = libvlc_server_client_stat 
         (lvsc, &err_code, NULL);
    if (err_code == 0)
      {
      char s[30];
      sprintf (s, "Volume %d%%", 
         vs_server_stat_get_volume (stat));
      message_show (s, context);
      }
    if (stat) 
      vs_server_stat_destroy (stat);
    }
  }

/*======================================================================
  
  control_volume_up

======================================================================*/
void control_volume_up (LibVlcServerClient *lvsc, 
       const AppContext *context)
  {
  char *message = NULL;
  VSApiError err_code;
  libvlc_server_client_volume_up (lvsc, &err_code, &message);
  if (message)
    {
    message_show (message, context);
    free (message);
    }
  else
    {
    VSServerStat *stat = libvlc_server_client_stat 
         (lvsc, &err_code, NULL);
    if (err_code == 0)
      {
      char s[30];
      sprintf (s, "Volume %d%%", 
         vs_server_stat_get_volume (stat));
      message_show (s, context);
      }
    if (stat) 
      vs_server_stat_destroy (stat);
    }
  }

/*======================================================================
  
  control_play_random_album 

======================================================================*/
void control_play_random_album (LibVlcServerClient *lvsc, 
       const AppContext *context)
  {
  char *message = NULL;
  VSApiError err_code;
  libvlc_server_client_play_random_album (lvsc, &err_code, &message);
  if (message)
    {
    message_show (message, context);
    free (message);
    }
  }

