/*======================================================================
  
  vlc-rest-server

  libvlc_server_playlist.h

  Copyright (c)2022 Kevin Boone, GPL v3.0

  This file contains definitions of all the functions and data
  structures provided by the libvlc-server API library.

======================================================================*/

#include <vlc-server/vs_defs.h>

/*======================================================================
  
  structures

======================================================================*/

struct _LibVlcServerPlaylist;
typedef struct _LibVlcServerPlaylist LibVlcServerPlaylist;

BEGIN_CDECLS

/*======================================================================
  
  LibVlcServerPlaylist functions 

======================================================================*/
LibVlcServerPlaylist *libvlc_server_playlist_new (int len);

void libvlc_server_playlist_destroy (LibVlcServerPlaylist *self);

void libvlc_server_playlist_set (LibVlcServerPlaylist *self, 
       int index, const char *mrl);

const char *libvlc_server_playlist_get (const LibVlcServerPlaylist *self, 
       int index);

int libvlc_server_playlist_length (const LibVlcServerPlaylist *self);

END_CDECLS

