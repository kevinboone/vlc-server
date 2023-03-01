/*======================================================================
  
  nv-vlc-client 

  nc-vlc-client/src/status.c

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <vlc-server/api-client.h>
#include <vlc-server/media_database_constraints.h>
#include <vlc-server/vs_defs.h>
#include <ncursesw/curses.h>
#include "view_misc.h" 
#include "message.h" 
#include "status.h" 

extern WINDOW *status_window; 

/*======================================================================
  
  ms_to_hms 

======================================================================*/
static void ms_to_hms (int msec, int *h, int *m, int *s)
  {
  if (msec < 0) msec = 0;
  int sec = msec / 1000;
  *h = sec / 3600;
  sec -= *h * 3600;
  *m = sec / 60;
  sec -= *m * 60;
  *s = sec; 
  }

/*======================================================================
  
  status_update  

======================================================================*/
void status_update (LibVlcServerClient *lvsc)
  {
  werase (status_window);
  box (status_window, 0, 0);
  char *error = NULL;
  VSApiError err_code;
  LibVlcServerStat *stat = libvlc_server_client_stat (lvsc, &err_code,
    &error);

  if (stat)
    {
    VSApiTransportStatus ts = 
      libvlc_server_stat_get_transport_status (stat);
    char *s_stat = "?";
    switch (ts)
      {
      case VSAPI_TS_STOPPED: s_stat = "stopped"; break;
      case VSAPI_TS_PLAYING: s_stat = "playing"; break;
      case VSAPI_TS_PAUSED: s_stat = "paused"; break;
      case VSAPI_TS_OPENING: s_stat = "opening"; break;
      case VSAPI_TS_BUFFERING: s_stat = "buffering"; break;
      case VSAPI_TS_ENDED: s_stat = "finished"; break;
      default:;
      }

    mvwaddnstr (status_window, 1, 1, "          ", 10);
    mvwaddnstr (status_window, 1, 1, s_stat, 10);

    if ((ts == VSAPI_TS_PLAYING) || (ts == VSAPI_TS_PAUSED))
      {
      int p_h, p_m, p_s, d_h, d_m, d_s;
      ms_to_hms (libvlc_server_stat_get_position (stat), &p_h, &p_m, &p_s);
      ms_to_hms (libvlc_server_stat_get_duration (stat), &d_h, &d_m, &d_s);
      char s[30];
      sprintf (s, "%3d", libvlc_server_stat_get_index (stat));
      mvwaddnstr (status_window, 1, 12, s, 4);
      sprintf (s, "%02d:%02d:%02d %02d:%02d:%02d",
        p_h, p_m, p_s, d_h, d_m, d_s); 
      mvwaddnstr (status_window, 1, 16, s, 17);
      }

    const char *mrl = libvlc_server_stat_get_mrl (stat);
    const char *mrl2 = mrl;
    if (strstr (mrl, "file:") == mrl)
      {
      char *p = strrchr (mrl, '/');
      if (p) mrl2 = p + 1; 
      }

    char *fitmrl = view_misc_fit_string (mrl2, COLS - 2); 
    mvwaddnstr (status_window, 2, 1, fitmrl, COLS - 2);
    free (fitmrl);

    libvlc_server_stat_destroy (stat);
    }
  else
    {
    if (error)
      {
      message_show (error);
      free (error);
      }
    }

  wrefresh (status_window);
  }

