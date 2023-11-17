/*======================================================================
  
  vlc-rest-server

  api.h

  This module contains a set of functions that the REST API might
    eventually call. My intention was that each call to the REST
    API would end up with one of these functions. However, I have
    been sloppy, and implemented some REST functionality elsewhere :/

  Functions ending in _js return a response in JSON format, intended
    to be returned directly to a REST client. These functions are mostly
    just wrappers around the corresponding functions with similar names
    not ending in _js. However, I have been sloppy here also.

  Most of these functions simply delegate to Player or MediaDatabase.

  Copyright (c)2022-3 Kevin Boone, GPL v3.0

======================================================================*/

#pragma once

#include <vlc-server/vs_defs.h>
#include <vlc-server/vs_list.h>
#include <vlc-server/vs_props.h>
#include <vlc-server/vs_server_stat.h>
#include <vlc-server/vs_search_constraints.h>
#include "media_database.h"

struct _Player;

/** Add an item to the playlist. */
extern char *api_add_js (struct _Player *player, const char *path);

/** Like add(), but clear the playlist first, then start playing. */
extern char *api_play_js (struct _Player *player, const char *path);

/** Adds all the tracks from an album. This is a convenience function
      for REST clients, including the Javascript implementation of the
      web interface. The alternative would be to enumerate all the tracks
      of the album, and then add them to the playlist individually. */
extern VSApiError api_add_album (struct _Player *player, MediaDatabase *mdb, 
            const char *album, int *added);
extern char *api_add_album_js (struct _Player *player, MediaDatabase *mdb, 
            const char *album);

/** Like add_album(), but clear the playlist first, then start playing. */
extern char *api_play_album_js (struct _Player *player, MediaDatabase *mdb, 
            const char *album);

extern VSApiError api_play_random_tracks (struct _Player *player, MediaDatabase *mdb);
extern char *api_play_random_tracks_js (struct _Player *player, MediaDatabase *mdb);

extern VSApiError api_play_random_album (struct _Player *player, MediaDatabase *mdb);
extern char *api_play_random_album_js (struct _Player *player, MediaDatabase *mdb);

/** Start playback from the start of the playlist, or from the
    current position if paused. Returns an error status if there
    is nothing in the playlist to play. */
extern char *api_start_js (struct _Player *player);

extern char *api_stop_js (struct _Player *player);
extern char *api_pause_js (struct _Player *player);
extern char *api_toggle_pause_js (struct _Player *player);

/** Get a list of everything in the playlist. */
extern VSList *api_playlist (const struct _Player *player);
extern char *api_playlist_js (struct _Player *player);

/** Clear the playlist. */
extern char *api_clear_js (struct _Player *player);

extern char *api_next_js (struct _Player *player);
extern char *api_prev_js (struct _Player *player);
extern char *api_set_index_js (struct _Player *player, int index);
extern char *api_set_volume_js (struct _Player *player, int volume);

/** Convenience functions to raise and lower the volume in a single
     REST call. */
extern char *api_volume_up_js (struct _Player *player);
extern char *api_volume_down_js (struct _Player *player);

/** Get a list of local files (not directories) in the specified path. */
extern VSList *api_list_files (const struct _Player *player, 
         const char *media_root, const char *path, VSApiError *e);
extern char *api_list_files_js (struct _Player *player, 
         const char *media_root, const char *path);

/** Get a list of local directories in the specified path. */
extern VSList *api_list_dirs (const struct _Player *player, 
         const char *media_root, const char *path, VSApiError *e);
extern char *api_list_dirs_js (struct _Player *player, 
         const char *media_root, const char *path);

/** The following functions all retrieve entries from the media database.
    The behaviour of the database search is controlled by the contents
    of the MediaDatabaseConstratints structure. In particular, this structure
    has a 'where' field that applies a WHERE clause to the SQL SELECT
    operation. */ 
extern VSList *api_list_albums (MediaDatabase *mdb,
         const VSSearchConstraints *mdc, VSApiError *e);
extern char *api_list_albums_js (MediaDatabase *mdb, 
         const VSProps *arguments);

extern VSList *api_list_artists (MediaDatabase *mdb,
         const VSSearchConstraints *mdc, VSApiError *e);
extern char *api_list_artists_js (MediaDatabase *mdb, 
         const VSProps *arguments);

extern VSList *api_list_genres (MediaDatabase *mdb,
         const VSSearchConstraints *mdc, VSApiError *e);
extern char *api_list_genres_js (MediaDatabase *mdb, 
         const VSProps *arguments);

extern VSList *api_list_composers (MediaDatabase *mdb,
         const VSSearchConstraints *mdc, VSApiError *e);
extern char *api_list_composers_js (MediaDatabase *mdb, 
         const VSProps *arguments);

extern VSList *api_list_tracks (MediaDatabase *mdb,
         const VSSearchConstraints *mdc, VSApiError *e);
extern char *api_list_tracks_js (MediaDatabase *mdb, 
         const VSProps *arguments);

/* Return the player status, including what is playing and the position. */
extern VSServerStat *api_stat (const struct _Player *player, VSApiError *e);
extern char *api_stat_js (const struct _Player *player);

/* Get a URL that can be requested by a browser, to retrieve the cover
     art for a specific album. The caller will form the complete URL
     by appending the result from this function to the prefix "/cover/". 
   In practice, the method just gets the directory that contains the first
     file tagged with the specific album. We kind-of assume that a particular
     album occupies a particular director; but this is probably the only
     place in vlc-server where this assumption actually matters. */ 
extern char *api_get_dir_for_album (MediaDatabase *mdb, const char *album);

/* Start a (quick) scan of the database. */
extern VSApiError api_scan (MediaDatabase *mdb, const char *media_root);
extern char *api_scan_js (MediaDatabase *mdb, const char *media_root);

/* Get the server version as a JSON object. */
extern char *api_version_js (void);

