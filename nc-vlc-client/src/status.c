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
#include <ncursesw/curses.h>
#include "app_context.h" 
#include "message.h" 
#include "status.h" 
#include "util.h" 
#include "colour.h" 

extern WINDOW *status_window; 

/*======================================================================
  
  status_update  

======================================================================*/
void status_update (LibVlcServerClient *lvsc, const AppContext *context)
  {
  werase (status_window);
  if (context->colour)
    wattron (status_window, COLOR_PAIR (CPAIR_BOX));
  box (status_window, 0, 0);
  if (context->colour)
    wattroff (status_window, COLOR_PAIR (CPAIR_BOX));
  char *error = NULL;
  VSApiError err_code;
  VSServerStat *stat = libvlc_server_client_stat (lvsc, &err_code,
    &error);

  if (stat)
    {
    VSApiTransportStatus ts = 
      vs_server_stat_get_transport_status (stat);
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

    int scanner_progress = vs_server_stat_get_scanner_progress (stat);
    if ((ts == VSAPI_TS_PLAYING) || (ts == VSAPI_TS_PAUSED))
      {
      int p_h, p_m, p_s, d_h, d_m, d_s;
      ms_to_hms (vs_server_stat_get_position (stat), &p_h, &p_m, &p_s);
      ms_to_hms (vs_server_stat_get_duration (stat), &d_h, &d_m, &d_s);
      char s[30];
      sprintf (s, "%3d", vs_server_stat_get_index (stat) + 1);
      mvwaddnstr (status_window, 1, 12, s, 4);
      sprintf (s, "%02d:%02d:%02d %02d:%02d:%02d",
        p_h, p_m, p_s, d_h, d_m, d_s); 
      mvwaddnstr (status_window, 1, 16, s, 17);
      const VSMetadata *amd = vs_server_stat_get_metadata (stat); 
      
      const char *title = vs_metadata_get_title (amd);
      if (!title || !title[0])
        title = vs_server_stat_get_mrl (stat);

      char *fittitle = fit_string (title, COLS - 2); 
      mvwaddnstr (status_window, 2, 1, fittitle, COLS - 2);
      free (fittitle);

      if (scanner_progress < 0)
        {
        // We will later write the scanner progress on the album
        //   line, if the scanner is running
	const char *album = vs_metadata_get_album (amd);
	if (album)
	  {
	  char *fitalbum = fit_string (album, COLS - 2); 
	  mvwaddnstr (status_window, 3, 1, fitalbum, COLS - 2);
	  free (fitalbum);
	  }
        }
      }
    else
      { 
      }

    if (scanner_progress >= 0)
      {
      char s[30];
      sprintf (s, "%d files scanned", scanner_progress);
      mvwaddnstr (status_window, 3, 1, s, COLS - 2);
      }

    vs_server_stat_destroy (stat);
    }
  else
    {
    if (error)
      {
      message_show (error, context);
      free (error);
      }
    }

  wrefresh (status_window);
  }

