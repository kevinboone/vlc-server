/*======================================================================
  
  vlc-rest-server

  player.h

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/

#pragma once

#include "api.h"

struct _Player;

typedef struct _Player Player;

/** patterns is a comma-separated list of file patterns that may be
    played, e.g., "*.flac,*.mp3". If NULL, then all files will be
    passed to VLC. There is no guarantee, of course, that VLC will
    be able to play them. */
extern Player   *player_new (const char *patterns, const char *media_root, 
                   int argc, const char * const *argv, VSApiError *e);
extern void      player_destroy (Player *self);
extern VSApiError  player_add (Player *self, const char *path, int *added);
extern VSApiError  player_play (Player *self, const char *path, int *added);
extern VSApiError  player_start (Player *self);
extern VSApiError  player_pause (Player *self);
extern VSApiError  player_next (Player *self);
extern VSApiError  player_prev (Player *self);
extern VSApiError  player_stop (Player *self);
extern int       player_get_playlist_length (const Player *self);
extern char     *player_get_playlist_item (const Player *self, int index);
extern VSApiTransportStatus 
                 player_get_transport_status (const Player *self);
/** Return the URL of the currently-playing item, or NULL if nothing
    if playing. */
extern char     *player_get_mrl (const Player *self);

/** Get playback position in msec. */
extern int       player_get_pos (const Player *self);

/** Get playist position. This could be -1 if there is no playlist. It
    could also be -1 if items have been added to the playlist but
    nothing actually played yet.  */
extern int       player_get_index (const Player *self);

/** Get duration of current media item in msec. */
extern int       player_get_duration (const Player *self);

extern void      player_clear (Player *self);

extern const char *player_get_media_root (const Player *self);

/** Range is 0-100. */
extern int player_get_volume (const Player *self);
extern void player_set_volume (const Player *self, int volume);

extern VSApiError player_set_index (Player *self, int index);

extern BOOL player_check_allowed_file (const Player *self, const char *path);

extern void player_list_devices (const Player *self);

