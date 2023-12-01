/*======================================================================
  
  vlc-server

  api-client.h

  Copyright (c)2022 Kevin Boone, GPL v3.0

  This file contains definitions of all the functions and data
  structures provided by the vlc-server API library.

======================================================================*/

#include <vlc-server/vs_defs.h>
#include <vlc-server/vs_util.h>
#include <vlc-server/vs_log.h>
#include <vlc-server/vs_list.h>
#include <vlc-server/vs_playlist.h>
#include <vlc-server/vs_server_stat.h>
#include <vlc-server/vs_storage.h>

/*======================================================================
  
  structures

======================================================================*/

struct _LibVlcServerClient;
typedef struct _LibVlcServerClient LibVlcServerClient;

struct _VSPlaylist;
typedef struct _VSPlaylist VSPlaylist;

BEGIN_CDECLS

/*======================================================================
  
  LibVlcServerClient functions 

======================================================================*/
/* NOTE: most of these functions return a error code and a message. 
   The message is dynamically-allocated, and must be freed by the client.
   There will be a message, even if the request were made successfully,
   in which case it will usually be "OK". If the caller does not want
   the message, it should use NULL for the msg parameter, and rely on
   the error code to determine whether the request succeeded. */

LibVlcServerClient *libvlc_server_client_new (const char *host, int port);
void libvlc_server_client_destroy (LibVlcServerClient  *self);

/** Get the server status in a VSServerStat object. The caller must
    destroy this object, if it is not NULL. */
VSServerStat *libvlc_server_client_stat (LibVlcServerClient *self, 
        VSApiError *err_code, char **msg);

/** Get the server's torage status in a VSStorage object. The caller must
    destroy this object, if it is not NULL. */
VSStorage *libvlc_server_client_storage (LibVlcServerClient *self, 
        VSApiError *err_code, char **msg);

/** Get the server's playlist as a VSPlayList object. The caller must
    destroy this object, if it is non-null. */
VSPlaylist *libvlc_server_client_get_playlist 
        (LibVlcServerClient *self, VSApiError *err_code, char **msg);

/** Clear the playlist. */
void libvlc_server_client_clear (LibVlcServerClient *self, 
        VSApiError *err_code, char **msg);

/** Clear playlist, add, and start playback. */
void libvlc_server_client_play (LibVlcServerClient *self, 
        VSApiError *err_code, char **msg, const char *mrl);

/** Play or resume playback. */
void libvlc_server_client_start (LibVlcServerClient *self, 
        VSApiError *err_code, char **msg);

/** Shut down the server. */ 
void libvlc_server_client_shutdown (LibVlcServerClient *self, 
        VSApiError *err_code, char **msg);

/** Start a rescan of the media root. */ 
void libvlc_server_client_scan (LibVlcServerClient *self, 
        VSApiError *err_code, char **msg);

/** Stop playback. Return to the start of the playlist (if any). */
void libvlc_server_client_stop (LibVlcServerClient *self, 
        VSApiError *err_code, char **msg);

/** Pause playback. */
void libvlc_server_client_pause (LibVlcServerClient *self, 
        VSApiError *err_code, char **msg);

/** Toggle play/pause. */
void libvlc_server_client_toggle_pause (LibVlcServerClient *self, 
        VSApiError *err_code, char **msg);

/** Move to the next playlist item. */
void libvlc_server_client_next (LibVlcServerClient *self, 
        VSApiError *err_code, char **msg);

/** Move to the previous playlist item. */
void libvlc_server_client_prev (LibVlcServerClient *self, 
        VSApiError *err_code, char **msg);

/** Add a URL to the playlist. Anything that does not have a colon (:)
    is taken to be a local file. A filename that begins with '@' is
    taken to be relative to the media root.  */
void libvlc_server_client_add_url (LibVlcServerClient *self, 
        VSApiError *err_code, char **msg, const char *url);

/** Sets te audio volume in the range 0-100 */ 
void libvlc_server_client_set_volume (LibVlcServerClient *self, 
        VSApiError *err_code, char **msg, int vol);

/** Increase audio volume by 10% */ 
void libvlc_server_client_volume_up (LibVlcServerClient *self, 
        VSApiError *err_code, char **msg);

/** Decrease audio volume by 10% */ 
void libvlc_server_client_volume_down (LibVlcServerClient *self, 
        VSApiError *err_code, char **msg);

/** Sets the playlist position (first item is 0) */ 
void libvlc_server_client_set_index (LibVlcServerClient *self, 
        VSApiError *err_code, char **msg, int index);

/** List files on the filesystem, under the specified path. */
VSList *libvlc_server_client_list_files 
        (const LibVlcServerClient *selfclient, VSApiError *err_code,
           char **msg, const char *path);

/** List directories on the filesystem, under the specified path. */
VSList *libvlc_server_client_list_dirs
        (const LibVlcServerClient *selfclient, VSApiError *err_code,
           char **msg, const char *path);

/** List albums that match the specified SQL 'where' expression. If
    'where' is NULL, returns all albums. */
VSList *libvlc_server_client_list_albums
        (const LibVlcServerClient *selfclient, const char *where, VSApiError 
           *err_code, char **msg);

/** List artists that match the specified SQL 'where' expression. If
    'where' is NULL, returns all artists. */
VSList *libvlc_server_client_list_artists
        (const LibVlcServerClient *selfclient, const char *where, VSApiError 
           *err_code, char **msg);

/** List composers that match the specified SQL 'where' expression. If
    'where' is NULL, returns all composers. */
VSList *libvlc_server_client_list_composers
        (const LibVlcServerClient *selfclient, const char *where, VSApiError 
           *err_code, char **msg);

/** List genres that match the specified SQL 'where' expression. If
    'where' is NULL, returns all genres. */
VSList *libvlc_server_client_list_genres
        (const LibVlcServerClient *selfclient, const char *where, VSApiError 
           *err_code, char **msg);

/** List tracks that match the specified SQL 'where' expression. If
    'where' is NULL, returns all tracks (which might potentially take
    a long time, and use a lot of memory). */
VSList *libvlc_server_client_list_tracks
        (const LibVlcServerClient *selfclient, const char *where, VSApiError 
           *err_code, char **msg);

/** Convenience function to play an album. Not really necessary, but the
      function exists in the REST API for the benefit of JavaScript
      clients, so there's no extra work involved in providing it here. */
void libvlc_server_client_play_album
        (const LibVlcServerClient *self, VSApiError *err_code,
           char **msg, const char *album);

/** Play a random album. The randomization is done
      in the database, so this functionality can't efficiently be implemented
      by getting a huge list of albums and selecting one of them. */
void libvlc_server_client_play_random_album
        (const LibVlcServerClient *self, VSApiError *err_code, char **msg);

/** Play a random selection of tracks (typically 50). The randomization is done
      in the database, so this functionality can't efficiently be implemented
      by getting a huge list of tracks and selecting some of them. */
void libvlc_server_client_play_random_tracks
        (const LibVlcServerClient *self, VSApiError *err_code, char **msg);

/** Gets the server version, as a char* that the client must
    free. If the return value is NULL, err_code should have been written. */
char *libvlc_server_client_get_version (const LibVlcServerClient *self, 
         VSApiError *err_code, char **msg);

/*======================================================================
  
  Utility functions 

======================================================================*/
/** A helper function for API calls that take an SQL-like 'where'
      argument. Returns the input string with SQL special characters
      escaped. There will always be a return value, and the caller
      must free() it after use. */
char *libvlc_escape_sql (const char *s);

END_CDECLS

