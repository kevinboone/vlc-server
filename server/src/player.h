/*======================================================================
  
  vlc-rest-server

  player.h

  Player is the main interface to the VLC media player. If provides
    functions to add URLs and manage the playlist, control the
    transport, etc.

  Copyright (c)2022-3 Kevin Boone, GPL v3.0

======================================================================*/

#pragma once

#include "api.h"

struct _Player;
typedef struct _Player Player;

/** Create a new Player instance. 
    patterns is a comma-separated list of file patterns that may be
    played, e.g., "*.flac,*.mp3". If NULL, then all files will be
    passed to VLC. There is no guarantee, of course, that VLC will
    be able to play them. The media_root is the top of the directory
    hierarchy that contains media files.  */
extern Player   *player_new (const char *patterns, const char *media_root, 
                   int argc, const char * const *argv, VSApiError *e);

/** Clean up, release VLC resources */
extern void      player_destroy (Player *self);

/** Add a URL. If it begins with @, it is a local file relative to the
      media root. Anything with a : is taken to be a URL. Anything else
      is an absolute pathname. Either absolute or relative path can be
      a directory. */ 
extern VSApiError  player_add (Player *self, const char *path, int *added);

/** Like _add(), except that the playlist is first cleared, and then playback
      starts with the first added item. */
extern VSApiError  player_play (Player *self, const char *path, int *added);

/** Resume playback if paused. Otherwise, start playback from the top of
      the playlist. */
extern VSApiError  player_start (Player *self);

extern VSApiError  player_pause (Player *self);

extern VSApiError  player_toggle_pause (Player *self);

/** _next() and _prev() play the next and previous items in the playlist. */
extern VSApiError  player_next (Player *self);
extern VSApiError  player_prev (Player *self);

extern VSApiError  player_stop (Player *self);

/** Get the number of items (not the duration) in the playlist. */
extern int       player_get_playlist_length (const Player *self);

/** Get the specified item from the playlist. This is a raw URL,
     not relative to the media root. */
extern char     *player_get_playlist_item (const Player *self, int index);

/** Get the status of the player. */
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

/** Clear the playlist. */
extern void      player_clear (Player *self);

/** Get the media root, that was set when the Player was constructed. */
extern const char *player_get_media_root (const Player *self);

/** Get or set volume; range is 0-100. */
extern int player_get_volume (const Player *self);
extern void player_set_volume (const Player *self, int volume);

/** Play a particular item in the playlist. */
extern VSApiError player_set_index (Player *self, int index);

/** Returns TRUE if the filname matches one of the patterns supplied
      when the player was created. */
extern BOOL player_check_allowed_file (const Player *self, const char *path);

/** Dump to stdout the audio output devices known to VLC. */
extern void player_list_devices (const Player *self);

/** Return metadata for the currently playing item. This might return
    NULL. If it does not, some of the individual fields in the returned
    AMD might themselves be NULL. Even if they are non-NULL, they might
    be empty -- particularly with network streams. */
extern const VSMetadata *player_get_metadata (const Player *self);


