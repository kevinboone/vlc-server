/*======================================================================
  
  vlc-server

  vs_playlist.h

  Copyright (c)2022 Kevin Boone, GPL v3.0

  This file contains definitions of all the functions and data
  structures provided by the libvlc-server API library.

======================================================================*/

#include <vlc-server/vs_defs.h>

/*======================================================================
  
  structures

======================================================================*/

struct _VSPlaylist;
typedef struct _VSPlaylist VSPlaylist;

BEGIN_CDECLS

/*======================================================================
  
  VSPlaylist functions 

======================================================================*/
VSPlaylist *vs_playlist_new (int len);

void vs_playlist_destroy (VSPlaylist *self);

void vs_playlist_set (VSPlaylist *self, 
       int index, const char *mrl);

const char *vs_playlist_get (const VSPlaylist *self, 
       int index);

int vs_playlist_length (const VSPlaylist *self);

END_CDECLS

