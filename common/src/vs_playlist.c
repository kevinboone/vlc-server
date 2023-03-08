/*======================================================================
  
  vlc-server

  vs_playlist.c

  Copyright (c)2022 Kevin Boone, GPL v3.0

======================================================================*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vlc-server/vs_playlist.h>

/*======================================================================
  
  VSPlaylist

======================================================================*/
struct _VSPlaylist
  {
  int len;
  char **mrls;
  };

/*======================================================================
  
  libvlc_server_stat_new

======================================================================*/
VSPlaylist *vs_playlist_new (int len)
  {
  VSPlaylist *self = malloc (sizeof (VSPlaylist));
  self->len = len;
  self->mrls = malloc (len * sizeof (char *));
  for (int i = 0; i < len; i++)
    self->mrls[i] = NULL;
  return self;
  }

/*======================================================================
  
  vs_playlist_destroy

======================================================================*/
void vs_playlist_destroy (VSPlaylist *self)
  {
  for (int i = 0; i < self->len; i++)
    {
    if (self->mrls[i]) free (self->mrls[i]);
    }
  if (self->mrls) free (self->mrls);
  free (self);
  }

/*======================================================================
  
  vs_playlist_set

======================================================================*/
void vs_playlist_set (VSPlaylist *self, int index, const char *mrl)
  {
  if (self->mrls[index]) free (self->mrls[index]);
  self->mrls[index] = strdup (mrl);
  }

/*======================================================================
  
  vs_playlist_get

======================================================================*/
const char *vs_playlist_get (const VSPlaylist *self, int index)
  {
  return self->mrls[index];
  }

/*======================================================================
  
  vs_playlist_length

======================================================================*/
int vs_playlist_length (const VSPlaylist *self)
  {
  return self->len;
  }


