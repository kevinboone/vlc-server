/*======================================================================
  
  vlc-rest-server

  api-client.h

  Copyright (c)2022 Kevin Boone, GPL v3.0

  This file contains definitions of all the functions and data
  structures provided by the libvlc-server API library.

======================================================================*/

#include <vlc-server/vs_defs.h>
#include <vlc-server/vs_util.h>
#include <vlc-server/vs_log.h>
#include <vlc-server/vs_list.h>
#include <vlc-server/libvlc_server_playlist.h>
#include <vlc-server/libvlc_server_stat.h>

/*======================================================================
  
  structures

======================================================================*/

struct _LibVlcServerClient;
typedef struct _LibVlcServerClient LibVlcServerClient;

struct _LibVlcServerPlaylist;
typedef struct _LibVlcServerPlaylist LibVlcServerPlaylist;

BEGIN_CDECLS

/*======================================================================
  
  LibVlcServerClient functions 

======================================================================*/

LibVlcServerClient *libvlc_server_client_new (const char *host, int port);
void libvlc_server_client_destroy (LibVlcServerClient  *self);

LibVlcServerStat *libvlc_server_client_stat (LibVlcServerClient *self, 
        VSApiError *err_code, char **msg);

LibVlcServerPlaylist *libvlc_server_client_get_playlist 
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

/** Stop playback. Return to the start of the playlist (if any). */
void libvlc_server_client_stop (LibVlcServerClient *self, 
        VSApiError *err_code, char **msg);

/** Pause playback. */
void libvlc_server_client_pause (LibVlcServerClient *self, 
        VSApiError *err_code, char **msg);

/** Move to the next playlist item. */
void libvlc_server_client_next (LibVlcServerClient *self, 
        VSApiError *err_code, char **msg);

/** Move to the previous playlist item. */
void libvlc_server_client_prev (LibVlcServerClient *self, 
        VSApiError *err_code, char **msg);

/** Add a URL to the playlist. Anything that does not have a colon (:)
    is taken to be a local file. */
void libvlc_server_client_add (LibVlcServerClient *self, 
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
        VSApiError *err_code, char **msg, int vol);

VSList *libvlc_server_client_list_files 
        (const LibVlcServerClient *selfclient, VSApiError *err_code,
           char **msg, const char *path);

VSList *libvlc_server_client_list_dirs
        (const LibVlcServerClient *selfclient, VSApiError *err_code,
           char **msg, const char *path);

END_CDECLS

