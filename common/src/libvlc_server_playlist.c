/*======================================================================
  
  vlc-rest-server

  api/src/libvlc_server_playlist.c

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vlc-server/libvlc_server_playlist.h>

/*======================================================================
  
  LibVlcServerPlaylist

======================================================================*/
struct _LibVlcServerPlaylist
  {
  int len;
  char **mrls;
  };

/*======================================================================
  
  libvlc_server_stat_new

======================================================================*/
LibVlcServerPlaylist *libvlc_server_playlist_new (int len)
  {
  LibVlcServerPlaylist *self = malloc (sizeof (LibVlcServerPlaylist));
  self->len = len;
  self->mrls = malloc (len * sizeof (char *));
  for (int i = 0; i < len; i++)
    self->mrls[i] = NULL;
  return self;
  }

/*======================================================================
  
  libvlc_server_playlist_destroy

======================================================================*/
void libvlc_server_playlist_destroy (LibVlcServerPlaylist *self)
  {
  for (int i = 0; i < self->len; i++)
    {
    if (self->mrls[i]) free (self->mrls[i]);
    }
  if (self->mrls) free (self->mrls);
  free (self);
  }

/*======================================================================
  
  libvlc_server_playlist_set

======================================================================*/
void libvlc_server_playlist_set (LibVlcServerPlaylist *self, 
       int index, const char *mrl)
  {
  if (self->mrls[index]) free (self->mrls[index]);
  self->mrls[index] = strdup (mrl);
  }

/*======================================================================
  
  libvlc_server_playlist_get

======================================================================*/
const char *libvlc_server_playlist_get (const LibVlcServerPlaylist *self, 
       int index)
  {
  return self->mrls[index];
  }

/*======================================================================
  
  libvlc_server_playlist_length

======================================================================*/
int libvlc_server_playlist_length 
        (const LibVlcServerPlaylist *self)
  {
  return self->len;
  }


