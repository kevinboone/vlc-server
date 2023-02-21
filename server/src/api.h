/*======================================================================
  
  vlc-rest-server

  api.h

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/

#pragma once

#include <vlc-server/vs_defs.h>
#include <vlc-server/vs_list.h>
#include <vlc-server/vs_props.h>
#include <vlc-server/libvlc_server_stat.h>
#include <vlc-server/media_database_constraints.h>
#include "media_database.h"

struct _Player;

extern char *api_play_js (struct _Player *player, const char *path);

extern VSApiError api_add_album (struct _Player *player, MediaDatabase *mdb, 
            const char *album, int *added);
extern char *api_add_album_js (struct _Player *player, MediaDatabase *mdb, 
            const char *album);

extern char *api_play_album_js (struct _Player *player, MediaDatabase *mdb, 
            const char *album);

extern char *api_add_js (struct _Player *player, const char *path);

/** Start playback from the start of the playlist, or from the
    current position if paused. Returns an error status if there
    is nothing in the playlist to play. */
extern char *api_start_js (struct _Player *player);

/** Stop playback. */ 
extern char *api_stop_js (struct _Player *player);
extern char *api_pause_js (struct _Player *player);

extern VSList *api_playlist (const struct _Player *player);
extern char *api_playlist_js (struct _Player *player);

extern char *api_clear_js (struct _Player *player);
extern char *api_next_js (struct _Player *player);
extern char *api_prev_js (struct _Player *player);
extern char *api_set_index_js (struct _Player *player, int index);
extern char *api_set_volume_js (struct _Player *player, int volume);

extern char *api_volume_up_js (struct _Player *player);
extern char *api_volume_down_js (struct _Player *player);

extern VSList *api_list_files (const struct _Player *player, 
         const char *media_root, const char *path, VSApiError *e);
extern char *api_list_files_js (struct _Player *player, 
         const char *media_root, const char *path);

extern VSList *api_list_dirs (const struct _Player *player, 
         const char *media_root, const char *path, VSApiError *e);
extern char *api_list_dirs_js (struct _Player *player, 
         const char *media_root, const char *path);

extern VSList *api_list_albums (MediaDatabase *mdb,
         const MediaDatabaseConstraints *mdc, VSApiError *e);
extern char *api_list_albums_js (MediaDatabase *mdb, 
         const VSProps *arguments);

extern VSList *api_list_artists (MediaDatabase *mdb,
         const MediaDatabaseConstraints *mdc, VSApiError *e);
extern char *api_list_artists_js (MediaDatabase *mdb, 
         const VSProps *arguments);

extern VSList *api_list_genres (MediaDatabase *mdb,
         const MediaDatabaseConstraints *mdc, VSApiError *e);
extern char *api_list_genres_js (MediaDatabase *mdb, 
         const VSProps *arguments);

extern VSList *api_list_composers (MediaDatabase *mdb,
         const MediaDatabaseConstraints *mdc, VSApiError *e);
extern char *api_list_composers_js (MediaDatabase *mdb, 
         const VSProps *arguments);

extern VSList *api_list_tracks (MediaDatabase *mdb,
         const MediaDatabaseConstraints *mdc, VSApiError *e);
extern char *api_list_tracks_js (MediaDatabase *mdb, 
         const VSProps *arguments);

extern LibVlcServerStat *api_stat (const struct _Player *player, VSApiError *e);
extern char *api_stat_js (const struct _Player *player);

extern char *api_get_dir_for_album (MediaDatabase *mdb, const char *album);

extern VSApiError api_scan (MediaDatabase *mdb, const char *media_root);
extern char *api_scan_js (MediaDatabase *mdb, const char *media_root);

