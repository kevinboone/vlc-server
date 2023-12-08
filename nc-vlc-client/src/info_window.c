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
#include "colour.h" 
#include "util.h" 


/*======================================================================
  
  info_window_update

======================================================================*/
static void info_window_update (WINDOW *info_window, 
              LibVlcServerClient *lvsc, const AppContext *context)
  {
  werase (info_window);
  //box (info_window, 0, 0);

  int row = 1;
  if (context->colour)
    wattron (info_window, COLOR_PAIR (CPAIR_MENU_CAPTION));
  mvwaddnstr (info_window, row, 1, context->title, 24);
  if (context->colour)
    wattroff (info_window, COLOR_PAIR (CPAIR_MENU_CAPTION));
  row++;

  time_t now = time (NULL);
  const struct tm* tm = localtime (&now);
  char s_time[20];
  strftime (s_time, sizeof(s_time) - 1, "%a %b %d %H:%M", tm);

  mvwaddnstr (info_window, row, 1, s_time, 24);
  row++;

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

    mvwaddnstr (info_window, row, 1, s_stat, COLS - 2);
    row++;

    if ((ts == VSAPI_TS_PLAYING) || (ts == VSAPI_TS_PAUSED))
      {
      int p_m, p_s, d_m, d_s;
      ms_to_minsec (vs_server_stat_get_position (stat), &p_m, &p_s);
      ms_to_minsec (vs_server_stat_get_duration (stat), &d_m, &d_s);
      char s[30];
      sprintf (s, "Item %d in playlist", 
         vs_server_stat_get_index (stat) + 1);
      mvwaddnstr (info_window, row, 1, s, 20);
      row++;
      sprintf (s, "%02d:%02d %02d:%02d", p_m, p_s, d_m, d_s); 
      mvwaddnstr (info_window, row, 1, s, 17);
      row++;
      const VSMetadata *amd = vs_server_stat_get_metadata (stat); 
      
      const char *title = vs_metadata_get_title (amd);
      if (!title || !title[0])
        title = vs_server_stat_get_mrl (stat);
      char *fittitle = fit_string (title, COLS - 1); 
      mvwaddnstr (info_window, row, 1, fittitle, COLS - 1);
      row++;
      free (fittitle);

      const char *album = vs_metadata_get_album (amd);
      if (album && album[0])
        {
        char *fitalbum = fit_string (album, COLS - 1); 
        mvwaddnstr (info_window, row, 1, fitalbum, COLS - 1);
        free (fitalbum);
        row++;
        }

      const char *artist = vs_metadata_get_artist (amd);
      if (artist && artist[0])
        {
        char *fitartist = fit_string (artist, COLS - 1); 
        mvwaddnstr (info_window, row, 1, fitartist, COLS - 1);
        free (fitartist);
        row++;
        }

      const char *album_artist = vs_metadata_get_album_artist (amd);
      if (album_artist && album_artist[0])
        {
        char *fitalbumartist = fit_string (album_artist, COLS - 1); 
        mvwaddnstr (info_window, row, 1, fitalbumartist, COLS - 1);
        row++;
        free (fitalbumartist);
        }

      const char *genre = vs_metadata_get_genre (amd);
      if (genre && genre[0])
        {
        char *fitgenre = fit_string (genre, COLS - 1); 
        mvwaddnstr (info_window, row, 1, fitgenre, COLS - 1);
        row++;
        free (fitgenre);
        }

      const char *composer = vs_metadata_get_composer (amd);
      if (composer && composer[0])
        {
        char *fitcomposer = fit_string (composer, COLS - 1); 
        mvwaddnstr (info_window, row, 1, fitcomposer, COLS - 1);
        row++;
        free (fitcomposer);
        }
      }

    int scanner_progress = vs_server_stat_get_scanner_progress (stat);
    if (scanner_progress >= 0)
      {
      char s[30];
      sprintf (s, "%d files scanned", scanner_progress);
      mvwaddnstr (info_window, row, 1, s, COLS - 1);
      row++;
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
  info_window_update (info_window, lvsc, context);
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
        info_window_update (info_window, lvsc, context);
        }
      else
        info_done = TRUE;
      } 
    }
  delwin (info_window);
  }


