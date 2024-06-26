/*======================================================================
  
  vlc-server

  api.c

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <vlc-server/vs_string.h>
#include <vlc-server/vs_log.h>
#include <vlc-server/vs_util.h>
#include <vlc-server/vs_server_stat.h>
#include <vlc-server/vs_storage.h>
#include <vlc-server/libvlc_media_database.h>
#include "http_server.h"
#include "player.h"
#include "media_database.h"
#include "scanner.h"

#define OK_MSG "{\"status\": 0}\r\n"
#define MAX(a,b) ((a > b) ? a : b)
#define MIN(a,b) ((a > b) ? b : a)
#define SAFE(x) (x ? x : "")

/*======================================================================

 api_escape_json

======================================================================*/
static char *api_escape_json (const char *str)
  {
  IN
  int l = strlen (str);
  int newlen = l;
  char *ret = malloc (newlen + 1);
  int p = 0;

  for (int i = 0; i < l; i++)
    {
    char c = str[i];
    if (c == '\"')
      {
      newlen+=2;
      ret = realloc (ret, newlen + 1);
      ret[p++] = '\\';
      ret[p++] = '\"';
      }
    else
      {
      ret[p++] = c;
      }
    }
  ret[p] = 0;
  return ret;
  OUT
  }

/*======================================================================

 api_play_js

======================================================================*/
char *api_play_js (Player *player, const char *path)
  {
  IN
  char *ret;
  int added = 0;
  int ret2 = player_play (player, path, &added);
  if (ret2)
    {
    char *j_error = api_escape_json (vs_util_strerror (ret2));
    asprintf (&ret, "{\"status\": %d, \"message\": \"%s\"}\r\n", 
      ret2, j_error);
    free (j_error);
    }
  else
    {
    asprintf (&ret, "{\"status\": %d, \"message\": \"Playing %d items\"}\r\n", 
      ret2, added);
    }
  OUT
  return ret;
  }

/*======================================================================

  api_play_random_tracks

======================================================================*/
VSApiError api_play_random_tracks (Player *player, MediaDatabase *mdb)
  {
  VSApiError ret = 0;
  if (media_database_is_init (mdb))
    {
    VSList *tracks = vs_list_create (free);
    char *error = NULL;
    // TODO -- make the number '50' of tracks configurable
    media_database_select_random (mdb, MDB_COL_PATH, 50, tracks, &error);
    if (tracks)
      {
      int len = vs_list_length (tracks);
      if (len > 0)
        {
        player_clear (player);
        player_stop (player);
        int added = 0;
        for (int i = 0; i < len; i++)
          {
          const char *path = vs_list_get (tracks, i);
          int this_added = 0; // Should always end up '1'
          char *rel_path;
          asprintf (&rel_path, "@/%s", path);
          if (player_add (player, rel_path, &this_added) == 0)
            added += this_added;
          free (rel_path);
          }

        if (added > 0)
          ret = player_start (player);
        else
          {
          // And unusual error: there are items in the media database and, after a
          // random selection, none can actually be played
          vs_log_warning ("%s", "Of a random select of tracks, none could be played");
          ret = VSAPI_ERR_GEN_DB;
          }
        }
      else
        {
        ret = VSAPI_ERR_NO_FILES;
        }
      vs_list_destroy (tracks);
      }
    else
      {
      vs_log_warning ("%s", error);
      free (error);
      ret = VSAPI_ERR_GEN_DB;
      }
    }
  else
   ret = VSAPI_ERR_INIT_DB;
  return ret;
  }

/*======================================================================

  api_play_random_album

======================================================================*/
VSApiError api_play_random_album (Player *player, MediaDatabase *mdb)
  {
  VSApiError ret = 0;
  if (media_database_is_init (mdb))
    {
    VSList *albums = vs_list_create (free);
    char *error = NULL;
    media_database_select_random (mdb, MDB_COL_ALBUM, 1, albums, &error);
    if (albums)
      {
      int len = vs_list_length (albums);
      if (len > 0)
        {
        const char *album = vs_list_get (albums, 0);
        player_clear (player);
        player_stop (player);
        int added = 0;
        ret = api_add_album (player, mdb, album, &added);
        if (added > 0)
          ret = player_start (player);
        }
      else
        {
        ret = VSAPI_ERR_NO_ALBUMS;
        }
      vs_list_destroy (albums);
      }
    else
      {
      vs_log_warning ("%s", error);
      free (error);
      ret = VSAPI_ERR_GEN_DB;
      }
    }
  else
   ret = VSAPI_ERR_INIT_DB;
  return ret;
  }


/*======================================================================

 play_random_tracks_js

======================================================================*/
char *api_play_random_tracks_js (Player *player, MediaDatabase *mdb)
  {
  char *ret;
  int ret2 = api_play_random_tracks (player, mdb);
  char *j_error = api_escape_json (vs_util_strerror (ret2));
  asprintf (&ret, "{\"status\": %d, \"message\": \"%s\"}\r\n", 
    ret2, j_error);
  free (j_error);
  return ret;
  }

/*======================================================================

 play_random_album_js

======================================================================*/
char *api_play_random_album_js (Player *player, MediaDatabase *mdb)
  {
  char *ret;
  int ret2 = api_play_random_album (player, mdb);
  char *j_error = api_escape_json (vs_util_strerror (ret2));
  asprintf (&ret, "{\"status\": %d, \"message\": \"%s\"}\r\n", 
    ret2, j_error);
  free (j_error);
  return ret;
  }

/*======================================================================

 api_add_js

======================================================================*/
char *api_add_js (Player *player, const char *path)
  {
  IN
  char *ret;
  int added = 0;
  int ret2 = player_add (player, path, &added);
  if (ret2)
    {
    char *j_error = api_escape_json (vs_util_strerror (ret2));
    asprintf (&ret, "{\"status\": %d, \"message\": \"%s\"}\r\n", 
      ret2, j_error);
    free (j_error);
    }
  else
    {
    asprintf (&ret, "{\"status\": %d, \"message\": \"Added %d items\"}\r\n", 
      ret2, added);
    }
  OUT
  return ret;
  }

/*======================================================================

 api_add_stream

======================================================================*/
VSApiError api_add_stream (Player *player, MediaDatabase *mdb, 
     const char *stream_name)
  {
  VSApiError ret = 0;

  char *escaped_stream = media_database_escape_sql (stream_name);
  char *sql;
  asprintf (&sql, "select uri from streams where name='%s'",
    escaped_stream);

  free (escaped_stream);

  char *error;
  VSList *results = media_database_sql_query (mdb, sql, FALSE, 
     1, &error);
  if (results)
    {
    if (vs_list_length (results) > 0)
      {
      const char *uri = vs_list_get (results, 0);
      int added = 0;
      player_add (player, uri, &added);
      }
    else
      {
      ret = VSAPI_ERR_STREAM_NAME; 
      }
    vs_list_destroy (results);
    }
  else
    {
    // Should never happen, because the value of results will be non-null
    //   even in a catastrophic failure
    vs_log_error (error);
    free (error);
    }

  free (sql);

/*
  VSSearchConstraints *mdc = vs_search_constraints_new ();
  mdc->start = 0;
  mdc->count = -1;
  char *where;


  //asprintf (&where, "stream='%s'", stream);

  asprintf (&where, "name='%s'", escaped_stream);
  free (escaped_stream);

  vs_search_constraints_set_where (mdc, where); 
  free (where);
  VSList *list = api_list_tracks (mdb, 
     mdc, &ret);
  vs_search_constraints_destroy (mdc);

  if (list)
    {
    int l = vs_list_length (list);
    for (int i = 0; i < l; i++)
      {
      const char *path = vs_list_get (list, i);
      char *rel_path;
      if (path[0] == '=')
        asprintf (&rel_path, "%s", path + 1);
      else
        asprintf (&rel_path, "@/%s", path);
      int this_added = 0;
      if (player_add (player, rel_path, &this_added) == 0)
        (*added) += this_added;
      free (rel_path);
      } 
    vs_list_destroy (list);
    }

*/
  OUT
  return ret;
  }

/*======================================================================

 api_add_album

======================================================================*/
VSApiError api_add_album (Player *player, MediaDatabase *mdb, 
     const char *album, int *added)
  {
  VSApiError ret = 0;
  VSSearchConstraints *mdc = vs_search_constraints_new ();
  mdc->start = 0;
  mdc->count = -1;
  char *where;

  char *escaped_album = media_database_escape_sql (album);

  //asprintf (&where, "album='%s'", album);

  asprintf (&where, "album='%s'", escaped_album);
  free (escaped_album);

  vs_search_constraints_set_where (mdc, where); 
  free (where);
  VSList *list = api_list_tracks (mdb, 
     mdc, &ret);
  vs_search_constraints_destroy (mdc);

  if (list)
    {
    int l = vs_list_length (list);
    for (int i = 0; i < l; i++)
      {
      const char *path = vs_list_get (list, i);
      char *rel_path;
      if (path[0] == '=')
        asprintf (&rel_path, "%s", path + 1);
      else
        asprintf (&rel_path, "@/%s", path);
      int this_added = 0;
      if (player_add (player, rel_path, &this_added) == 0)
        (*added) += this_added;
      free (rel_path);
      } 
    vs_list_destroy (list);
    }

  OUT
  return ret;
  }

/*======================================================================

 api_add_album_js

======================================================================*/
char *api_add_album_js (Player *player, MediaDatabase *mdb, 
        const char *album)
  {
  IN
  char *ret;
  int added = 0;
  VSApiError e = api_add_album (player, mdb, album, &added);
  if (e)
    {
    char *j_error = api_escape_json (vs_util_strerror (e));
    asprintf (&ret, "{\"status\": %d, \"message\": \"%s\"}\r\n", 
      e, j_error);
    free (j_error);
    }
  else
    {
    asprintf (&ret, "{\"status\": %d, \"message\": \"Added %d items\"}\r\n", 
      0, added);
    }
  OUT
  return ret;
  }

/*======================================================================

 api_play_album_js

======================================================================*/
char *api_play_album_js (Player *player, MediaDatabase *mdb, 
        const char *album)
  {
  IN
  char *ret;
  int added = 0;
  player_clear (player);
  player_stop (player);
  VSApiError e = api_add_album (player, mdb, album, &added);
  if (e)
    {
    char *j_error = api_escape_json (vs_util_strerror (e));
    asprintf (&ret, "{\"status\": %d, \"message\": \"%s\"}\r\n", 
      e, j_error);
    free (j_error);
    }
  else
    {
    player_start (player);
    asprintf (&ret, "{\"status\": %d, \"message\": \"Added %d items\"}\r\n", 
      0, added);
    }
  OUT
  return ret;
  }

/*======================================================================

 api_play_stream_js

======================================================================*/
char *api_play_stream_js (Player *player, MediaDatabase *mdb, 
        const char *stream_name)
  {
  IN
  char *ret;
  player_clear (player);
  player_stop (player);
  VSApiError e = api_add_stream (player, mdb, stream_name);
  if (e)
    {
    char *j_error = api_escape_json (vs_util_strerror (e));
    asprintf (&ret, "{\"status\": %d, \"message\": \"%s\"}\r\n", 
      e, j_error);
    free (j_error);
    }
  else
    {
    player_start (player);
    asprintf (&ret, 
      "{\"status\": %d, \"message\": \"Added stream to playlist\"}\r\n", 0);
    }
  OUT
  return ret;
  }

/*======================================================================

 api_set_volume_js

======================================================================*/
char *api_set_volume_js (Player *player, int volume)
  {
  IN
  char *ret;
  player_set_volume (player, volume);
  ret = strdup (OK_MSG);
  OUT
  return ret;
  }

/*======================================================================

 api_volume_down_js

======================================================================*/
char *api_volume_down_js (Player *player)
  {
  IN
  char *ret;
  int vol = player_get_volume (player);
  if (vol > 0)
    {
    player_set_volume (player, MAX (vol - 10, 0));
    ret = strdup (OK_MSG);
    }
  else 
    {
    asprintf (&ret, 
      "{\"status\": %d, \"message\": \"Already at mnimum volume\"}\r\n", 
      VSAPI_ERR_VOL_RANGE);
    }
  OUT
  return ret;
  }

/*======================================================================

 api_volume_up_js

======================================================================*/
char *api_volume_up_js (Player *player)
  {
  IN
  char *ret;
  int vol = player_get_volume (player);
  if (vol < 100)
    {
    player_set_volume (player, MIN (vol + 10, 100));
    ret = strdup (OK_MSG);
    }
  else 
    {
    asprintf (&ret, 
      "{\"status\": %d, \"message\": \"Already at maximum volume\"}\r\n", 
      VSAPI_ERR_VOL_RANGE);
    }
  OUT
  return ret;
  }

/*======================================================================

 api_set_index_js

======================================================================*/
char *api_set_index_js (Player *player, int index)
  {
  IN
  char *ret;
  int ret2 = player_set_index (player, index);
  char *j_error = api_escape_json (vs_util_strerror (ret2));
  asprintf (&ret, "{\"status\": %d, \"message\": \"%s\"}\r\n", 
    ret2, j_error);
  free (j_error);
  OUT
  return ret;
  }

/*======================================================================

  api_clear_js

======================================================================*/
char *api_clear_js (Player *player)
  {
  IN
  char *ret;
  player_clear (player);
  ret = strdup (OK_MSG);
  OUT
  return ret; 
  }

/*======================================================================

  api_start_js

======================================================================*/
char *api_start_js (Player *player)
  {
  IN
  char *ret;
  int ret2 = player_start (player);
  char *j_error = api_escape_json (vs_util_strerror (ret2));
  asprintf (&ret, "{\"status\": %d, \"message\": \"%s\"}\r\n", 
    ret2, j_error);
  free (j_error);
  OUT
  return ret;
  }

/*======================================================================

  api_pause_js

======================================================================*/
char *api_pause_js (Player *player)
  {
  char *ret;
  int ret2 = player_pause (player);
  char *j_error = api_escape_json (vs_util_strerror (ret2));
  asprintf (&ret, "{\"status\": %d, \"message\": \"%s\"}\r\n", 
    ret2, j_error);
  free (j_error);
  return ret;
  }

/*======================================================================

  api_toggle_pause_js

======================================================================*/
char *api_toggle_pause_js (Player *player)
  {
  char *ret;
  int ret2 = player_toggle_pause (player);
  char *j_error = api_escape_json (vs_util_strerror (ret2));
  asprintf (&ret, "{\"status\": %d, \"message\": \"%s\"}\r\n", 
    ret2, j_error);
  free (j_error);
  return ret;
  }

/*======================================================================

  api_next_js

======================================================================*/
char *api_next_js (Player *player)
  {
  char *ret;
  int ret2 = player_next (player);
  char *j_error = api_escape_json (vs_util_strerror (ret2));
  asprintf (&ret, "{\"status\": %d, \"message\": \"%s\"}\r\n", 
    ret2, j_error);
  free (j_error);
  return ret;
  }

/*======================================================================

  api_prev_js

======================================================================*/
char *api_prev_js (Player *player)
  {
  char *ret;
  int ret2 = player_prev (player);
  char *j_error = api_escape_json (vs_util_strerror (ret2));
  asprintf (&ret, "{\"status\": %d, \"message\": \"%s\"}\r\n", 
    ret2, j_error);
  free (j_error);
  return ret;
  }

/*======================================================================

  api_stop_js

======================================================================*/
char *api_stop_js (Player *player)
  {
  char *ret;
  int ret2 = player_stop (player);
  char *j_error = api_escape_json (vs_util_strerror (ret2));
  asprintf (&ret, "{\"status\": %d, \"message\": \"%s\"}\r\n", 
    ret2, j_error);
  free (j_error);
  return ret;
  }

/*======================================================================

  api_stat

======================================================================*/
VSServerStat *api_stat (const Player *player, VSApiError *e)
  {
  VSApiTransportStatus ts;
  char *mrl;
  int pos;
  int duration;
  int pl_len = player_get_playlist_length (player);
  int index = player_get_index (player);
  int volume = player_get_volume (player);
  if (pl_len == 0)
    {
    ts = VSAPI_TS_STOPPED;
    mrl = strdup ("");
    pos = 0;
    duration = 0;
    }
  else
    {
    ts = player_get_transport_status (player);
    if (ts == VSAPI_TS_STOPPED)
      mrl = strdup (""); 
    else
      mrl = player_get_mrl (player);
    pos = player_get_pos (player);
    duration = player_get_duration (player);
    }

  int scanner_progress = scanner_get_progress();

  const char *title = "";
  const char *artist = "";
  const char *composer = "";
  const char *album = "";
  const char *genre = "";
  const VSMetadata *amd = player_get_metadata (player);
  if (amd)
    {
    title = SAFE(vs_metadata_get_title (amd));
    artist = SAFE(vs_metadata_get_artist (amd));
    genre = SAFE(vs_metadata_get_genre (amd));
    album = SAFE(vs_metadata_get_album (amd));
    composer = SAFE(vs_metadata_get_composer (amd));
    }
  else
    {
    title = "";
    artist = "";
    genre = "";
    album = "";
    composer = "";
    }

  VSServerStat *stat = vs_server_stat_new (ts, 
    mrl, pos, duration, index, volume,
    title, artist, album, genre, composer, scanner_progress);

  *e = 0; // No errors possible in this function
  free (mrl);
  return stat;
  }

/*======================================================================

  api_storage

======================================================================*/
VSStorage *api_storage (const MediaDatabase *mdb, 
             const struct _Player *player, VSApiError *)
  {
  int capacity_mb = -1;
  int free_mb = -1;
  int albums = -1;
  int tracks = -1;
  const char *media_root = player_get_media_root (player);
  struct statvfs stat;
  if (statvfs(media_root, &stat) == 0)
    {
    capacity_mb = (int) (stat.f_bsize * stat.f_blocks / 1000000L);
    free_mb = (int) (stat.f_bsize * stat.f_bfree/ 1000000L);
    }

  if (media_database_is_init (mdb))
    {
    albums = media_database_search_count (mdb, MDB_COL_ALBUM,
       NULL, NULL);
    tracks = media_database_search_count (mdb, MDB_COL_PATH,
       NULL, NULL);
    }

  VSStorage *ret = vs_storage_new (capacity_mb, free_mb, albums, tracks);
  return ret;
  }

/*======================================================================

  api_storage_js

======================================================================*/
char *api_storage_js (const MediaDatabase *mdb, const struct _Player *player)
  {
  char *ret;
  VSApiError e = 0;
  VSStorage *storage = api_storage (mdb, player, &e);
  if (storage)
    {
    int capacity_mb = vs_storage_get_capacity_mb (storage);
    int free_mb = vs_storage_get_free_mb (storage);
    int albums = vs_storage_get_albums (storage);
    int tracks = vs_storage_get_tracks (storage);
    asprintf (&ret, "{\"status\": %d, \
\"capacity_mb\": %d, \
\"free_mb\": %d, \
\"albums\": %d, \
\"tracks\": %d}\r\n", 
      0, capacity_mb, free_mb, albums, tracks);
    vs_storage_destroy (storage);
    }
  else
    {
    char *j_error = api_escape_json (vs_util_strerror (e));
    asprintf (&ret, "{\"status\": %d, \"message\": \"%s\"}\r\n", 
      e, j_error);
    free (j_error);
    }
  return ret;
  }

/*======================================================================

  api_stat_js

======================================================================*/
char *api_stat_js (const Player *player)
  {
  IN
  char *ret;

  int pl_len = player_get_playlist_length (player);

  VSApiError e;
  VSServerStat *stat = api_stat (player, &e);

  VSApiTransportStatus ts = vs_server_stat_get_transport_status (stat);
  const char *ts_str = vs_util_strts (ts);
  char *j_mrl;
  const char *mrl = vs_server_stat_get_mrl (stat);
  if (mrl)
    j_mrl = api_escape_json (mrl);
  else
    j_mrl = strdup ("");

  int volume = vs_server_stat_get_volume (stat);
  int duration = vs_server_stat_get_duration (stat);
  int pos = vs_server_stat_get_position (stat);
  int index = vs_server_stat_get_index (stat);
  int scanner_progress = vs_server_stat_get_scanner_progress (stat);

  const VSMetadata *amd = player_get_metadata (player);
  char *j_title, *j_artist, *j_album, *j_genre, *j_composer;
  if (amd)
    {
    j_title = strdup (SAFE (vs_metadata_get_title (amd)));
    j_album = strdup (SAFE (vs_metadata_get_album (amd)));
    j_artist = strdup (SAFE (vs_metadata_get_artist (amd)));
    j_genre = strdup (SAFE (vs_metadata_get_genre (amd)));
    j_composer = strdup (SAFE (vs_metadata_get_composer (amd)));
    }
  else
    {
    j_title = strdup ("");
    j_album = strdup ("");
    j_artist = strdup ("");
    j_genre = strdup ("");
    j_composer = strdup ("");
    }
  
  asprintf (&ret, "{\"status\": 0, \
\"playlist_length\": %d, \
\"transport_status\": %d, \
\"transport_status_str\": \"%s\", \
\"position\": %d, \
\"duration\": %d, \
\"index\": %d, \
\"volume\": %d, \
\"title\": \"%s\", \
\"artist\": \"%s\", \
\"album\": \"%s\", \
\"genre\": \"%s\", \
\"composer\": \"%s\", \
\"scanner_progress\": %d, \
\"mrl\": \"%s\"} \
\r\n", pl_len, ts, ts_str, pos, duration, index, volume, 
   j_title, j_artist, j_album, j_genre, j_composer, scanner_progress,
   j_mrl);

  free (j_mrl);
  free (j_title);
  free (j_artist);
  free (j_album);
  free (j_genre);
  free (j_composer);

  vs_server_stat_destroy (stat);
  OUT
  return ret;
  }

/*======================================================================

  api_playlist_

======================================================================*/
VSList *api_playlist (const Player *player)
  {
  IN
  VSList *list = vs_list_create_strings();
  int pl_len = player_get_playlist_length (player);
  for (int i = 0; i < pl_len; i++)
    {
    // Note -- get_playlist_item returns a newly-allocated string.
    // It does not need to be dup'd for the list
    char *mrl = player_get_playlist_item (player, i);
    if (strncmp (mrl, "file://", 7) == 0)
      {
      const char *p = mrl + 7; 
      const char *media_root = player_get_media_root (player);
      if (strncmp (p, media_root, strlen (media_root)) == 0)
        {
        // If the playlist MRL starts with file:///[media_root], then
        //   replace that first part with "@", since this signifies
        //   the media root in add/play operations. We don't want to
        //   give away the media root to clients.
        const char *rel = p + strlen (media_root);
        char *new_mrl = malloc (strlen (rel) + 3);
        strcpy (new_mrl, "@");
        strcat (new_mrl, rel);
        free (mrl);
        mrl = new_mrl;
        }
      } 
    vs_list_append (list, mrl);
    }
  OUT
  return list;
  }

/*======================================================================

  api_playlist_js
  
  // TODO take output of api_playlist

======================================================================*/
char *api_playlist_js (Player *player)
  {
  IN
  VSList *list = api_playlist (player);
  int pl_len = vs_list_length (list); 
  VSString *s = vs_string_create ("{\"status\": 0, \"list\": [");
  for (int i = 0; i < pl_len; i++)
    {
    char *mrl = vs_list_get (list, i);
    char *j_mrl = api_escape_json (mrl);
    vs_string_append (s, "\"");
    vs_string_append (s, j_mrl);
    vs_string_append (s, "\"");
    if (i != pl_len - 1) 
      vs_string_append (s, ",");
    free (j_mrl);
    free (mrl);
    }
  vs_string_append (s, "]}\r\n");
  char *ret = strdup (vs_string_cstr(s));
  vs_string_destroy (s);
  OUT
  return ret;
  }

/*======================================================================

  api_sort_strings

======================================================================*/
static int api_sort_strings (const void *a, const void *b, void *user_data)
  {
  (void)user_data;
  const char **_a = (const char **)a;
  const char **_b = (const char **)b;
  return strcmp (*_a, *_b);
  }

/*======================================================================

  api_list_dir_

======================================================================*/
static VSList *api_list_dir_ (const Player *player, 
      const char *media_root, const char *path, VSApiError *e, BOOL dirs)
  {
  IN
  VSList *list = NULL; 
  char *fullpath = NULL;
  asprintf (&fullpath, "%s/%s", media_root, path); 
  char *canon = canonicalize_file_name (fullpath);
  if (canon)
    {
    DIR *dir = opendir (canon);
    if (dir)
      {
      list = vs_list_create_strings ();
      *e = 0;
      struct dirent *de = readdir (dir); 
      do
        { 
        if (de->d_name[0] != '.')
          {
	  BOOL include = FALSE;
	  char *abspath;
	  asprintf (&abspath, "%s/%s", canon, de->d_name);
	  struct stat sb;
	  stat (abspath, &sb);
	  if ((sb.st_mode & S_IFMT) == S_IFREG)
	    {
	    // It's a file 
	    if (!dirs)
              {
	      if (player_check_allowed_file (player, abspath))
	        include = TRUE;
              }
	    }
	  else
	    {
	    // It's a directory
	    if (dirs)
	      include = TRUE;
	    }
	  if (include)
	    {
	    char *relpath;
            if (path[strlen(path) - 1] == '/')
	      asprintf (&relpath, "%s%s", path, de->d_name);
            else
	      asprintf (&relpath, "%s/%s", path, de->d_name);
	    vs_list_append (list, relpath);
	    }
	  free (abspath);
          }
	de = readdir (dir); 
        } while (de);
      }
    else
      {
      *e = VSAPI_ERR_OPEN_DIR;
      }
    free (canon);
    }
  else
    {
    *e = VSAPI_ERR_NOT_FOUND;
    }

  free (fullpath);
  OUT
  if (list && (*e == 0))
    {
    vs_list_sort (list, api_sort_strings, NULL);
    }
  return list;
  }

/*======================================================================

  api_list_dirs

======================================================================*/
VSList *api_list_dirs (const Player *player, const char *media_root,
      const char *path, VSApiError *e)
  {
  return api_list_dir_ (player, media_root,
      path, e, TRUE);
  }

/*======================================================================

  api_list_files

======================================================================*/
VSList *api_list_files (const Player *player, const char *media_root,
      const char *path, VSApiError *e)
  {
  return api_list_dir_ (player, media_root,
      path, e, FALSE);
  }

/*======================================================================

  api_list_files_js

======================================================================*/
char *api_list_files_js (Player *player, const char *media_root, 
       const char *path)
  {
  IN
  char *ret; 
  VSApiError e;
  VSList *list = api_list_files (player, media_root, path, &e);
  if (list)
    {
    VSString *s = vs_string_create ("{\"status\": 0, \"list\": [");
    int l = vs_list_length (list);
    for (int i = 0; i < l; i++)
      {
      char *j_file = api_escape_json (vs_list_get (list, i));
      vs_string_append (s, "\"");
      vs_string_append (s, j_file);
      vs_string_append (s, "\"");
      free (j_file);
      if (i != l - 1) 
        vs_string_append (s, ",");
      }

    vs_list_destroy (list);
    vs_string_append (s, "]}\r\n");
    ret = strdup (vs_string_cstr (s));
    vs_string_destroy (s);
    }
  else
    {
    char *j_error = api_escape_json (vs_util_strerror (e));
    asprintf (&ret, "{\"status\": %d, \"message\": \"%s\"}\r\n", 
      e, j_error);
    free (j_error);
    }
  OUT
  return ret;
  }

/*======================================================================

  api_list_dirs_js

======================================================================*/
char *api_list_dirs_js (Player *player, const char *media_root, 
       const char *path)
  {
  IN
  char *ret; 
  VSApiError e;
  VSList *list = api_list_dirs (player, media_root, path, &e);
  if (list)
    {
    VSString *s = vs_string_create ("{\"status\": 0, \"list\": [");
    int l = vs_list_length (list);
    for (int i = 0; i < l; i++)
      {
      char *j_file = api_escape_json (vs_list_get (list, i));
      vs_string_append (s, "\"");
      vs_string_append (s, j_file);
      vs_string_append (s, "\"");
      free (j_file);
      if (i != l - 1) 
        vs_string_append (s, ",");
      }

    vs_list_destroy (list);
    vs_string_append (s, "]}\r\n");
    ret = strdup (vs_string_cstr (s));
    vs_string_destroy (s);
    }
  else
    {
    char *j_error = api_escape_json (vs_util_strerror (e));
    asprintf (&ret, "{\"status\": %d, \"message\": \"%s\"}\r\n", 
      e, j_error);
    free (j_error);
    }
  OUT
  return ret;
  }

/*======================================================================

  api_list_albums

======================================================================*/
VSList *api_list_albums (MediaDatabase *mdb, 
     const VSSearchConstraints *mdc, VSApiError *e)
  {
  VSList *ret = NULL;
  if (media_database_is_init (mdb))
    {
    VSList *albums = vs_list_create (free);
    char *error = NULL;
    media_database_search (mdb, MDB_COL_ALBUM, albums, mdc, &error);
    ret = albums;
    }
  else
   *e = VSAPI_ERR_INIT_DB;
  return ret;
  }


/*======================================================================

  api_list_albums_js

======================================================================*/
char *api_list_albums_js (MediaDatabase *mdb, 
        const VSProps *arguments)
  {
  IN
  char *ret; 
  VSApiError e;
  VSSearchConstraints *mdc = vs_search_constraints_new();
  mdc->start = 0;
  mdc->count = -1;
  const char *where = vs_props_get (arguments, "where");
  if (where)
    vs_search_constraints_set_where (mdc, where);
  VSList *list = api_list_albums (mdb, mdc, &e);
  vs_search_constraints_destroy (mdc);
  if (list)
    {
    VSString *s = vs_string_create ("{\"status\": 0, \"list\": [");
    int l = vs_list_length (list);
    for (int i = 0; i < l; i++)
      {
      char *j_file = api_escape_json (vs_list_get (list, i));
      vs_string_append (s, "\"");
      vs_string_append (s, j_file);
      vs_string_append (s, "\"");
      free (j_file);
      if (i != l - 1) 
        vs_string_append (s, ",");
      }

    vs_list_destroy (list);
    vs_string_append (s, "]}\r\n");
    ret = strdup (vs_string_cstr (s));
    vs_string_destroy (s);
    }
  else
    {
    char *j_error = api_escape_json (vs_util_strerror (e));
    asprintf (&ret, "{\"status\": %d, \"message\": \"%s\"}\r\n", 
      e, j_error);
    free (j_error);
    }
  OUT
  return ret;
  }

/*======================================================================

  api_list_streams

======================================================================*/
VSList *api_list_streams (MediaDatabase *mdb, 
     const VSSearchConstraints *mdc, VSApiError *e)
  {
  VSList *ret = NULL;
  if (media_database_is_init (mdb))
    {
    VSList *streams = vs_list_create (free);
    char *error = NULL;
    media_database_search_streams (mdb, MDB_COL_STRMNAME, streams, mdc, &error);
    ret = streams;
    }
  else
   *e = VSAPI_ERR_INIT_DB;
  return ret;
  }


/*======================================================================

  api_list_streams_js

======================================================================*/
char *api_list_streams_js (MediaDatabase *mdb, 
        const VSProps *arguments)
  {
  IN
  char *ret; 
  VSApiError e;
  VSSearchConstraints *mdc = vs_search_constraints_new();
  mdc->start = 0;
  mdc->count = -1;
  const char *where = vs_props_get (arguments, "where");
  if (where)
    vs_search_constraints_set_where (mdc, where);
  VSList *list = api_list_streams (mdb, mdc, &e);
  vs_search_constraints_destroy (mdc);
  if (list)
    {
    VSString *s = vs_string_create ("{\"status\": 0, \"list\": [");
    int l = vs_list_length (list);
    for (int i = 0; i < l; i++)
      {
      char *j_file = api_escape_json (vs_list_get (list, i));
      vs_string_append (s, "\"");
      vs_string_append (s, j_file);
      vs_string_append (s, "\"");
      free (j_file);
      if (i != l - 1) 
        vs_string_append (s, ",");
      }

    vs_list_destroy (list);
    vs_string_append (s, "]}\r\n");
    ret = strdup (vs_string_cstr (s));
    vs_string_destroy (s);
    }
  else
    {
    char *j_error = api_escape_json (vs_util_strerror (e));
    asprintf (&ret, "{\"status\": %d, \"message\": \"%s\"}\r\n", 
      e, j_error);
    free (j_error);
    }
  OUT
  return ret;
  }

/*======================================================================

  api_list_genres

======================================================================*/
VSList *api_list_genres (MediaDatabase *mdb, 
     const VSSearchConstraints *mdc, VSApiError *e)
  {
  VSList *ret = NULL;
  if (media_database_is_init (mdb))
    {
    VSList *genres = vs_list_create (free);
    char *error = NULL;
    media_database_search (mdb, MDB_COL_GENRE, genres, mdc, &error);
    ret = genres;
    }
  else
   *e = VSAPI_ERR_INIT_DB;
  return ret;
  }


/*======================================================================

  api_list_genres_js

======================================================================*/
char *api_list_genres_js (MediaDatabase *mdb, 
        const VSProps *arguments)
  {
  IN
  char *ret; 
  VSApiError e;
  VSSearchConstraints *mdc = vs_search_constraints_new();
  mdc->start = 0;
  mdc->count = -1;
  const char *where = vs_props_get (arguments, "where");
  if (where)
    vs_search_constraints_set_where (mdc, where);
  VSList *list = api_list_genres (mdb, mdc, &e);
  vs_search_constraints_destroy (mdc);
  if (list)
    {
    VSString *s = vs_string_create ("{\"status\": 0, \"list\": [");
    int l = vs_list_length (list);
    for (int i = 0; i < l; i++)
      {
      char *j_file = api_escape_json (vs_list_get (list, i));
      vs_string_append (s, "\"");
      vs_string_append (s, j_file);
      vs_string_append (s, "\"");
      free (j_file);
      if (i != l - 1) 
        vs_string_append (s, ",");
      }

    vs_list_destroy (list);
    vs_string_append (s, "]}\r\n");
    ret = strdup (vs_string_cstr (s));
    vs_string_destroy (s);
    }
  else
    {
    char *j_error = api_escape_json (vs_util_strerror (e));
    asprintf (&ret, "{\"status\": %d, \"message\": \"%s\"}\r\n", 
      e, j_error);
    free (j_error);
    }
  OUT
  return ret;
  }

/*======================================================================

  api_list_composers

======================================================================*/
VSList *api_list_composers (MediaDatabase *mdb, 
     const VSSearchConstraints *mdc, VSApiError *e)
  {
  VSList *ret = NULL;
  if (media_database_is_init (mdb))
    {
    VSList *composers = vs_list_create (free);
    char *error = NULL;
    media_database_search (mdb, MDB_COL_COMPOSER, composers, mdc, &error);
    ret = composers;
    }
  else
   *e = VSAPI_ERR_INIT_DB;
  return ret;
  }


/*======================================================================

  api_list_composers_js

======================================================================*/
char *api_list_composers_js (MediaDatabase *mdb, 
        const VSProps *arguments)
  {
  IN
  char *ret; 
  VSApiError e;
  VSSearchConstraints *mdc = vs_search_constraints_new();
  mdc->start = 0;
  mdc->count = -1;
  const char *where = vs_props_get (arguments, "where");
  if (where)
    vs_search_constraints_set_where (mdc, where);
  VSList *list = api_list_composers (mdb, mdc, &e);
  vs_search_constraints_destroy (mdc);
  if (list)
    {
    VSString *s = vs_string_create ("{\"status\": 0, \"list\": [");
    int l = vs_list_length (list);
    for (int i = 0; i < l; i++)
      {
      char *j_file = api_escape_json (vs_list_get (list, i));
      vs_string_append (s, "\"");
      vs_string_append (s, j_file);
      vs_string_append (s, "\"");
      free (j_file);
      if (i != l - 1) 
        vs_string_append (s, ",");
      }

    vs_list_destroy (list);
    vs_string_append (s, "]}\r\n");
    ret = strdup (vs_string_cstr (s));
    vs_string_destroy (s);
    }
  else
    {
    char *j_error = api_escape_json (vs_util_strerror (e));
    asprintf (&ret, "{\"status\": %d, \"message\": \"%s\"}\r\n", 
      e, j_error);
    free (j_error);
    }
  OUT
  return ret;
  }

/*======================================================================

  api_list_artists

======================================================================*/
VSList *api_list_artists (MediaDatabase *mdb, 
     const VSSearchConstraints *mdc, VSApiError *e)
  {
  VSList *ret = NULL;
  if (media_database_is_init (mdb))
    {
    VSList *artists = vs_list_create (free);
    char *error = NULL;
    media_database_search (mdb, MDB_COL_ARTIST, artists, mdc, &error);
    ret = artists;
    }
  else
   *e = VSAPI_ERR_INIT_DB;
  return ret;
  }


/*======================================================================

  api_list_artists_js

======================================================================*/
char *api_list_artists_js (MediaDatabase *mdb, 
        const VSProps *arguments)
  {
  IN
  char *ret; 
  VSApiError e;
  VSSearchConstraints *mdc = vs_search_constraints_new();
  mdc->start = 0;
  mdc->count = -1;
  const char *where = vs_props_get (arguments, "where");
  if (where)
    vs_search_constraints_set_where (mdc, where);
  VSList *list = api_list_artists (mdb, mdc, &e);
  vs_search_constraints_destroy (mdc);
  if (list)
    {
    VSString *s = vs_string_create ("{\"status\": 0, \"list\": [");
    int l = vs_list_length (list);
    for (int i = 0; i < l; i++)
      {
      char *j_file = api_escape_json (vs_list_get (list, i));
      vs_string_append (s, "\"");
      vs_string_append (s, j_file);
      vs_string_append (s, "\"");
      free (j_file);
      if (i != l - 1) 
        vs_string_append (s, ",");
      }

    vs_list_destroy (list);
    vs_string_append (s, "]}\r\n");
    ret = strdup (vs_string_cstr (s));
    vs_string_destroy (s);
    }
  else
    {
    char *j_error = api_escape_json (vs_util_strerror (e));
    asprintf (&ret, "{\"status\": %d, \"message\": \"%s\"}\r\n", 
      e, j_error);
    free (j_error);
    }
  OUT
  return ret;
  }

/*======================================================================

  api_list_tracks

======================================================================*/
VSList *api_list_tracks (MediaDatabase *mdb, 
     const VSSearchConstraints *mdc, VSApiError *e)
  {
  VSList *ret = NULL;
  if (media_database_is_init (mdb))
    {
    VSList *tracks = vs_list_create (free);
    char *error = NULL;
    media_database_search (mdb, MDB_COL_PATH, tracks, mdc, &error);
    ret = tracks;
    }
  else
   *e = VSAPI_ERR_INIT_DB;
  return ret;
  }


/*======================================================================

  api_list_tracks_js

======================================================================*/
char *api_list_tracks_js (MediaDatabase *mdb, 
        const VSProps *arguments)
  {
  IN
  char *ret; 
  VSApiError e;
  VSSearchConstraints *mdc = vs_search_constraints_new();
  mdc->start = 0;
  mdc->count = -1;
  const char *where = vs_props_get (arguments, "where");
  if (where)
    vs_search_constraints_set_where (mdc, where);
  VSList *list = api_list_tracks (mdb, mdc, &e);
  vs_search_constraints_destroy (mdc);
  if (list)
    {
    VSString *s = vs_string_create ("{\"status\": 0, \"list\": [");
    int l = vs_list_length (list);
    for (int i = 0; i < l; i++)
      {
      char *j_file = api_escape_json (vs_list_get (list, i));
      vs_string_append (s, "\"");
      vs_string_append (s, j_file);
      vs_string_append (s, "\"");
      free (j_file);
      if (i != l - 1) 
        vs_string_append (s, ",");
      }

    vs_list_destroy (list);
    vs_string_append (s, "]}\r\n");
    ret = strdup (vs_string_cstr (s));
    vs_string_destroy (s);
    }
  else
    {
    char *j_error = api_escape_json (vs_util_strerror (e));
    asprintf (&ret, "{\"status\": %d, \"message\": \"%s\"}\r\n", 
      e, j_error);
    free (j_error);
    }
  OUT
  return ret;
  }

/*======================================================================

  api_get_image_for_album

======================================================================*/
char *api_get_dir_for_album (MediaDatabase *mdb, const char *album)
  {
  char *ret = NULL;
  if (media_database_is_init (mdb))
    {
    VSSearchConstraints *mdc = vs_search_constraints_new();
    mdc->start = 0;
    mdc->count = 1;
    VSString *where = vs_string_create ("album='");
    char *enc_album = media_database_escape_sql (album); 
    vs_string_append (where, enc_album); 
    free (enc_album);
    vs_string_append (where, "'");
    vs_search_constraints_set_where (mdc, vs_string_cstr (where));
    VSList *list = vs_list_create (free);
    char *error = NULL;
    //printf ("where=%s\n", vs_string_cstr (where));
    media_database_search (mdb, MDB_COL_PATH, list, mdc, &error);
    if (error)
      {
      //printf ("track=%s\n", file);
      // TODO
      free (error);
      }
    else 
      {
      int l = vs_list_length (list);
      if (l > 0)
        {
        const char *file = vs_list_get (list, 0);
        if (file[0])
          {
          char *spos = strchr (file + 1, '/');
          if (spos)
            {
            ret = strdup (file);
            int l = spos - file;
            ret[l] = 0;
            }
          }
        }
      }
    vs_list_destroy (list);
    vs_search_constraints_destroy (mdc);
    vs_string_destroy (where);
    }
  return ret;
  }

/*======================================================================

  api_scan

======================================================================*/
VSApiError api_scan (MediaDatabase *mdb, const char *media_root)
  {
  const char *filename = media_database_get_filename (mdb);
  char exec[PATH_MAX];
  readlink ("/proc/self/exe", exec, sizeof (exec) - 1);
  char *cmd;
  asprintf (&cmd, "%s -q -d '%s' -r '%s'", exec, filename, media_root); 
  vs_log_info ("Running command '%s'", cmd);
  system (cmd);
  free (cmd);
  return 0;
  }

/*======================================================================

  api_fullscan

======================================================================*/
VSApiError api_fullscan (MediaDatabase *mdb, const char *media_root)
  {
  const char *filename = media_database_get_filename (mdb);
  char exec[PATH_MAX];
  readlink ("/proc/self/exe", exec, sizeof (exec) - 1);
  char *cmd;
  asprintf (&cmd, "%s -s -d '%s' -r '%s'", exec, filename, media_root); 
  vs_log_info ("Running command '%s'", cmd);
  system (cmd);
  free (cmd);
  return 0;
  }

/*======================================================================

  api_scan_js

======================================================================*/
char *api_scan_js (MediaDatabase *mdb, const char *media_root)
  {
  char *ret;
  int ret2 = api_scan (mdb, media_root); 
  char *j_error = api_escape_json (vs_util_strerror (ret2));
  asprintf (&ret, "{\"status\": %d, \"message\": \"%s\"}\r\n", 
    ret2, j_error);
  free (j_error);
  return ret;
  }

/*======================================================================

  api_fullscan_js

======================================================================*/
char *api_fullscan_js (MediaDatabase *mdb, const char *media_root)
  {
  char *ret;
  int ret2 = api_fullscan (mdb, media_root); 
  char *j_error = api_escape_json (vs_util_strerror (ret2));
  asprintf (&ret, "{\"status\": %d, \"message\": \"%s\"}\r\n", 
    ret2, j_error);
  free (j_error);
  return ret;
  }

/*======================================================================

  api_version_js

======================================================================*/
char *api_version_js (void)
  {
  char *ret;
  asprintf (&ret, "{\"status\": 0, \"version\": \"%s\"}\r\n", VERSION);
  return ret;
  }



