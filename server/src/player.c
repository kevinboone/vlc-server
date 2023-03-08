/*======================================================================
  
  vlc-server

  player.c

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/

#define _GNU_SOURCE
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fnmatch.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <vlc/vlc.h>
#include <dirent.h>
#include <vlc-server/vs_log.h> 
#include "player.h"

#define SAFE(x) (x ? x : "")

/*======================================================================

  Player 

======================================================================*/
struct _Player
  {
  libvlc_media_list_t *vlc_media_list;
  libvlc_media_list_player_t *mlp;
  libvlc_instance_t * inst;
  int index; // playlist position
  char *patterns; // Comma-separated list of allowed file patterns
  char *media_root;
  VSMetadata *amd;
  };

/*======================================================================

  player_url_decode

======================================================================*/
static char player_from_hex(char ch) 
  {
  return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
  }

static char *player_url_decode (const char *str) 
  {
  const char *pstr = str; 
  char *buf = malloc(strlen(str) + 1), *pbuf = buf;
  while (*pstr) 
    {
    if (*pstr == '%') 
      {
      if (pstr[1] && pstr[2]) 
        {
        *pbuf++ = player_from_hex(pstr[1]) << 4 | player_from_hex(pstr[2]);
        pstr += 2;
        }
      } 
    else if (*pstr == '+') 
      { 
      *pbuf++ = ' ';
      } 
     else 
      {
      *pbuf++ = *pstr;
      }
      pstr++;
    }
  *pbuf = '\0';
  return buf;
  }

/*======================================================================

  player_vlc_cleanup

======================================================================*/
static void player_vlc_cleanup (Player *self)
  {
  IN
  if (self->vlc_media_list)
    {
    libvlc_media_list_release (self->vlc_media_list);
    self->vlc_media_list = NULL;
    }

  if (self->mlp)
    {
    libvlc_media_list_player_release (self->mlp);
    }

  if (self->inst)
    {
    libvlc_release (self->inst);
    }
  OUT
  }

/*======================================================================

 player_vlc_media_change_handler  

======================================================================*/
static void player_update_metadata (Player *self)
  {
  if (self->amd)
    {
    vs_metadata_destroy (self->amd);
    self->amd = NULL;
    }
  libvlc_media_player_t *p 
     = libvlc_media_list_player_get_media_player (self->mlp);
  libvlc_media_t *m = libvlc_media_player_get_media (p);

  // Ugly, ugly, ugly. Repeat parse_with_options until there is some kind
  //   of response, with 200 msec between attempts. Aysnchronous notification
  //   of completed parse doesn't seem to work at all.
  int status, i = 3;
  do
    {
    libvlc_media_parse_with_options (m, libvlc_media_parse_network, 1000);
    status = libvlc_media_get_parsed_status(m);
    usleep (200000);
    i++;
    } while (status != libvlc_media_parsed_status_done && i < 4); 

  self->amd = vs_metadata_create();
  const char *path = libvlc_media_get_meta (m, libvlc_meta_URL); 
  const char *title = libvlc_media_get_meta (m, libvlc_meta_Title); 
  const char *artist = libvlc_media_get_meta(m, libvlc_meta_Artist);
  const char *album_artist = libvlc_media_get_meta(m, libvlc_meta_AlbumArtist);
  const char *album = libvlc_media_get_meta(m, libvlc_meta_Album);
  const char *track = libvlc_media_get_meta(m, libvlc_meta_TrackID);
  const char *genre = libvlc_media_get_meta(m, libvlc_meta_Genre);
  const char *comment = libvlc_media_get_meta(m, libvlc_meta_Description);
  const char *year = libvlc_media_get_meta(m, libvlc_meta_Date);

  vs_metadata_set_path (self->amd, SAFE (path));
  vs_metadata_set_title (self->amd, SAFE (title));
  vs_metadata_set_album (self->amd, SAFE (album));
  vs_metadata_set_artist (self->amd, SAFE (artist));
  // VLC does not have a specific 'composer' meta item :/
  vs_metadata_set_composer (self->amd, SAFE (artist));
  vs_metadata_set_album_artist (self->amd, SAFE (album_artist));
  vs_metadata_set_genre (self->amd, SAFE (genre));
  vs_metadata_set_track (self->amd, SAFE (track));
  vs_metadata_set_comment (self->amd, SAFE (comment));
  vs_metadata_set_comment (self->amd, SAFE (year));
  //vs_metadata_dump (self->amd);
  }

/*======================================================================

 player_vlc_media_change_handler  

======================================================================*/
static void player_vlc_media_change_handler (const libvlc_event_t *p_event, 
    void *p_data)
  {
  IN
  libvlc_media_t *m 
     = p_event->u.media_list_player_next_item_set.item;
  Player *self = (Player *)p_data;
  if (self->vlc_media_list)
    {
    int index = libvlc_media_list_index_of_item (self->vlc_media_list, m);
    vs_log_info ("Moving to item %d in playlist", index);
    self->index = index;
    player_update_metadata (self);
    }
  OUT
  }

/*======================================================================

  Player 

======================================================================*/
static void player_vlc_log (void *data, int level, 
     const libvlc_log_t *ctx, const char *fmt, va_list args)
  {
  (void)data; (void)ctx;
  int my_level = VSLOG_DEBUG;
  switch (level)
    {
    case LIBVLC_DEBUG: my_level = VSLOG_DEBUG; break;
    case LIBVLC_NOTICE: my_level = VSLOG_INFO; break;
    case LIBVLC_WARNING: my_level = VSLOG_WARNING; break;
    case LIBVLC_ERROR: my_level = VSLOG_ERROR;  break;
    }
  vs_log_vprintf (my_level, fmt, args);
  }

/*======================================================================

  Player 

======================================================================*/
static VSApiError player_vlc_init (Player *self, 
             int argc, const char * const *argv)
  {
  IN
  self->inst = libvlc_new (argc, argv);
  VSApiError ret = 0;
  if (self->inst)
    {
    self->mlp = libvlc_media_list_player_new (self->inst);
    self->index = -1;
    libvlc_event_manager_t *em = 
	 libvlc_media_list_player_event_manager(self->mlp);
    libvlc_event_attach (em, libvlc_MediaListPlayerNextItemSet, 
	 player_vlc_media_change_handler, self);
    libvlc_log_set (self->inst, player_vlc_log, self);

    libvlc_media_player_t* p = libvlc_media_list_player_get_media_player 
	(self->mlp); 
/*
    if (device_name)
      {
      libvlc_audio_output_device_set (p, NULL, device_name);
      if (FALSE)
	{
	vs_log_warning ("Can't set output device %s: using defaults", 
	  device_name);
	}
      }
*/

    char *d = libvlc_audio_output_device_get (p); 	
    vs_log_info ("Audio output device is %s", d); 
    }
  else
    ret = VSAPI_ERR_INIT_VLC;

  OUT
  return ret;
  }

/*======================================================================

  player_new  

======================================================================*/
Player *player_new (const char *patterns, const char *media_root, 
    int argc, const char * const *argv,
    VSApiError *e)
  {
  IN
  *e = 0;
  Player *self = malloc (sizeof (Player));
  memset (self, 0, sizeof (Player));
  if (media_root == NULL || media_root[0] == 0)
    self->media_root = strdup (".");
  else
    self->media_root = strdup (media_root);
  self->patterns = NULL;
  if (patterns)
    self->patterns = strdup (patterns);
  *e = player_vlc_init (self, argc, argv);
  OUT
  return self;
  }

/*======================================================================

  player_destroy

======================================================================*/
void player_destroy (Player *self)
  {
  IN
  player_stop (self);
  player_vlc_cleanup (self);
  if (self->patterns) free (self->patterns);
  if (self->media_root) free (self->media_root);
  if (self->amd) vs_metadata_destroy (self->amd);
  free (self);
  OUT
  }

/*======================================================================

  player_start

======================================================================*/
VSApiError player_start (Player *self)
  {
  IN
 
  if (self->vlc_media_list)
    {
    vs_log_info ("Starting playback");
    // Note that we don't have to handle "unpause" specifically --
    //   _set_media_list does not reset anything if the player is currently
    //   paused, so _play just resumes
    libvlc_media_list_player_set_media_list (self->mlp, self->vlc_media_list);
    libvlc_media_list_player_play (self->mlp);

    OUT
    return 0;
    }
  else
    {
    OUT
    return VSAPI_ERR_NO_PLAYLIST;
    }
  }

/*======================================================================

  player_pause

======================================================================*/
VSApiError player_pause (Player *self)
  {
  IN
  libvlc_media_list_player_set_pause (self->mlp, 1);
  OUT
  return 0;
  }

/*======================================================================

  player_toggle_pause

======================================================================*/
VSApiError player_toggle_pause (Player *self)
  {
  IN
  VSApiError ret = 0;
  VSApiTransportStatus ts = player_get_transport_status (self);
  if (ts == VSAPI_TS_PLAYING)
    player_pause (self);
  else
    ret = player_start (self);
  OUT
  return ret;
  }

/*======================================================================

  player_stop

======================================================================*/
VSApiError player_stop (Player *self)
  {
  IN
  // Careful... this function will be called even if VLC does not 
  //   initialize, because it's part of destroying the Player instance.
  // So we need to check that self->mlp actually exists.
  if (self->mlp)
    {
    vs_log_info ("Stopping playback");
    libvlc_media_list_player_stop (self->mlp);
    if (player_get_playlist_length (self) > 0)
      self->index = 0;
    else
      self->index = -1;
    }
  OUT
  return 0;
  }

/*======================================================================

  player_next

======================================================================*/
VSApiError player_next (Player *self)
  {
  IN
  libvlc_media_list_player_next (self->mlp);
  OUT
  return 0;
  }

/*======================================================================

  player_prev

======================================================================*/
VSApiError player_prev (Player *self)
  {
  IN
  libvlc_media_list_player_previous (self->mlp);
  OUT
  return 0;
  }

/*======================================================================

  player_create_media_list_if_necessary

======================================================================*/
void player_create_media_list_if_necessary (Player *self)
  {
  IN
  if (self->vlc_media_list == NULL)
    {
    vs_log_debug ("media_list empty -- creating new");
    self->vlc_media_list = libvlc_media_list_new (self->inst);
    }
  OUT
  }

/*======================================================================

  player_check_allowed_file

======================================================================*/
BOOL player_check_allowed_file (const Player *self, const char *path)
  {
  IN
  BOOL ret = FALSE;
  const char *patterns = self->patterns;
  if (patterns)
    {
    vs_log_debug ("Check file %s is allowed", path);
    char *saveptr = NULL;
    char *_patterns = strdup (patterns);
    char *pattern = strtok_r (_patterns, ",", &saveptr);
    do
      {
      if (fnmatch (pattern, path, 0 | FNM_CASEFOLD) == 0) ret = TRUE;
      pattern = strtok_r (NULL, ",", &saveptr);
      } while (pattern && ret == FALSE);
    free (_patterns);
    }
  else
    {
    ret = TRUE;
    }
  OUT
  return ret;
  }

/*======================================================================

  player_add_dir

======================================================================*/
VSApiError player_add_dir (Player *self, const char *path, int *added)
  {
  IN
  int ret = 0;
  vs_log_debug ("Expanding directory %s", path);
  DIR *d = opendir (path);
  if (d)
    {
    struct dirent *de = readdir (d);
    do 
      {
      char *fullpath;
      asprintf (&fullpath, "%s/%s", path, de->d_name);
      char *canon = canonicalize_file_name (fullpath);
      struct stat sb;
      stat (canon, &sb);
      if ((sb.st_mode & S_IFMT) == S_IFREG)
        {
        /* VSApiError e = */ player_add (self, canon, added);
        //if (e == 0)
        //  added++;
        }
      free (canon);
      free (fullpath);
      de = readdir (d);
      } while (de);
    closedir (d);
    }
  else
    ret = VSAPI_ERR_OPEN_DIR;
  OUT
  return ret;
  }

/*======================================================================

  player_add

======================================================================*/
VSApiError player_add (Player *self, const char *path, int *added)
  {
  IN
  VSApiError ret = 0;
  char *fullpath = strdup (path); 
  BOOL ok_to_add = TRUE;
  BOOL is_url;

  if (strchr (path, ':'))
    {
    // This is a URL. Don't check it.
    is_url = TRUE;
    }
  else
    {
    is_url = FALSE;
    // Treat the path as a local file. Check that it exists
    if (path[0] == '@' && strstr (path, "..") == NULL)
      {
      // This is a path relative to the media root
      free (fullpath);
      if (self->media_root [strlen (self->media_root) - 1] == '/')
        {
        if (path[1] == '/') path++;
        asprintf (&fullpath, "%s%s", self->media_root, path + 1);
        }
      else
        {
        if (path[1] == '/') path++;
        asprintf (&fullpath, "%s/%s", self->media_root, path + 1);
        }
      }

    if (access (fullpath, R_OK) != 0)
      {
      vs_log_warning ("local file %s not found", fullpath);
      ok_to_add = FALSE;
      ret = VSAPI_ERR_NOT_FOUND;
      } 
    if (ok_to_add)
      {
      struct stat sb;
      stat (fullpath, &sb);
      if ((sb.st_mode & S_IFMT) == S_IFDIR)
        {
        ret = player_add_dir (self, fullpath, added);
        ok_to_add = FALSE; // We already added it
        }
      else if (!player_check_allowed_file (self, fullpath))
        {
        vs_log_warning ("local file %s not allowed", fullpath);
        ret = VSAPI_ERR_FILE_NOT_ALLOWED;
        ok_to_add = FALSE;
        }
      }
    }

  if (ok_to_add)
    {
    (*added)++;
    libvlc_media_t *m;
    if (is_url)
      m = libvlc_media_new_location (self->inst, fullpath); 
    else
      m = libvlc_media_new_path (self->inst, fullpath); 
    vs_log_info ("Adding: %s", fullpath); 
    player_create_media_list_if_necessary (self); 

    libvlc_media_list_lock (self->vlc_media_list);
    libvlc_media_list_add_media (self->vlc_media_list, m); 
    libvlc_media_list_unlock (self->vlc_media_list);
    libvlc_media_release (m);
    }
  free (fullpath);
  OUT
  return ret;
  }

/*======================================================================

  player_play

======================================================================*/
VSApiError player_play (Player *self, const char *path, int *added)
  {
  IN
  VSApiError ret = 0;
  player_clear (self);
  player_stop (self);
  ret = player_add (self, path, added);
  if (*added > 0 && ret == 0)
    ret = player_start (self); 
  OUT
  return ret;
  }



/*======================================================================

  player_get_playlist_length

======================================================================*/
int player_get_playlist_length (const Player *self)
  {
  IN
  int ret = 0;
  if (self)
    {
    if (self->vlc_media_list != NULL)
      {
      ret = libvlc_media_list_count (self->vlc_media_list);
      } 
    }
  OUT
  return ret;
  }


/*======================================================================

  player_get_playlist_item

======================================================================*/
char *player_get_playlist_item (const Player *self, int index)
  {
  IN
  char *ret;

  if (self->vlc_media_list)
    {
    if (index >= 0 && index < libvlc_media_list_count (self->vlc_media_list))
      {
      libvlc_media_list_lock (self->vlc_media_list);
      libvlc_media_t *m = libvlc_media_list_item_at_index 
         (self->vlc_media_list, index);
      char *encmrl = libvlc_media_get_mrl (m);
      libvlc_media_list_unlock (self->vlc_media_list);
      char *mrl = player_url_decode (encmrl);
      ret = mrl;
      libvlc_media_release (m); 
      free (encmrl);
      }
    else
      {
      vs_log_warning ("Playlist index %d out of range", index);
      ret = NULL;
      }
    }
  else
    {
    vs_log_warning ("Attempt to get an item in an empty playlist");
    ret = NULL;
    }
  OUT
  return ret;
  }


/*======================================================================

  player_get_transport_status

======================================================================*/
VSApiTransportStatus player_get_transport_status (const Player *self)
  {
  IN
  VSApiTransportStatus ret = VSAPI_TS_STOPPED;
  if (player_get_playlist_length (self) == 0)
    {
    ret = VSAPI_TS_STOPPED;
    }
  else
    {
    libvlc_state_t s = libvlc_media_list_player_get_state (self->mlp);
    switch (s)
      {
      case libvlc_NothingSpecial: ret = VSAPI_TS_STOPPED; break;
      case libvlc_Opening: ret = VSAPI_TS_OPENING; break;
      case libvlc_Buffering: ret = VSAPI_TS_BUFFERING; break;
      case libvlc_Playing: ret = VSAPI_TS_PLAYING; break;
      case libvlc_Paused: ret = VSAPI_TS_PAUSED; break;
      case libvlc_Stopped: ret = VSAPI_TS_STOPPED; break;
      case libvlc_Error: ret = VSAPI_TS_ERROR; break;
      case libvlc_Ended: ret = VSAPI_TS_ENDED; break;
      }
    }
  return ret;
  OUT
  }

/*======================================================================

  player_get_mrl

======================================================================*/
char *player_get_mrl (const Player *self)
  {
  IN
  libvlc_media_player_t *mp = 
     libvlc_media_list_player_get_media_player (self->mlp);
  libvlc_media_t *m = libvlc_media_player_get_media (mp);
  // Ugh! The media_list_player is populated with media_t items. However,
  //   if nothing is playing, the value of get_media() is NULL. 
  char *mrl;
  if (m)
    {
    char *encmrl = libvlc_media_get_mrl (m);
    mrl = player_url_decode (encmrl);
    libvlc_media_release (m); 
    free (encmrl);
    }
  else
    mrl = NULL;
  OUT
  return mrl;
  }

/*======================================================================

  player_get_pos

======================================================================*/
int player_get_pos (const Player *self)
  {
  IN
  int duration = player_get_duration (self);
  libvlc_media_player_t *mp = 
     libvlc_media_list_player_get_media_player (self->mlp);

  double pos = libvlc_media_player_get_position (mp);
  // Weirdly, _get_position() returns 0.0 when no media is assigned
  //   to the player, but -1.0 if media is assigned, but playback
  //   is stopped. That makes little sense.
  if (pos < 0.0) pos = 0.0;

  OUT
  return pos * duration;
  }

/*======================================================================

  player_get_index

======================================================================*/
int player_get_index (const Player *self)
  {
  return self->index;
  }

/*======================================================================

  player_get_duration

======================================================================*/
int player_get_duration (const Player *self)
  {
  libvlc_media_player_t *mp = 
     libvlc_media_list_player_get_media_player (self->mlp);
  libvlc_media_t *m = libvlc_media_player_get_media (mp);
  int duration; 
  if (m)
    duration = libvlc_media_get_duration (m);
  else
    duration = 0;
  OUT
  return duration;
  }

/*======================================================================

  player_clear

======================================================================*/
extern void player_clear (Player *self)
  {
  if (self->vlc_media_list)
    {
    libvlc_media_list_release (self->vlc_media_list);
    self->vlc_media_list = NULL;
    }
  self->index = -1;
  }

/*======================================================================

  player_get_volume

======================================================================*/
int player_get_volume (const Player *self)
  {
  IN
  int ret = -1;
  if (self->mlp)
    {
    libvlc_media_player_t* p = libvlc_media_list_player_get_media_player 
      (self->mlp); 
    float v = libvlc_audio_get_volume (p);
    ret = (int)v;
    }
  OUT
  return ret;
  }

/*======================================================================

  player_set_volume

======================================================================*/
extern void player_set_volume (const Player *self, int volume)
  {
  libvlc_media_player_t* p = libvlc_media_list_player_get_media_player 
    (self->mlp); 
  libvlc_audio_set_volume (p, volume);
  }

/*======================================================================

  player_set_index

======================================================================*/
VSApiError player_set_index (Player *self, int index)
  {
  IN
  int ret = 0; 
  if (self->vlc_media_list)
    {
    int length = player_get_playlist_length (self);
    if (index < 0 || index >= length)
      ret = VSAPI_ERR_INDEX_RANGE;
    else
      {
      /* There seems to be a horrible bug in libvlc here. 
         Calling play_item_at_index causes a segfault unless something
         is already playing. Calling player_start seems to fix the problem,
         even though it notionally begins playback in the wrong place. 
         I can't be sure that I'm not doing something wrong in my use of
         libvlc, but the segfault seems to occur in even 
         the simplest cases. */
      player_start (self);
      libvlc_media_list_player_play_item_at_index (self->mlp, index);
      }
    }
  else
    {
    ret = VSAPI_ERR_NO_PLAYLIST;
    }
  OUT
  return ret;
  }

/*======================================================================

  player_get_get_media_root

======================================================================*/
const char *player_get_media_root (const Player *self)
  {
  return self->media_root;
  }

/*======================================================================

  player_list_devices 

======================================================================*/
const VSMetadata *player_get_metadata (const Player *self)
  {
  if (player_get_transport_status (self) == VSAPI_TS_STOPPED)
    return NULL;
  else
    return self->amd;
  }

/*======================================================================

  player_list_devices 

======================================================================*/
void player_list_devices (const Player *self)
  {
/*
  libvlc_audio_output_t* olist = libvlc_audio_output_list_get (self->inst);
  if (olist)
    {
    libvlc_audio_output_t *next = olist; 
    do
      {
      const char *device = next->psz_name;
      printf ("name=%s\n", device);
      next = next->p_next;
      } while (next);
      
    libvlc_audio_output_list_release (olist);
    }
*/

  libvlc_media_player_t* p = libvlc_media_list_player_get_media_player 
    (self->mlp); 
  libvlc_audio_output_device_t *dlist = libvlc_audio_output_device_enum (p); 
  if (dlist)
    {
    libvlc_audio_output_device_t *next = dlist; 
    do
      {
      const char *device = next->psz_device;
      printf ("%s\n", device);
      next = next->p_next;
      } while (next);
      
    libvlc_audio_output_device_list_release (dlist); 
    }
  }

