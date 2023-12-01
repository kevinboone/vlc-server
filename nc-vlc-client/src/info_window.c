/*======================================================================
  
  nv-vlc-client 

  nc-vlc-client/src/info_window.c

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <vlc-server/api-client.h>
#include <ncursesw/curses.h>
#include "keys.h" 
#include "message.h" 
#include "status.h" 
#include "info_window.h" 
#include "view_misc.h" 
#include "util.h" 


/*======================================================================
  
  info_window_update

======================================================================*/
static void info_window_update (WINDOW *info_window, 
              LibVlcServerClient *lvsc)
  {
  werase (info_window);
  //box (info_window, 0, 0);

  time_t now = time (NULL);
  char *s_time = ctime (&now);
  s_time[16] = 0;
  mvwaddnstr (info_window, 2, 1, s_time, 24);

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
      case VSAPI_TS_STOPPED: s_stat = "Stopped"; break;
      case VSAPI_TS_PLAYING: s_stat = "Playing"; break;
      case VSAPI_TS_PAUSED: s_stat = "Paused"; break;
      case VSAPI_TS_OPENING: s_stat = "Opening"; break;
      case VSAPI_TS_BUFFERING: s_stat = "Buffering"; break;
      case VSAPI_TS_ENDED: s_stat = "Finished playlist"; break;
      default:;
      }

    mvwaddnstr (info_window, 3, 1, s_stat, COLS - 2);

    if ((ts == VSAPI_TS_PLAYING) || (ts == VSAPI_TS_PAUSED))
      {
      int p_h, p_m, p_s, d_h, d_m, d_s;
      ms_to_hms (vs_server_stat_get_position (stat), &p_h, &p_m, &p_s);
      ms_to_hms (vs_server_stat_get_duration (stat), &d_h, &d_m, &d_s);
      char s[30];
      sprintf (s, "Item %d in playlist", 
         vs_server_stat_get_index (stat) + 1);
      mvwaddnstr (info_window, 4, 1, s, 20);
      sprintf (s, "%02d:%02d:%02d %02d:%02d:%02d",
        p_h, p_m, p_s, d_h, d_m, d_s); 
      mvwaddnstr (info_window, 5, 1, s, 17);
      const VSMetadata *amd = vs_server_stat_get_metadata (stat); 
      
      const char *title = vs_metadata_get_title (amd);
      if (!title || !title[0])
        title = vs_server_stat_get_mrl (stat);
      char *fittitle = fit_string (title, COLS - 3); 
      mvwaddnstr (info_window, 6, 1, fittitle, COLS - 3);
      free (fittitle);

      mvwaddnstr (info_window, 7, 1, "Album:", COLS - 3);
      const char *album = vs_metadata_get_album (amd);
      if (album)
        {
        char *fitalbum = fit_string (album, COLS - 3); 
        mvwaddnstr (info_window, 7, 8, fitalbum, COLS - 3 - 8);
        free (fitalbum);
        }

      mvwaddnstr (info_window, 8, 1, "Artist:", COLS - 3);
      const char *artist = vs_metadata_get_artist (amd);
      if (artist)
        {
        char *fitartist = fit_string (artist, COLS - 3); 
        mvwaddnstr (info_window, 8, 9, fitartist, COLS - 3 - 9);
        free (fitartist);
        }

      mvwaddnstr (info_window, 9, 1, "Album artist:", COLS - 3);
      const char *album_artist = vs_metadata_get_album_artist (amd);
      if (album_artist)
        {
        char *fitalbumartist = fit_string (album_artist, COLS - 3); 
        mvwaddnstr (info_window, 9, 15, fitalbumartist, COLS - 3 - 15);
        free (fitalbumartist);
        }

      mvwaddnstr (info_window, 10, 1, "Genre:", COLS - 3);
      const char *genre = vs_metadata_get_genre (amd);
      if (genre)
        {
        char *fitgenre = fit_string (genre, COLS - 3); 
        mvwaddnstr (info_window, 10, 8, fitgenre, COLS - 3 - 8);
        free (fitgenre);
        }

      mvwaddnstr (info_window, 11, 1, "Composer:", COLS - 3);
      const char *composer = vs_metadata_get_composer (amd);
      if (genre)
        {
        char *fitcomposer = fit_string (composer, COLS - 3); 
        mvwaddnstr (info_window, 11, 11, fitcomposer, COLS - 3 - 11);
        free (fitcomposer);
        }
      }

    int scanner_progress = vs_server_stat_get_scanner_progress (stat);
    if (scanner_progress >= 0)
      {
      char s[30];
      sprintf (s, "%d files scanned", scanner_progress);
      mvwaddnstr (info_window, 12, 1, s, COLS - 2);
      }

    vs_server_stat_destroy (stat);
    }
  else
    {
    if (error)
      {
      mvwaddnstr (info_window, 3, 1, error, COLS - 2);
      free (error);
      }
    }

  wrefresh (info_window);
  }

/*======================================================================
  
  info_window_run

======================================================================*/
void info_window_run (WINDOW *main_window, LibVlcServerClient *lvsc,
       const AppContext *context)
  {
  WINDOW *info_window = subwin (main_window, LINES, COLS, 0, 0);
  info_window_update (info_window, lvsc);
  BOOL info_done = FALSE;
  while (!info_done)
    {
    int ch;
    while (!info_done && (ch = getch ()))
      {
      if (view_misc_handle_non_menu_key (lvsc, ch, context))
        {
        // Nothing to do
        info_done = TRUE;
        } 
      else if (ch == ERR)
        {
        info_window_update (info_window, lvsc);
        }
      else
        info_done = TRUE;
      } 
    }
  delwin (info_window);
  }


